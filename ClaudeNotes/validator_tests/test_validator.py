"""Tests for validate_scn.py.

Run from the repository root:
    python ClaudeNotes/validator_tests/test_validator.py

Each fixture must produce exactly the findings listed in EXPECTED, given as
(line, severity, rule). Message wording is not checked.
"""

import glob
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.dirname(HERE))

import validate_scn  # noqa: E402

E, W = "error", "warning"

EXPECTED = {
    "valid.scn": [],
    "rule01_line1_wrong.scn": [(1, E, 1)],
    "rule01_line1_params.scn": [(1, W, 1)],
    "rule02_lexical.scn": [
        (2, E, 2),             # trailing space
        (3, E, 2),             # blank line
        (4, E, 2),             # tab between tokens
        (7, E, 2),             # comma
        (7, E, 5), (7, E, 6),  # ...which also breaks the color value
    ],
    "rule03_keywords.scn": [(2, E, 3), (6, E, 3)],
    "rule04_end.scn": [(6, E, 4)],
    "rule05_params.scn": [
        (5, E, 5), (5, W, 9),  # misspelled "position", so position is also missing
        (6, E, 5),             # luminance repeated
        (7, E, 5),             # position has 2 values
    ],
    "rule06_values.scn": [(2, E, 6), (3, E, 6), (8, E, 6)],
    "rule07_ranges.scn": [(2, E, 7), (3, E, 7), (3, E, 7), (6, E, 7)],
    "rule08_comb_params.scn": [(5, W, 8), (8, W, 8)],
    "rule09_generator.scn": [(2, W, 9), (3, W, 9)],
    "rule10_combination.scn": [(2, E, 10), (4, E, 10)],
    "rule11_depth.scn": [(5, W, 11), (11, W, 11)],
    "rule12_needs_shape.scn": [(2, E, 12), (7, E, 12)],
    "rule13_light_order.scn": [(6, E, 13)],
    "rule13_no_light.scn": [(4, E, 13)],
    "rule14_anim_position.scn": [(2, E, 14), (6, E, 14)],
    "rule15_anim_property.scn": [
        (3, E, 15),   # plain COMBAdd cannot be animated
        (5, E, 15),   # 3D animation on 1D radius
        (6, E, 15),   # misspelled propName
        (7, E, 15),   # no propName
        (10, E, 15),  # repetitions animated
        (12, E, 15),  # 1D animation on vec3 translation
    ],
    "rule16_duplicate_anim.scn": [(5, E, 16)],
}

REAL_SCENES = sorted(glob.glob(os.path.join(REPO, "CustomScenes", "*.scn")))


def summary(findings):
    return sorted((f.line, f.severity, f.rule) for f in findings)


class FixtureTests(unittest.TestCase):
    def test_fixtures(self):
        for name, expected in EXPECTED.items():
            with self.subTest(fixture=name):
                findings = validate_scn.validate_file(os.path.join(HERE, name))
                detail = "\n".join(f.format(name) for f in findings)
                self.assertEqual(summary(findings), sorted(expected), "\n" + detail)

    def test_every_fixture_is_listed(self):
        on_disk = {os.path.basename(p) for p in glob.glob(os.path.join(HERE, "*.scn"))}
        self.assertEqual(on_disk, set(EXPECTED))

    def test_every_rule_has_a_fixture(self):
        covered = {rule for expected in EXPECTED.values() for _, _, rule in expected}
        self.assertEqual(covered, set(range(1, 17)))

    def test_crlf_gives_same_result(self):
        for name in EXPECTED:
            with self.subTest(fixture=name):
                with open(os.path.join(HERE, name), encoding="utf-8", newline="") as f:
                    text = f.read().replace("\r\n", "\n")
                lf = summary(validate_scn.validate_text(text))
                crlf = summary(validate_scn.validate_text(text.replace("\n", "\r\n")))
                self.assertEqual(lf, crlf)


class RealSceneTests(unittest.TestCase):
    def test_real_scenes_have_no_errors(self):
        self.assertTrue(REAL_SCENES, "no scenes found in CustomScenes/")
        for path in REAL_SCENES:
            with self.subTest(scene=os.path.basename(path)):
                errors = [f for f in validate_scn.validate_file(path) if f.severity == "error"]
                self.assertEqual(errors, [], "\n".join(f.format(path) for f in errors))


if __name__ == "__main__":
    unittest.main(verbosity=2)
