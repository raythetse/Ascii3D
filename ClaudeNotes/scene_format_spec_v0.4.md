# .scn scene format: spec for the scene-generator prompt (v0.4, DRAFT)

Draft 2026-11-02, revised 2026-11-03, for review. Supersedes v0.3. Sources:
HOWTO.txt, SDFBuilder.cpp/.h, CombinationOperation.*, SDFObject.*,
SDFAxisObject.h, SDF2DObject.*, TransformOperation.*, DomainOperation.*,
ResurfaceOperation.*, Animation.h, LightSource.h, Vec3.h, as of commit d0fb8f1,
plus the 2026-11-04 loader fixes (branch SceneFileValidator).

**Strict subset.** The loader accepts more than this spec allows. For many
mistakes it now prints a warning with the line number and recovers, but some
are still ignored silently, and it never rejects a file (Appendix A). The
generator and the validator follow the stricter rules here, so every file
means exactly what it says. A file that makes the loader warn is invalid.

Changes from v0.3 are listed in Appendix B.

## 1. Lexical rules (strict subset)

- Plain text, one entry per line. LF or CRLF line endings are both fine.
- **No blank lines and no whitespace-only lines.** (The loader ignores them;
  this is a style rule.)
- **No trailing whitespace.** (The loader ignores it; this is a style rule.)
- Leading tabs for indentation are allowed and ignored. Use them for readability.
- Separate tokens with single spaces. **Commas are not part of the format.**
  A comma breaks the value it is in, and the rest of that line is dropped
  with a warning (`color 1,0,0` loads as 1 0 1).
- Keywords and parameter names are case sensitive.
- A line whose first token is not a keyword is ignored. Comments use `// text`.
  The loader warns about a misspelled keyword but does not suggest the
  intended one, so the validator must flag near-misses. A misspelled
  parameter name is still dropped silently.
- The optional last line `end` (the serializer writes it) is ignored like a comment.
- A line is `Keyword` followed by zero or more `paramName value...` pairs, in any
  order. Each parameter gets exactly its value count (vec3 = 3, vec2 = 2). Never
  repeat a parameter on a line.
- Numbers: plain decimals (`1`, `0.5`, `-2.25`). Float fields accept integers.
  Int fields (`axis`, `operation`, `colorMode`, `repetitions`, `numPoints`) must
  be integers. Bools must be exactly `0` or `1`.

## 2. Scene conventions

- Units are meters. Time is in seconds. Rotations are in full turns (1.0 = 360 degrees).
  Colors are `r g b`, each 0.0-1.0. `sweep` values are fractions of a full turn (0-1).
- Camera starts at (0, 1.8, -5) looking along +z. The user can move it while the
  scene runs, so a scene must look right from the start position. Max draw distance is 100 m.
- Axes: +x right, +y up, +z away from the camera (left-handed). Horizontal FOV is
  about 90 degrees, so at z = 5 the visible width is roughly x in +/-10.
- Floor: `SDFPlane normal 0 1 0 offset 0`. Keep content at about z = 0 to 15, y = 0 to 4.
- Int enums: axis 0 = x, 1 = y, 2 = z; 2D `operation` 0 = extrude, 1 = revolve,
  2 = infinite column.

## 3. File structure

```
SDFObject                 <- line 1, exactly this
	...stack entries...   <- COMB/SDF/OP/AN/stepBack lines
LightSource ...           <- one or more, at the end
```

- **Line 1 must be exactly `SDFObject`.** The loader never parses line 1. It
  always creates its own white root, so parameters on line 1 do nothing, and if
  line 1 is real content, that content is lost.
- **Never emit a `stepBack` at root depth.** The loader ignores it, so it does
  nothing, but it usually means the levels were miscounted. The serializer
  writes one as a trailing line before the lights; the validator warns about it
  instead of rejecting the file.
- `LightSource` lines are not stack entries. Put them all after the last stack line.

## 4. The stack model

1. A shape line (any `SDF*`, including `SDFObject` used as a group) opens a new
   level on top of the stack. `stepBack` closes the top level. Operations belong
   to the level they are written in.
2. **Every non-root shape is introduced by exactly one combination line,** written
   directly before it. Only `AN*` lines (and comments) may sit between the
   combination and its shape. A combination with no shape is dropped with a
   warning (Section 5). A shape without its own combination is skipped with a
   warning, together with everything up to its own closing `stepBack`.
3. Transform/domain operations (`OPTranslate`, `OPRotate`, `OPElongate`,
   `OPRepeat`, `OPRepeatCircle`, `OPMirror`) are written BEFORE the shapes they
   affect. They apply to every shape added after them in the same level,
   including nested ones.
4. Resurface operations (`OPRoundOff`, `OPOnion`, `OPInsideOut`) go AFTER the
   shape, inside its level, and act on what is already there. Wrap the shape in
   its own group to keep the effect local.
5. Balance: each shape gets exactly one closing `stepBack`, except the root,
   which gets none. Depth must never drop below the root.
6. **Safe pattern for an independently positioned part:** wrap it in an empty
   `SDFObject` group, then close the shape and the group:

```
	COMBAdd
	SDFObject
		OPTranslate translation 0 1 3
		COMBAdd
		SDFSphere radius 1 color 1 0 0
			stepBack
		stepBack
```

7. Onion shell that only shows where a later subtraction cuts into it:

```
	COMBAdd
	SDFObject
		COMBAdd
		SDFSphere radius 1 color 1 0.5 0
			OPOnion magnitude 0.1
			stepBack
		stepBack
	OPTranslate translation 0 0 -0.5
	COMBSubtract
	SDFBox dimensions 2 2 1
		stepBack
```

   Note: the `OPTranslate` here is at root level, so it also moves every shape
   added to the root after it. Wrap the subtract part in a group if that matters.

8. Nesting without a wrapper (a sphere attached to the top of a box):

```
	COMBAdd
	SDFBox dimensions 2 1 0.5 color 0 0 1
		OPTranslate translation 0 1 0
		COMBAdd
		SDFSphere radius 0.75 color 1 0 0
			stepBack
		stepBack
```

## 5. Combinations

| Keyword | Params (default) |
|---|---|
| `COMBAdd` | `onTime` 1, `offTime` 0, `phase` 0, `overwriteColor` 0 |
| `COMBSubtract` | none (any params are ignored) |
| `COMBIntersect` | none (any params are ignored) |
| `COMBExclude` | `colorMode` 0 |
| `COMBAddSmooth`, `COMBSubtractSmooth`, `COMBIntersectSmooth`, `COMBExcludeSmooth` | `smoothing` 0.5, `colorBlending` 1.0, `colorShifting` 0.0 |

- `COMBAdd` blinking: if `offTime` > 0, the shape is visible for `onTime` s, then
  hidden for `offTime` s, shifted by `phase`.
- `overwriteColor 1`: the added shape's color always wins.
- `COMBExclude colorMode`: 0 each shape keeps its color; 1 shapes take the
  original's color; 2 inner surfaces get the added color, outer keep the original;
  3 like 2, with colors swapped by proximity.
- Subtract, Intersect and Exclude (and their smooth versions) need something
  already present in the same level to act on.
- **Combination with no shape:** the loader drops it with a warning. This
  covers `COMBAdd COMBAdd`, a `COMB*` at the end of the stack, and a `COMB*`
  followed by an `OP*` line. The renderer itself would still crash on one:
  `update()`, `combine()`, `serialize()`, `resetAnimations()` and shader
  generation all dereference the combination's object without a null check.
- Animations: plain combinations accept **none**. `CombinationOperation::bindAnimation`
  is empty, so `onTime`/`offTime`/`phase` cannot be animated and the animation is
  dropped. Smooth combinations accept 1D animations on `smoothing`,
  `colorBlending`, `colorShifting`.

## 6. Shapes

All shapes accept `color` (vec3, default 1 1 1) and an animation on `color` (3D).
"Animatable" lists the valid `propName` values and the animation dimension each needs.

| Keyword | Params (default) | Animatable |
|---|---|---|
| `SDFObject` | none (empty group) | color 3D |
| `SDFPlane` | `normal` vec3 (0 1 0, auto-normalized; must be non-zero), `offset` 0 | normal 3D*, offset 1D |
| `SDFBox` | `dimensions` vec3 (1 1 1) | dimensions 3D |
| `SDFBoxFrame` | `dimensions` vec3 (1 1 1), `edgeThickness` 0.1 | dimensions 3D, edgeThickness 1D |
| `SDFSphere` | `radius` 0.5 | radius 1D |
| `SDFEllipsoid` | `radii` vec3 (1 0.5 0.25) | radii 3D |
| `SDFCrystal` | `edgeLength` 1 | edgeLength 1D |
| `SDFCone` | `height` 1, `sweep` 0.5 | height 1D, sweep 1D |
| `SDFSolidAngle` | `radius` 0.5, `sweep` 0.25 | radius 1D, sweep 1D |

\* An animated plane normal is not re-normalized in the C++ path. Avoid it.

Axis shapes also take `axis` int (default 1 = y). `axis` cannot be animated.

| Keyword | Params (default) | Animatable |
|---|---|---|
| `SDFTorus` | `radii` vec2 (1 0.25: major, minor) | radii 2D |
| `SDFTorusSegment` | `radii` vec2 (1 0.25), `sweep` 0.25 | radii 2D, sweep 1D |
| `SDFCapsule` | `radius` 0.5, `bodyLength` 1 | radius 1D, bodyLength 1D |
| `SDFCylinder` | `radius` 0.5, `sideLength` 1 | radius 1D, sideLength 1D |
| `SDFTaperedCapsule` | `radii` vec2 (0.5 0.25), `bodyLength` 1 | radii 2D, bodyLength 1D |
| `SDFTaperedCylinder` | `radii` vec2 (0.5 0.25), `sideLength` 1 | radii 2D, sideLength 1D |

`SDFTaperedCylinder` uses `sideLength` (not `bodyLength`).

2D shapes (extruded or revolved). Common params: `operation` int (0), `magnitude` 1
(extrude height, or revolve ring radius), `revolveRotations` 0,
`revolveRotateOffset` vec2 (0 0). Common animatable: magnitude 1D,
revolveRotations 1D, revolveRotateOffset 2D. `operation` cannot be animated.

| Keyword | Params (default) | Animatable |
|---|---|---|
| `SDFRhombus` | `dimensions` vec2 (1 0.5) | dimensions 2D |
| `SDFTriangle` | `point1` (-0.5 -0.5), `point2` (0 0.5), `point3` (0.5 0), each vec2 | point1/2/3 2D |
| `SDFEquilateralTriangle` | `base` 1 | base 1D |
| `SDFIsoscelesTriangle` | `dimensions` vec2 (1 0.5) | dimensions 2D |
| `SDFPieSlice` | `sweep` 0.25, `radius` 0.5 | sweep 1D, radius 1D |
| `SDFPentagon`, `SDFHexagon`, `SDFOctagon` | `radius` 0.5 | radius 1D |
| `SDFStar` | `radius` 0.5, `numPoints` int (5, >= 1), `ratio` 0.5 | radius 1D, ratio 1D, numPoints 1D |

## 7. Operations

| Keyword | Params (default) | Animatable |
|---|---|---|
| `OPTranslate` | `translation` vec3 (**0 1 0**) | translation 3D |
| `OPRotate` | `axis` vec3 (0 1 0, auto-normalized; must be non-zero), `rotations` 0.5 | rotations 1D, axis 3D* |
| `OPElongate` | `axis` int (1), `length` 1, `offset` 0 | length 1D, offset 1D |
| `OPRepeat` | `spacing` vec3 (1 1 1), `limits` vec3 (1 1 1) | spacing 3D, limits 3D |
| `OPRepeatCircle` | `axis` int (1), `repetitions` int (10, >= 1) | none (see below) |
| `OPMirror` | `mirrorX` 1, `mirrorY` 0, `mirrorZ` 0 | none |
| `OPRoundOff` | `magnitude` 0.01 | magnitude 1D |
| `OPOnion` | `magnitude` 0.01 (shell thickness; stackable) | magnitude 1D |
| `OPInsideOut` | none | none |

- **Defaults are not neutral.** A bare `OPTranslate` moves up 1 m, a bare
  `OPRotate` turns half a turn about y, and a bare `OPRepeat` makes 3x3x3 = 27 copies.
  Always write every parameter.
- \* An animated rotation axis is not re-normalized before use. Avoid it.
- `OPRepeat limits a b c`: N copies in BOTH directions on each axis, so
  `2 0 1` gives 5 x 1 x 3 = 15 copies. Keep repeated shapes centered and symmetrical.
- `OPRepeatCircle`: follow it with an `OPTranslate` purely along x to set the
  circle's radius. Copies are first re-oriented to the target axis. **Never animate
  `repetitions`.** It is a `size_t`, and the animation writes a double into it
  (memory corruption).
- `OPRoundOff` looks poor combined with subtractions.
- `rotMat` is accepted by `OPRotate::bindAnimation` but is internal and overwritten
  every frame. Never use it.
- Not for the generator: `OPCubeRollHeight` (`cubeHeight` 1, `period` 1, `phase` 0)
  and `OPCubeRoller` (`axes` N then N vec3s, default fwd/right/back/left;
  `quarterTurnTime` 0.5, `phase` 0). They are undocumented and used by the walking-cubes demo.

## 8. Animations

| Keyword | Params (default) |
|---|---|
| `ANContinuous{1,2,3}D` | `propName`, `amplitude` 1 (change per second), `phase` 0 |
| `ANSmoothCycle{1,2,3}D` | `propName`, `period` 1, `amplitude` 1, `phase` 0 (sine) |
| `ANLinearCycle{1,2,3}D` | `propName`, `forwardTime` 1, `delayTime1` 1, `backwardTime` 1, `delayTime2` 1, `amplitude` 1, `phase` 0 (a time of 0 snaps) |

In 2D/3D variants every numeric param is a vec2/vec3 (one value per component).

**Attachment rule** (from `SDFBuilder::animate`):
- After a shape line, the animation binds to that shape.
- After an `OP*` or `COMB*` line, it binds to that operation.
- After another `AN*` line, it binds to the same target as that animation.
- After `stepBack` or `LightSource`, it has no target and is dropped with a
  warning. **Never put an animation there.**
- Never animate the root (line 1 is not parsed anyway).

**Validity:**
- `propName` must be in the target's Animatable list (Sections 5-7). An unknown
  or missing `propName` drops the animation silently.
- **The dimension must match the property.** `BindToProperty` is an unchecked
  `reinterpret_cast`. A 3D animation on a 1D property (e.g. `ANContinuous3D
  propName radius`) writes past the field and corrupts memory. A 1D animation
  on a vec3 animates only the first component in the C++ path, and the GLSL
  shader fails to compile (confirmed 2026-11-03).
- Never put two animations on the same property of the same target. The later one wins.
- Lights cannot be animated.

## 9. Lights

`LightSource position x y z luminance L ambientPower a color r g b`

| Param | Default |
|---|---|
| `position` vec3 | 5 5 -3 |
| `luminance` | 100 |
| `ambientPower` (0-1) | 0.2 |
| `color` vec3 | 1 1 1 |

- Brightness falls off as luminance / distance squared (double the distance needs 4x the power).
- HOWTO guidance: luminance 150 gives decent light at 10 m. Ambient 0.1-0.2 for
  a nearby light, 0.4-0.5 for a large distant light (outdoor scene).
- Starting point for a normally lit scene: luminance = 1.5 x d^2 (d = distance
  in meters from the light to the main subject). This is a guideline for a
  decent amount of light, not a requirement. Artistic intent overrides it.
  Go lower for dim or moody scenes and higher for bright ones. If the user's
  request gives no lighting mood, use the guideline.
- Example of intentional low light: Jack_O_Lantern's main light is luminance
  50 at about 11 m with ambient 0.5. That is well below the guideline (about
  180), which is by design. The high ambient keeps shapes readable in the dark.
- At least one light is required.

## 10. Worked examples

All three are balanced, have no root-level `stepBack`, and follow Section 1.

Animated torus:
```
SDFObject
	OPRotate axis 0.5 1 0.875 rotations 0.125
	ANContinuous1D propName rotations amplitude 0.1 phase 0
	COMBAdd
	SDFTorus axis 1 radii 1.5 0.25 color 0 0.7 1
		stepBack
LightSource position 2 5 -3 luminance 150 ambientPower 0.2 color 1 1 1
```

Ring of spheres:
```
SDFObject
	OPRepeatCircle axis 2 repetitions 6
	OPTranslate translation 2 0 0
	COMBAdd
	SDFSphere radius 0.4 color 1 0.5 0
		stepBack
LightSource position 2 5 -3 luminance 150 ambientPower 0.2 color 1 1 1
```

Note: both examples put operations at root level, so they move the whole scene.
That is fine for a one-object scene. For more objects, use the wrapper pattern.

Complete scene (ground, red sphere, blue box). Rendered and confirmed correct
(2026-11-03, from CustomScenes/CustomScene.scn). Safe to use as a prompt example.
```
SDFObject
	COMBAdd
	SDFObject
		COMBAdd
		SDFPlane color 0.4 0.4 0.4 normal 0 1 0 offset 0
			stepBack
		stepBack
	COMBAdd
	SDFObject
		OPTranslate translation 0 1 3
		COMBAdd
		SDFSphere radius 1 color 1 0 0
			stepBack
		stepBack
	COMBAdd
	SDFObject
		OPTranslate translation 2.5 0.5 3
		COMBAdd
		SDFBox dimensions 1 1 1 color 0 0 1
			stepBack
		stepBack
LightSource position 2 6 -2 luminance 150 ambientPower 0.2 color 1 1 1
```

Trusted real example: CustomScenes/Jack_O_Lantern.scn (serializer output). It
has two quirks the generator must NOT copy: `color` on line 1 and a trailing
root-level `stepBack`. It also puts `onTime`/`offTime`/`phase` on
`COMBSubtract`/`COMBIntersect`, where they are ignored.

## 11. Validator rules (Python)

Lexical
1. Line 1 is exactly `SDFObject`.
2. No blank or whitespace-only lines; no trailing whitespace; no commas; no tabs
   except leading indentation.
3. Every other line's first token is a keyword, `//` (comment) or `end`. Flag
   near-miss keywords (edit distance <= 2 or case-insensitive match).
4. `end`, if present, is the last line.

Parameters
5. Parameter names valid for the keyword; exact value count; no duplicates.
6. Numeric values; int fields are integers; bools are 0/1.
7. `axis` (int) 0-2, `operation` 0-2, `colorMode` 0-3; `repetitions`,
   `numPoints` >= 1; `SDFPlane normal` and `OPRotate axis` non-zero.
8. Plain `COMBSubtract`/`COMBIntersect` take no params; `COMBExclude` only `colorMode`
   (warning: the loader ignores them; the serializer writes `onTime`/`offTime`/`phase` on them).
9. Generator-only (warning): `OPCubeRollHeight`/`OPCubeRoller` are not allowed;
   every parameter is written explicitly.

Structure (track depth and the "previous meaningful line")
10. Every `SDF*` line on lines 2+ is preceded by a `COMB*`, with only `AN*` or
    comments in between. Every `COMB*` is followed by exactly one shape.
11. `stepBack` never at root depth (warning: the loader ignores it). At the end of the stack lines, depth equals
    the root (warning if not; trailing levels are closed implicitly).
12. Subtract/Intersect/Exclude (incl. smooth) need an earlier shape in the same level.
13. `LightSource` lines all come after the last stack line; at least one exists.

Animations
14. `AN*` follows a shape, `OP*`, `COMB*` or `AN*` line (not line 1, `stepBack`
    or `LightSource`). Resolve its target with the attachment rule.
15. `propName` present and in the target's Animatable list; animation dimension
    equals the property's dimension. Plain `COMB*`, `OPMirror`,
    `OPRepeatCircle`, `OPInsideOut` accept none.
16. No two animations on the same (target, propName).

## Appendix A: loader behavior and known bugs

Describes the loader after the 2026-11-04 fixes (branch SceneFileValidator,
`SDFBuilder.cpp/.h`, `SDFObject::popOperation`). The loader now prints
`Warning (line N): ...` to stdout and recovers instead of crashing or
silently misplacing things. It still never rejects a file. The generator and
validator rules in this spec are unchanged: a file that triggers any
warning is invalid.

**Structure**
- `loadSerialized` starts at index 1, so line 1 is never parsed.
- `loadSerialized` resets the stack and builder state before pushing its root.
  `stepBack` never pops the last level, so a `stepBack` at root depth is
  ignored (no warning).
- **Combination without a shape:** dropped with a warning. Before running any
  line that is not `AN*` or `SDF*`, and again at end of file, a combination
  still waiting for its shape is removed from the current level. This covers
  `COMBAdd COMBAdd`, `COMB*` followed by `OP*`/`stepBack`/`LightSource`, and a
  `COMB*` at the end of the stack. (The renderer still dereferences a null
  combination object, but the loader no longer produces one.)
- **Shape without a combination:** warned and skipped together with its whole
  subtree. Every line is ignored until that shape's own closing `stepBack`
  (nested shapes and their `stepBack`s are counted). Later lines keep their
  correct depth. A skipped shape with no closing `stepBack` skips the rest of
  the stack lines, including the lights.
- A combination takes exactly one shape (`m_lastOp` is cleared after a shape).

**Animations**
- An animation after `stepBack`, after `LightSource`, or directly after line 1
  has no target: warned and dropped. (Previously it bound to the parent shape
  or to whatever came before.) The other attachment rules in Section 8 are unchanged.
- An unknown or missing `propName` is still dropped silently.
- Still unchecked: `reinterpret_cast` binding, so a dimension mismatch corrupts
  memory (Section 8), and `OPRepeatCircle` `repetitions` is a `size_t` bound as
  double. Not fixed; the validator must catch these.

**Lines and parameters**
- An unknown first token is warned (`unknown keyword 'COMBadd'`) and the line
  is ignored. Blank lines, `//` comments and `end` are ignored silently.
- Unknown parameter names are still skipped silently (their values are read
  as names and skipped too). Extra values are read as names. `axis 1.0` reads 1.
- A malformed value (non-numeric, comma, too few values) sets failbit, which
  ends the parameter loop. If unparsed text remains, it is warned
  (`could not parse value; rest of line ignored: ',0,0 radius 2'`). Values
  read earlier on the line are kept. In a vec2/vec3, the failing component
  becomes 0 and later components keep their defaults.
- **Not detected:** a bad or missing value at the very end of a line
  (`overwriteColor 2`, `radius` with no value). failbit is also set at every
  normal end of line (`peek()` after eofbit), so these cases look identical
  from outside the handler. A bool other than 0/1 then loads as MSVC leaves it
  (`2` loads as 1).
- Trailing whitespace and CRLF are harmless.
- Commas are not replaced. `1,0,0` reads 1, then fails at the comma (warned).
  This is intentional: commas are not part of the format.

**Value checks (warn and use the default)**
- `axis` (int), `operation`, `colorMode` outside 0-2 / 0-2 / 0-3.
- `repetitions`, `numPoints` < 1 (read as signed, so negatives no longer wrap).
- Zero-length `SDFPlane normal` or `OPRotate axis` (falls back to 0 1 0).
  `SDFPlane::DFLT_NORMAL` is now a literal; it used to copy `Vec3::Up` from
  another translation unit and could be zero, depending on static
  initialization order.
- Not checked: zero vectors in `OPCubeRoller axes` (excluded from the generator).

## Appendix B: changes from v0.3

- Added a defaults column and per-keyword animatable properties with dimensions.
- Line 1 is skipped (root params ignored); never emit a root-level `stepBack`
  (the loader ignores it, but the generator should not write one).
- Strict lexical subset: no blank lines, no trailing whitespace (style rules;
  the loader ignores both).
- Commas are no longer separators (intentional loader change; v0.3 said commas become spaces).
- Every shape gets a closing `stepBack` in the examples (balanced files).
- Plain combinations take no animations; Subtract/Intersect take no params.
- Animation attachment rule corrected (after `stepBack`/`LightSource` = no
  valid target; now dropped with a warning).
- Dimension-mismatch and `repetitions` animation hazards (1D on a vec3 fails
  GLSL compilation).
- `SDFTaperedCylinder` uses `sideLength`.
- `OPRepeat` default makes 27 copies; `OPRotate` default is half a turn.
- `OPCubeRollHeight`, `OPCubeRoller` listed as excluded.
- The complete example scene has been rendered and confirmed.
- Lighting: the 1.5 x d^2 rule is now a guideline that artistic intent
  overrides. Jack_O_Lantern is low light by design.
- HOWTO corrections (colorMode 3 typo, inverse-square sentence) are now in
  HOWTO.txt and are removed from the open list.
- Describes one loader (commit d0fb8f1). The earlier draft compared the old
  committed loader with uncommitted edits; those comparisons are removed.
- Loader fixes (2026-11-04, Appendix A): warnings with line numbers;
  combinations with no shape are dropped instead of crashing; a shape with no
  combination is skipped with its whole subtree, so later depths stay correct;
  animations with no target are dropped; out-of-range enums, counts < 1 and zero
  direction vectors fall back to defaults. `SDFPlane::DFLT_NORMAL` no longer
  depends on static initialization order. The spec's rules are unchanged.

## Still open

- None at this time.
