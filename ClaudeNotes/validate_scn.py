"""Validator for Ascii3D .scn scene files.

Implements validator rules 1-16 from ClaudeNotes/scene_format_spec_v0.4.md
(section 11). Standard library only.

Usage:
    python ClaudeNotes/validate_scn.py FILE [FILE...]

Prints one line per finding:
    path:line: error|warning: [rule N] message
Exits with status 1 if any file has an error, 0 otherwise.
"""

import re
import sys
from dataclasses import dataclass


# ---------------------------------------------------------------------------
# Tables (compare these with sections 5-9 of the spec)
#
# A parameter is (value type, default). Value types:
#   "float"  one number          "int"   one whole number
#   "bool"   exactly 0 or 1      "vec2"  two numbers     "vec3"  three numbers
#   "name"   one word (only propName)
# Animatable is {propName: dimension}.
# ---------------------------------------------------------------------------

VALUE_COUNT = {"float": 1, "int": 1, "bool": 1, "vec2": 2, "vec3": 3, "name": 1}

COLOR = {"color": ("vec3", "1 1 1")}

SMOOTH_COMB_PARAMS = {
    "smoothing": ("float", "0.5"),
    "colorBlending": ("float", "1.0"),
    "colorShifting": ("float", "0.0"),
}
SMOOTH_COMB_ANIMATABLE = {"smoothing": 1, "colorBlending": 1, "colorShifting": 1}

# Section 5. Plain combinations accept no animations.
COMBINATIONS = {
    "COMBAdd": {
        "params": {
            "onTime": ("float", "1"),
            "offTime": ("float", "0"),
            "phase": ("float", "0"),
            "overwriteColor": ("bool", "0"),
        },
        "animatable": {},
    },
    "COMBSubtract": {"params": {}, "animatable": {}},
    "COMBIntersect": {"params": {}, "animatable": {}},
    "COMBExclude": {"params": {"colorMode": ("int", "0")}, "animatable": {}},
    "COMBAddSmooth": {"params": SMOOTH_COMB_PARAMS, "animatable": SMOOTH_COMB_ANIMATABLE},
    "COMBSubtractSmooth": {"params": SMOOTH_COMB_PARAMS, "animatable": SMOOTH_COMB_ANIMATABLE},
    "COMBIntersectSmooth": {"params": SMOOTH_COMB_PARAMS, "animatable": SMOOTH_COMB_ANIMATABLE},
    "COMBExcludeSmooth": {"params": SMOOTH_COMB_PARAMS, "animatable": SMOOTH_COMB_ANIMATABLE},
}

# Combinations that act on something already in the level (rule 12).
NEEDS_EXISTING_SHAPE = {
    "COMBSubtract", "COMBIntersect", "COMBExclude",
    "COMBSubtractSmooth", "COMBIntersectSmooth", "COMBExcludeSmooth",
}

# Combinations that take no parameters at all (rule 8).
NO_PARAM_COMBINATIONS = {"COMBSubtract", "COMBIntersect"}

AXIS_PARAM = {"axis": ("int", "1")}

TWO_D_PARAMS = {
    "operation": ("int", "0"),
    "magnitude": ("float", "1"),
    "revolveRotations": ("float", "0"),
    "revolveRotateOffset": ("vec2", "0 0"),
}
TWO_D_ANIMATABLE = {"magnitude": 1, "revolveRotations": 1, "revolveRotateOffset": 2}

# Section 6. Every shape also takes color (vec3) and color can be animated (3D).
SHAPES = {
    "SDFObject": {"params": {}, "animatable": {}},
    "SDFPlane": {
        "params": {"normal": ("vec3", "0 1 0"), "offset": ("float", "0")},
        "animatable": {"normal": 3, "offset": 1},
    },
    "SDFBox": {"params": {"dimensions": ("vec3", "1 1 1")}, "animatable": {"dimensions": 3}},
    "SDFBoxFrame": {
        "params": {"dimensions": ("vec3", "1 1 1"), "edgeThickness": ("float", "0.1")},
        "animatable": {"dimensions": 3, "edgeThickness": 1},
    },
    "SDFSphere": {"params": {"radius": ("float", "0.5")}, "animatable": {"radius": 1}},
    "SDFEllipsoid": {"params": {"radii": ("vec3", "1 0.5 0.25")}, "animatable": {"radii": 3}},
    "SDFCrystal": {"params": {"edgeLength": ("float", "1")}, "animatable": {"edgeLength": 1}},
    "SDFCone": {
        "params": {"height": ("float", "1"), "sweep": ("float", "0.5")},
        "animatable": {"height": 1, "sweep": 1},
    },
    "SDFSolidAngle": {
        "params": {"radius": ("float", "0.5"), "sweep": ("float", "0.25")},
        "animatable": {"radius": 1, "sweep": 1},
    },
    # Axis shapes (also take axis, which cannot be animated)
    "SDFTorus": {
        "params": {**AXIS_PARAM, "radii": ("vec2", "1 0.25")},
        "animatable": {"radii": 2},
    },
    "SDFTorusSegment": {
        "params": {**AXIS_PARAM, "radii": ("vec2", "1 0.25"), "sweep": ("float", "0.25")},
        "animatable": {"radii": 2, "sweep": 1},
    },
    "SDFCapsule": {
        "params": {**AXIS_PARAM, "radius": ("float", "0.5"), "bodyLength": ("float", "1")},
        "animatable": {"radius": 1, "bodyLength": 1},
    },
    "SDFCylinder": {
        "params": {**AXIS_PARAM, "radius": ("float", "0.5"), "sideLength": ("float", "1")},
        "animatable": {"radius": 1, "sideLength": 1},
    },
    "SDFTaperedCapsule": {
        "params": {**AXIS_PARAM, "radii": ("vec2", "0.5 0.25"), "bodyLength": ("float", "1")},
        "animatable": {"radii": 2, "bodyLength": 1},
    },
    "SDFTaperedCylinder": {
        "params": {**AXIS_PARAM, "radii": ("vec2", "0.5 0.25"), "sideLength": ("float", "1")},
        "animatable": {"radii": 2, "sideLength": 1},
    },
    # 2D shapes (also take the TWO_D params)
    "SDFRhombus": {
        "params": {**TWO_D_PARAMS, "dimensions": ("vec2", "1 0.5")},
        "animatable": {**TWO_D_ANIMATABLE, "dimensions": 2},
    },
    "SDFTriangle": {
        "params": {
            **TWO_D_PARAMS,
            "point1": ("vec2", "-0.5 -0.5"),
            "point2": ("vec2", "0 0.5"),
            "point3": ("vec2", "0.5 0"),
        },
        "animatable": {**TWO_D_ANIMATABLE, "point1": 2, "point2": 2, "point3": 2},
    },
    "SDFEquilateralTriangle": {
        "params": {**TWO_D_PARAMS, "base": ("float", "1")},
        "animatable": {**TWO_D_ANIMATABLE, "base": 1},
    },
    "SDFIsoscelesTriangle": {
        "params": {**TWO_D_PARAMS, "dimensions": ("vec2", "1 0.5")},
        "animatable": {**TWO_D_ANIMATABLE, "dimensions": 2},
    },
    "SDFPieSlice": {
        "params": {**TWO_D_PARAMS, "sweep": ("float", "0.25"), "radius": ("float", "0.5")},
        "animatable": {**TWO_D_ANIMATABLE, "sweep": 1, "radius": 1},
    },
    "SDFPentagon": {
        "params": {**TWO_D_PARAMS, "radius": ("float", "0.5")},
        "animatable": {**TWO_D_ANIMATABLE, "radius": 1},
    },
    "SDFHexagon": {
        "params": {**TWO_D_PARAMS, "radius": ("float", "0.5")},
        "animatable": {**TWO_D_ANIMATABLE, "radius": 1},
    },
    "SDFOctagon": {
        "params": {**TWO_D_PARAMS, "radius": ("float", "0.5")},
        "animatable": {**TWO_D_ANIMATABLE, "radius": 1},
    },
    "SDFStar": {
        "params": {
            **TWO_D_PARAMS,
            "radius": ("float", "0.5"),
            "numPoints": ("int", "5"),
            "ratio": ("float", "0.5"),
        },
        "animatable": {**TWO_D_ANIMATABLE, "radius": 1, "ratio": 1, "numPoints": 1},
    },
}
for _shape in SHAPES.values():
    _shape["params"] = {**COLOR, **_shape["params"]}
    _shape["animatable"] = {"color": 3, **_shape["animatable"]}

# Section 7.
OPERATIONS = {
    "OPTranslate": {"params": {"translation": ("vec3", "0 1 0")}, "animatable": {"translation": 3}},
    "OPRotate": {
        "params": {"axis": ("vec3", "0 1 0"), "rotations": ("float", "0.5")},
        "animatable": {"rotations": 1, "axis": 3},
    },
    "OPElongate": {
        "params": {"axis": ("int", "1"), "length": ("float", "1"), "offset": ("float", "0")},
        "animatable": {"length": 1, "offset": 1},
    },
    "OPRepeat": {
        "params": {"spacing": ("vec3", "1 1 1"), "limits": ("vec3", "1 1 1")},
        "animatable": {"spacing": 3, "limits": 3},
    },
    "OPRepeatCircle": {
        "params": {"axis": ("int", "1"), "repetitions": ("int", "10")},
        "animatable": {},
    },
    "OPMirror": {
        "params": {"mirrorX": ("bool", "1"), "mirrorY": ("bool", "0"), "mirrorZ": ("bool", "0")},
        "animatable": {},
    },
    "OPRoundOff": {"params": {"magnitude": ("float", "0.01")}, "animatable": {"magnitude": 1}},
    "OPOnion": {"params": {"magnitude": ("float", "0.01")}, "animatable": {"magnitude": 1}},
    "OPInsideOut": {"params": {}, "animatable": {}},
    # Not for the generator (rule 9). OPCubeRoller's "axes N then N vec3s" has a
    # variable length, so its parameters are not checked.
    "OPCubeRollHeight": {
        "params": {"cubeHeight": ("float", "1"), "period": ("float", "1"), "phase": ("float", "0")},
        "animatable": None,
    },
    "OPCubeRoller": {"params": None, "animatable": None},
}

GENERATOR_EXCLUDED = {"OPCubeRollHeight", "OPCubeRoller"}

# Section 8. Numeric params are vec2/vec3 in the 2D/3D variants.
ANIMATION_PARAMS = {
    "ANContinuous": [("amplitude", "1"), ("phase", "0")],
    "ANSmoothCycle": [("period", "1"), ("amplitude", "1"), ("phase", "0")],
    "ANLinearCycle": [
        ("forwardTime", "1"), ("delayTime1", "1"), ("backwardTime", "1"),
        ("delayTime2", "1"), ("amplitude", "1"), ("phase", "0"),
    ],
}
DIM_TYPE = {1: "float", 2: "vec2", 3: "vec3"}

ANIMATIONS = {}
for _base, _params in ANIMATION_PARAMS.items():
    for _dim in (1, 2, 3):
        ANIMATIONS[f"{_base}{_dim}D"] = {
            "dim": _dim,
            "params": {
                "propName": ("name", None),
                **{name: (DIM_TYPE[_dim], " ".join([d] * _dim)) for name, d in _params},
            },
        }

# Section 9.
LIGHT_PARAMS = {
    "position": ("vec3", "5 5 -3"),
    "luminance": ("float", "100"),
    "ambientPower": ("float", "0.2"),
    "color": ("vec3", "1 1 1"),
}

# Rule 9 ("every parameter is written explicitly") skips these. The spec's own
# examples leave them out: COMBAdd defaults mean "always visible, normal
# color", and an empty group's color has no visible effect.
EXPLICIT_EXEMPT = {
    "COMBAdd": {"onTime", "offTime", "phase", "overwriteColor"},
    "SDFObject": {"color"},
}

# Rule 7 ranges for int params: {param: (min, max)}; None means no upper limit.
INT_RANGES = {
    "axis": (0, 2),
    "operation": (0, 2),
    "colorMode": (0, 3),
    "repetitions": (1, None),
    "numPoints": (1, None),
}

# Rule 7: vectors that must not be all zeros.
NONZERO_VECTORS = {("SDFPlane", "normal"), ("OPRotate", "axis")}

# Rule 15: properties that are accepted but should not be animated.
ANIMATION_HAZARDS = {
    ("SDFPlane", "normal"): ("warning", "an animated plane normal is not re-normalized; avoid it"),
    ("OPRotate", "axis"): ("warning", "an animated rotation axis is not re-normalized; avoid it"),
    ("OPRotate", "rotMat"): ("error", "rotMat is internal and overwritten every frame; never animate it"),
    ("OPRepeatCircle", "repetitions"): (
        "error",
        "never animate repetitions: it is a whole-number field and the animation "
        "writes a decimal into it (memory corruption)",
    ),
}

STEP_BACK = "stepBack"
LIGHT = "LightSource"
END = "end"
COMMENT = "//"

KEYWORDS = (
    set(COMBINATIONS) | set(SHAPES) | set(OPERATIONS) | set(ANIMATIONS) | {STEP_BACK, LIGHT}
)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

NUMBER_RE = re.compile(r"^[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?$")
INT_RE = re.compile(r"^[+-]?\d+$")


@dataclass
class Finding:
    line: int
    severity: str  # "error" or "warning"
    rule: int
    message: str

    def format(self, path):
        return f"{path}:{self.line}: {self.severity}: [rule {self.rule}] {self.message}"


def kind_of(keyword):
    if keyword in COMBINATIONS:
        return "comb"
    if keyword in SHAPES:
        return "shape"
    if keyword in OPERATIONS:
        return "op"
    if keyword in ANIMATIONS:
        return "anim"
    if keyword == STEP_BACK:
        return "stepBack"
    if keyword == LIGHT:
        return "light"
    return None


def params_for(keyword):
    """Parameter table for a keyword, or None if its parameters are not checked."""
    for table in (COMBINATIONS, SHAPES, OPERATIONS, ANIMATIONS):
        if keyword in table:
            return table[keyword]["params"]
    if keyword == LIGHT:
        return LIGHT_PARAMS
    if keyword == STEP_BACK:
        return {}
    return None


def animatable_for(keyword):
    """{propName: dimension} for a keyword, or None if unknown."""
    for table in (COMBINATIONS, SHAPES, OPERATIONS):
        if keyword in table:
            return table[keyword]["animatable"]
    return None


def edit_distance(a, b):
    previous = list(range(len(b) + 1))
    for i, ca in enumerate(a, 1):
        current = [i]
        for j, cb in enumerate(b, 1):
            current.append(min(previous[j] + 1, current[j - 1] + 1, previous[j - 1] + (ca != cb)))
        previous = current
    return previous[-1]


def near_miss(word, candidates):
    """Closest candidate if it differs only by case or by at most 2 edits."""
    for candidate in sorted(candidates):
        if candidate.lower() == word.lower():
            return candidate
    best = min(sorted(candidates), key=lambda c: edit_distance(word, c), default=None)
    if best is not None and edit_distance(word, best) <= 2:
        return best
    return None


def is_comment(token):
    return token.startswith(COMMENT)


# ---------------------------------------------------------------------------
# Validation
# ---------------------------------------------------------------------------

class Validator:
    def __init__(self, text):
        # Accept LF or CRLF line endings; a final newline does not add a line.
        self.lines = [line.rstrip("\r") for line in text.split("\n")]
        if len(self.lines) > 1 and self.lines[-1] == "":
            self.lines.pop()
        self.findings = []

    def add(self, line, severity, rule, message):
        self.findings.append(Finding(line, severity, rule, message))

    def run(self):
        self.check_line_one()
        entries = []  # (line number, keyword, tokens) for keyword lines from line 2 on
        end_line = None
        for number, raw in enumerate(self.lines, 1):
            self.check_lexical(number, raw)
            tokens = raw.split()
            if number == 1 or not tokens or is_comment(tokens[0]):
                continue
            keyword = tokens[0]
            if end_line is not None:
                self.add(number, "error", 4, f"'end' on line {end_line} must be the last line")
            if keyword == END:
                end_line = number
                if len(tokens) > 1:
                    self.add(number, "error", 4, "'end' must be alone on its line")
                continue
            if keyword not in KEYWORDS:
                self.report_unknown_keyword(number, keyword)
                continue
            self.check_params(number, keyword, tokens[1:])
            entries.append((number, keyword, tokens[1:]))
        self.check_structure(entries)
        self.findings.sort(key=lambda f: (f.line, f.rule))
        return self.findings

    # Rule 1 -----------------------------------------------------------------

    def check_line_one(self):
        first = self.lines[0] if self.lines else ""
        tokens = first.split()
        if first == "SDFObject":
            return
        if tokens and tokens[0] == "SDFObject":
            self.add(1, "warning", 1,
                     "line 1 should be exactly 'SDFObject'; the loader never reads line 1, "
                     "so these parameters do nothing")
        else:
            self.add(1, "error", 1,
                     "line 1 must be exactly 'SDFObject'; the loader never reads line 1, "
                     "so whatever is written here is lost")

    # Rules 2-3 --------------------------------------------------------------

    def check_lexical(self, number, raw):
        if raw.strip() == "":
            self.add(number, "error", 2, "blank or whitespace-only line")
            return
        if raw != raw.rstrip():
            self.add(number, "error", 2, "trailing whitespace")
        if "\t" in raw.lstrip("\t"):
            self.add(number, "error", 2, "tab used other than as leading indentation")
        if "," in raw and not is_comment(raw.split()[0]):
            self.add(number, "error", 2,
                     "commas are not part of the format; separate values with spaces "
                     "(the loader drops the rest of the line at a comma)")

    def report_unknown_keyword(self, number, keyword):
        guess = near_miss(keyword, KEYWORDS | {END})
        if guess:
            self.add(number, "error", 3,
                     f"unknown keyword '{keyword}' (did you mean '{guess}'?); "
                     "the loader silently skips this line")
        else:
            self.add(number, "error", 3,
                     f"unknown keyword '{keyword}'; the loader silently skips this line "
                     "(use '// text' for comments)")

    # Rules 5-9 --------------------------------------------------------------

    def check_params(self, number, keyword, args):
        if keyword in GENERATOR_EXCLUDED:
            self.add(number, "warning", 9,
                     f"{keyword} is undocumented and not allowed in generated scenes")
        table = params_for(keyword)
        if table is None:
            return
        if keyword in NO_PARAM_COMBINATIONS and args:
            self.add(number, "warning", 8,
                     f"{keyword} takes no parameters; '{' '.join(args)}' is ignored")
            return

        seen = {}
        i = 0
        while i < len(args):
            name = args[i]
            i += 1
            if name not in table:
                guess = near_miss(name, table)
                hint = f" (did you mean '{guess}'?)" if guess else ""
                if keyword == "COMBExclude" and name in COMBINATIONS["COMBAdd"]["params"]:
                    self.add(number, "warning", 8,
                             f"COMBExclude only takes colorMode; '{name}' is ignored")
                elif table:
                    self.add(number, "error", 5,
                             f"'{name}' is not a parameter of {keyword}{hint}; "
                             f"valid: {', '.join(table)}")
                else:
                    self.add(number, "error", 5, f"{keyword} takes no parameters, got '{name}'")
                # Skip the values that belong to the unknown name.
                while i < len(args) and NUMBER_RE.match(args[i]):
                    i += 1
                continue

            value_type, _default = table[name]
            if name in seen:
                self.add(number, "error", 5,
                         f"'{name}' is repeated (first given earlier on this line)")
            if value_type == "name":
                if i >= len(args):
                    self.add(number, "error", 5, f"'{name}' needs a value")
                    seen[name] = None
                else:
                    seen[name] = args[i]
                    i += 1
                continue

            # Take numbers up to the next parameter name. A non-number is taken
            # only while values are still missing, so a misspelled name that
            # follows a complete value is reported as a name.
            expected = VALUE_COUNT[value_type]
            values = []
            while i < len(args) and args[i] not in table and (
                NUMBER_RE.match(args[i]) or len(values) < expected
            ):
                values.append(args[i])
                i += 1
            seen.setdefault(name, values)
            self.check_values(number, keyword, name, value_type, values)

        self.check_explicit(number, keyword, table, seen)

    def check_values(self, number, keyword, name, value_type, values):
        expected = VALUE_COUNT[value_type]
        if len(values) != expected:
            plural = "value" if expected == 1 else "values"
            self.add(number, "error", 5,
                     f"'{name}' needs exactly {expected} {plural}, got {len(values)}")
        bad = [v for v in values if not NUMBER_RE.match(v)]
        if bad:
            self.add(number, "error", 6,
                     f"'{name}' value '{bad[0]}' is not a number "
                     "(the loader drops the rest of the line here)")
            return
        if len(values) != expected:
            return
        if value_type == "int":
            if not INT_RE.match(values[0]):
                self.add(number, "error", 6, f"'{name}' must be a whole number, got '{values[0]}'")
                return
            if name in INT_RANGES:
                low, high = INT_RANGES[name]
                value = int(values[0])
                if value < low or (high is not None and value > high):
                    allowed = f"{low}-{high}" if high is not None else f">= {low}"
                    self.add(number, "error", 7, f"'{name}' must be {allowed}, got {value}")
        elif value_type == "bool":
            if values[0] not in ("0", "1"):
                self.add(number, "error", 6, f"'{name}' must be exactly 0 or 1, got '{values[0]}'")
        if (keyword, name) in NONZERO_VECTORS and all(float(v) == 0 for v in values):
            self.add(number, "error", 7, f"{keyword} '{name}' must not be 0 0 0")

    def check_explicit(self, number, keyword, table, seen):
        exempt = EXPLICIT_EXEMPT.get(keyword, set())
        missing = [name for name in table if name not in seen and name not in exempt]
        if keyword in ANIMATIONS:
            missing = [name for name in missing if name != "propName"]  # rule 15 reports it
        if missing:
            defaults = ", ".join(
                f"{name} {table[name][1]}" for name in missing if table[name][1] is not None
            )
            self.add(number, "warning", 9,
                     f"write every parameter explicitly; missing: {', '.join(missing)} "
                     f"(defaults used: {defaults})")

    # Rules 10-16 ------------------------------------------------------------

    def check_structure(self, entries):
        # Each level: {"keyword", "line", "has_shape"}. The root is an empty group.
        levels = [{"keyword": "SDFObject", "line": 1, "has_shape": False}]
        pending_comb = None   # (line, keyword) of a combination still waiting for its shape
        previous = ("root", 1, "SDFObject")  # (kind, line, keyword) of last meaningful line
        anim_target = None    # (line, keyword) the last animation bound to, or None
        animated = {}         # (target line, propName) -> line of the animation
        first_light = None
        stack_ended_checked = False

        def close_stack(line):
            nonlocal stack_ended_checked, pending_comb
            if stack_ended_checked:
                return
            stack_ended_checked = True
            if pending_comb:
                self.report_lonely_comb(pending_comb)
                pending_comb = None
            open_levels = len(levels) - 1
            if open_levels:
                self.add(line, "warning", 11,
                         f"{open_levels} level(s) still open at the end of the stack lines "
                         f"(innermost opened on line {levels[-1]['line']}); "
                         "the loader closes them implicitly")

        for number, keyword, args in entries:
            kind = kind_of(keyword)
            params = self.parse_quietly(keyword, args)

            # Rule 13: lights come after every stack line.
            if kind == "light":
                if first_light is None:
                    first_light = number
                    close_stack(number)
            elif first_light is not None:
                self.add(number, "error", 13,
                         f"stack line after the first LightSource (line {first_light}); "
                         "put all LightSource lines at the end")

            # Rule 10: a combination must be followed by exactly one shape.
            if pending_comb and kind in ("comb", "op", "stepBack", "light"):
                self.report_lonely_comb(pending_comb)
                pending_comb = None

            if kind == "comb":
                if keyword in NEEDS_EXISTING_SHAPE and not levels[-1]["has_shape"]:
                    self.add(number, "error", 12,
                             f"{keyword} needs an earlier shape in the same level to act on, "
                             f"but the level opened on line {levels[-1]['line']} is still empty")
                pending_comb = (number, keyword)
                anim_target = (number, keyword)
            elif kind == "shape":
                if pending_comb is None:
                    self.add(number, "error", 10,
                             f"{keyword} has no combination line (COMB*) directly before it; "
                             "the loader skips this shape, and its stepBack then closes the "
                             "parent level instead")
                pending_comb = None
                levels[-1]["has_shape"] = True
                levels.append({
                    "keyword": keyword,
                    "line": number,
                    "has_shape": keyword != "SDFObject",  # a real shape is something to act on
                })
                anim_target = (number, keyword)
            elif kind == "op":
                anim_target = (number, keyword)
            elif kind == "stepBack":
                if len(levels) == 1:
                    self.add(number, "warning", 11,
                             "stepBack at root depth does nothing (the loader ignores it); "
                             "check the level count")
                else:
                    levels.pop()
                anim_target = None
            elif kind == "light":
                anim_target = None
            elif kind == "anim":
                target = self.check_animation_position(number, keyword, previous, anim_target)
                if target:
                    self.check_animation_property(number, keyword, params, target, animated)
                anim_target = target  # an invalid position stays invalid for the next AN*

            previous = (kind, number, keyword)

        close_stack(entries[-1][0] if entries else len(self.lines))
        if first_light is None:
            self.add(len(self.lines) or 1, "error", 13, "the scene needs at least one LightSource")

    def report_lonely_comb(self, comb):
        line, keyword = comb
        self.add(line, "error", 10,
                 f"{keyword} is not followed by a shape; a combination without a shape "
                 "crashes the program")

    def check_animation_position(self, number, keyword, previous, anim_target):
        kind, prev_line, prev_keyword = previous
        if kind in ("shape", "op", "comb"):
            return anim_target
        if kind == "anim":
            return anim_target  # same target as the animation before it
        if kind == "root":
            where = "line 1 (the root, which the loader never reads)"
        elif kind == "stepBack":
            where = f"a stepBack (line {prev_line}); it would bind to the parent shape"
        else:
            where = f"a LightSource (line {prev_line}); it would bind to whatever came before"
        self.add(number, "error", 14,
                 f"{keyword} must follow a shape, OP*, COMB* or AN* line, not {where}")
        return None

    def check_animation_property(self, number, keyword, params, target, animated):
        target_line, target_keyword = target
        dim = ANIMATIONS[keyword]["dim"]
        prop = params.get("propName")
        if not prop:
            self.add(number, "error", 15,
                     f"{keyword} has no propName; the loader drops the animation")
            return
        hazard = ANIMATION_HAZARDS.get((target_keyword, prop))
        animatable = animatable_for(target_keyword)
        if animatable is None:
            return  # target not in the tables (OPCubeRoll*); nothing to check against
        if hazard and hazard[0] == "error":
            self.add(number, "error", 15, f"{target_keyword} (line {target_line}): {hazard[1]}")
            return
        if not animatable:
            self.add(number, "error", 15,
                     f"{target_keyword} (line {target_line}) cannot be animated; "
                     "the animation is dropped or misapplied")
            return
        if prop not in animatable:
            guess = near_miss(prop, animatable)
            hint = f" (did you mean '{guess}'?)" if guess else ""
            self.add(number, "error", 15,
                     f"'{prop}' is not animatable on {target_keyword} (line {target_line}){hint}; "
                     f"valid: {', '.join(animatable)}")
            return
        needed = animatable[prop]
        if dim != needed:
            if dim > needed:
                effect = "writes past the end of the property and corrupts memory"
            elif needed == 3:
                effect = "animates only the first component, and the GLSL shader fails to compile"
            else:
                effect = "animates only part of it, and the GLSL shader may fail to compile"
            self.add(number, "error", 15,
                     f"'{prop}' on {target_keyword} is {needed}D but {keyword} is {dim}D; "
                     f"this {effect}. Use the {needed}D variant")
        if hazard:
            self.add(number, hazard[0], 15, f"{target_keyword} (line {target_line}): {hazard[1]}")

        # Rule 16
        key = (target_line, prop)
        if key in animated:
            self.add(number, "error", 16,
                     f"'{prop}' on {target_keyword} (line {target_line}) is already animated "
                     f"on line {animated[key]}; only the later animation takes effect")
        else:
            animated[key] = number

    @staticmethod
    def parse_quietly(keyword, args):
        """{param: first value token} without reporting anything (used for propName)."""
        table = params_for(keyword) or {}
        result = {}
        for i, token in enumerate(args):
            if token in table and token not in result and i + 1 < len(args):
                result[token] = args[i + 1]
        return result


def validate_text(text):
    return Validator(text).run()


def validate_file(path):
    with open(path, encoding="utf-8", errors="replace", newline="") as f:
        return validate_text(f.read())


def main(argv):
    if not argv:
        print(__doc__.strip(), file=sys.stderr)
        return 2
    any_error = False
    for path in argv:
        try:
            findings = validate_file(path)
        except OSError as e:
            print(f"{path}: error: cannot read file: {e.strerror}")
            any_error = True
            continue
        for finding in findings:
            print(finding.format(path))
        any_error |= any(f.severity == "error" for f in findings)
    return 1 if any_error else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
