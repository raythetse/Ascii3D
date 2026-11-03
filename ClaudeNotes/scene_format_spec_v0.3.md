# .scn scene format: spec for the scene-generator prompt (v0.3)

Condensed from the 34,351-byte HOWTO.txt (saved 2026-11-02 13:14).
Items marked **[CONFIRM]** are still unresolved.

## 1. Basics

- Plain text, one entry per line. Indentation is optional (readability only).
- Keywords are CASE SENSITIVE.
- The first line must be `SDFObject` (the root).
- Any line that does not start with a valid keyword is silently IGNORED
  (this is how comments work; `//` is the convention). Consequence: a
  misspelled keyword is dropped with no error. The generator must emit no
  prose and only keywords from this document.
- A line is a keyword followed by optional `paramName value...` pairs.
  Pairs may be in any order and each needs its full count of values
  (vec3 = 3 numbers, vec2 = 2). Integers and floats are interchangeable.
- Every parameter has a default (except `propName`), but defaults are not
  listed and are not always zero (a bare `OPTranslate` moves up). The
  generator should state every parameter it cares about explicitly.
  **[CONFIRM]** add a defaults column to the reference.
- Units are meters.
- Camera starts at (0, 1.8, -5) looking along +z (roughly human eye height).
  The user can move it while the scene runs, so scenes must look right from
  the start position. Max draw distance is 100 m from the camera.
- Axes (confirmed by the tutorial text): +x right, +y up, +z away from the
  camera (left-handed). Horizontal field of view is about 90 degrees in a
  default terminal. Visible area at z = 5 is therefore roughly x in +/-10.
- Ground: `SDFPlane normal 0 1 0 offset 0` is the natural floor for the start
  camera position. Keep content at about z = 0 to 15, y = 0 to 4.
- Colors: `color r g b`, each 0.0-1.0. Time is in seconds. Rotations are in
  full turns (1.0 = 360 degrees).
- At least one `LightSource` is needed for correct rendering (section 7).
  Light lines are not part of the stack; put them at the END of the file.

## 2. Entry types

| Prefix | Meaning |
|---|---|
| `SDF*` | A shape, or `SDFObject`, an empty shape used as a grouping level |
| `COMB*` | Combination: how the NEXT shape merges with what exists |
| `OP*` | Operation that transforms or modifies |
| `AN*` | Animation bound to the immediately preceding OP, COMB or SDF line |
| `stepBack` | Pop the top level off the stack |
| `LightSource` | A light (not on the stack) |

## 3. The stack model (the part that matters most)

1. Every shape line (including `SDFObject`) opens a new level. Operations
   belong to the level they are written under. `stepBack` pops the top level
   together with the operations declared in it.
2. Every shape except the root is preceded by a combination line, normally
   `COMBAdd`. A combination must be followed by a shape (animations bound to
   the combination may sit between them). Adding an operation before that
   shape exists can crash the loader.
3. Operations (Translate, Rotate, Elongate, Repeat, RepeatCircle, Mirror) are
   written BEFORE the shape they act on and are inherited by every shape
   added after them within the same level, including nested ones.
4. `stepBack` past the root is ignored, and trailing `stepBack`s at the end of
   the file are optional. An operation written at the root level can never be
   popped, so it affects everything.
5. **Safe pattern for a part that must be positioned independently**: wrap it
   in an empty `SDFObject`, then use two `stepBack`s (one closes the shape,
   one closes the wrapper):

```
COMBAdd
SDFObject
	OPTranslate translation 0 1 3
	COMBAdd
	SDFSphere radius 1 color 1 0 0
		stepBack
	stepBack
```

6. Resurface operations (`OPRoundOff`, `OPOnion`, `OPInsideOut`) are the
   exception: they go AFTER the shape and act on objects already on the stack.
   Wrap the shape so the effect stays local. An onion's hollow interior only
   shows if a later `COMBSubtract` cuts into it:

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
```

7. Nesting without a wrapper is allowed (operations after a shape apply to
   shapes added after it), e.g. a sphere attached to a box:

```
COMBAdd
SDFBox dimensions 2 1 0.5 color 0 0 1
	OPTranslate translation 0 1 0
	COMBAdd
	SDFSphere radius 0.75 color 1 0 0
```

8. Animations attach to the line immediately before them. Two animations on
   the same property: the later one wins, so never do that. An animation with
   an invalid `propName` is silently dropped.

## 4. Combinations

- `COMBAdd`: `onTime`, `offTime`, `phase`, `overwriteColor` (0/1)
- `COMBSubtract`, `COMBIntersect`: no params
- `COMBExclude`: `colorMode` 0-3 (0 shapes keep their colors; 1 shapes gain the
  original's color; 2 inner surfaces get the added color, outer keep original;
  3 like 2 with colors swapped by proximity). The HOWTO text for 3 says "same
  as three", clearly a typo for "two".
- Smooth variants share `smoothing`, `colorBlending`, `colorShifting`:
  `COMBAddSmooth`, `COMBSubtractSmooth`, `COMBIntersectSmooth`, `COMBExcludeSmooth`
- Subtract, Intersect and Exclude need something already present to act on.

## 5. Shapes (all accept `color r g b`)

- `SDFObject`: empty grouping level
- `SDFPlane`: `normal` (vec3), `offset`
- `SDFBox`: `dimensions` (vec3)
- `SDFBoxFrame`: `dimensions` (vec3), `edgeThickness`
- `SDFSphere`: `radius`
- `SDFEllipsoid`: `radii` (vec3)
- `SDFCrystal`: `edgeLength`
- `SDFCone`: `height`, `sweep` (0-1)
- `SDFSolidAngle`: `radius`, `sweep` (0-1)

Axis shapes take `axis` (0=x, 1=y, 2=z; default 1):
- `SDFTorus`: `radii` (vec2: major, minor)
- `SDFTorusSegment`: `radii` (vec2), `sweep` (0-1)
- `SDFCapsule`: `radius`, `bodyLength`
- `SDFCylinder`: `sideLength`, `radius`
- `SDFTaperedCapsule`: `bodyLength`, `radii` (vec2)
- `SDFTaperedCylinder`: `bodyLength`, `radii` (vec2)

2D shapes (extruded or revolved into 3D). Common params: `operation`
(0 extrude, 1 revolve, 2 infinite column; default 0), `magnitude` (extrude
height, or revolve ring radius), `revolveRotations`, `revolveRotateOffset` (vec2):
- `SDFRhombus`: `dimensions` (vec2)
- `SDFTriangle`: `point1`, `point2`, `point3` (each vec2)
- `SDFEquilateralTriangle`: `base`
- `SDFIsoscelesTriangle`: `dimensions` (vec2)
- `SDFPieSlice`: `sweep` (0-1), `radius`
- `SDFPentagon`, `SDFHexagon`, `SDFOctagon`: `radius`
- `SDFStar`: `radius`, `numPoints`, `ratio`

## 6. Operations

- `OPTranslate`: `translation` (vec3)
- `OPRotate`: `axis` (vec3, auto-normalized), `rotations`
- `OPElongate`: `axis` (0/1/2), `length`, `offset`
- `OPRepeat`: `spacing` (vec3), `limits` (vec3; N repeats in BOTH directions per
  axis, so limits `2 0 1` gives 5x1x3 = 15 copies). Repeated shapes should be
  centred and symmetrical.
- `OPRepeatCircle`: `axis` (0/1/2), `repetitions`. Follow it with an
  `OPTranslate` purely along x to set the circle's radius. Repeated objects are
  first re-oriented to the target axis.
- `OPMirror`: `mirrorX`, `mirrorY`, `mirrorZ` (0/1; default x only)
- Resurface (placed AFTER the shape): `OPRoundOff` (`magnitude`; poor with
  subtractions), `OPOnion` (`magnitude` = shell thickness; stackable),
  `OPInsideOut` (no params)

## 7. Lights

`LightSource position x y z luminance L ambientPower a color r g b`
- Brightness falls off as luminance / distance squared.
- HOWTO guidance: luminance 150 gives decent light at 10 m.
  **Correction needed in the HOWTO:** it says "double the power as you double
  the distance", but inverse-square means FOUR times the power. Rule for the
  generator: luminance = 1.5 x (distance in meters) squared.
- `ambientPower` 0.0-1.0: about 0.1-0.2 for a nearby light, 0.4-0.5 for a
  large distant light such as an outdoor scene.
- Lights cannot be animated.
- Example: `LightSource position 2.0 5.0 -3.0 luminance 150.0 ambientPower 0.2 color 1.0 1.0 1.0`

## 8. Animations

Use the 1D/2D/3D variant matching the property's dimension (one value per
dimension). `propName` is the property being animated (same name as the
parameter). Animation lines directly follow the line they animate.

- `ANContinuous{1,2,3}D`: `propName`, `amplitude` (per second), `phase`
- `ANSmoothCycle{1,2,3}D`: `propName`, `period`, `amplitude`, `phase` (sine)
- `ANLinearCycle{1,2,3}D`: `propName`, `forwardTime`, `delayTime1`,
  `backwardTime`, `delayTime2`, `amplitude`, `phase` (a time of 0 snaps)

## 9. Worked examples

Animated torus:
```
SDFObject
	OPRotate axis 0.5 1 0.875 rotations 0.125
	ANContinuous1D propName rotations amplitude 0.1
	COMBAdd
	SDFTorus radii 1.5 0.25 color 0 0.7 1
LightSource position 2 5 -3 luminance 150 ambientPower 0.2 color 1 1 1
```

Ring of spheres:
```
SDFObject
	OPRepeatCircle axis 2 repetitions 6
	OPTranslate translation 2 0 0
	COMBAdd
	SDFSphere radius 0.4 color 1 0.5 0
LightSource position 2 5 -3 luminance 150 ambientPower 0.2 color 1 1 1
```

Complete scene (ground, red sphere, blue box). UNTESTED: built from the rules
above, so render it before trusting it as a prompt example:
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

## 10. Still open

- Per-parameter defaults are not documented.
- Behavior on a known keyword with malformed parameters (wrong value count,
  non-numeric value) is unknown. This is the likely crash case, and the
  validator must catch it before the loader sees the file.
- `COMBExclude` `colorMode` 3 description typo ("same as three").
- HOWTO attenuation sentence (double distance needs 4x power, not 2x).
- The complete example scene in section 9 has not been rendered yet.
