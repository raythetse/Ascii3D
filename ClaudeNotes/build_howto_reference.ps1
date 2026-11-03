# Regenerates Part 2 (the reference) of HOWTO.txt in place. Part 1 (the tutorial) is
# kept exactly as it is in the file; everything from the "PART 2 - REFERENCE" banner on
# is replaced. Edit the Entry/Table data below, then run:
#     powershell -ExecutionPolicy Bypass -File ClaudeNotes\build_howto_reference.ps1
# Defaults come from the DFLT_* constants and the initial values in SDFBuilder::s_opMap;
# update the rows here when those change.
param([string]$HowTo = (Join-Path $PSScriptRoot '..\HOWTO.txt'))

$ErrorActionPreference = 'Stop'
$Width = 80
$Lines = New-Object System.Collections.Generic.List[string]

# Table columns: indent 4, name 21, type 7, default 12, description from col 44.
$ColName = 4; $ColType = 25; $ColDflt = 32; $ColDesc = 44

function Out-Line([string]$s = '') { $script:Lines.Add($s.TrimEnd()) }

function Wrap-Text([string]$text, [string]$first, [string]$rest) {
    $words = $text -split '\s+' | Where-Object { $_ -ne '' }
    $line = $first; $empty = $true
    foreach ($w in $words) {
        if (-not $empty -and ($line.Length + 1 + $w.Length) -gt $Width) {
            Out-Line $line; $line = $rest + $w
        } elseif ($empty) { $line += $w } else { $line += ' ' + $w }
        $empty = $false
    }
    if (-not $empty) { Out-Line $line }
}

function Para([string]$text, [int]$indent = 0) {
    $pad = ' ' * $indent
    Wrap-Text $text $pad $pad
}

function Trim-Blanks {
    while ($script:Lines.Count -gt 0 -and $script:Lines[$script:Lines.Count - 1] -eq '') {
        $script:Lines.RemoveAt($script:Lines.Count - 1)
    }
}

function H1([string]$title) {
    Trim-Blanks; Out-Line; Out-Line
    Out-Line ('=' * $Width); Out-Line $title; Out-Line ('=' * $Width)
}

function H2([string]$title) {
    Trim-Blanks; Out-Line; Out-Line
    Out-Line $title; Out-Line ('-' * $Width); Out-Line
}

function Pad([string]$s, [int]$col) {
    if ($s.Length -ge $col) { return $s + ' ' } else { return $s.PadRight($col) }
}

function Table([string[]]$rows) {
    $head = Pad (' ' * $ColName + 'Parameter') $ColType
    $head = Pad ($head + 'Type') $ColDflt
    $head = (Pad ($head + 'Default') $ColDesc) + 'Description'
    Out-Line $head
    foreach ($r in $rows) {
        $f = $r -split '\|'
        $prefix = Pad (' ' * $ColName + $f[0]) $ColType
        $prefix = Pad ($prefix + $f[1]) $ColDflt
        $prefix = Pad ($prefix + $f[2]) $ColDesc
        Wrap-Text $f[3] $prefix (' ' * $ColDesc)
    }
}

function Entry {
    param([string]$Name, [string[]]$Desc = @(), [string[]]$Eg = @(), [string[]]$Params = @(),
          [string]$NoParams = '')
    Out-Line $Name
    $firstPara = $true
    foreach ($d in $Desc) {
        if (-not $firstPara) { Out-Line }
        Para $d 4; $firstPara = $false
    }
    if ($Eg.Count -gt 0) {
        Out-Line
        Out-Line ('    eg. ' + $Eg[0])
        for ($i = 1; $i -lt $Eg.Count; $i++) { Out-Line ('        ' + $Eg[$i]) }
    }
    if ($Params.Count -gt 0) { Out-Line; Table $Params }
    elseif ($NoParams -ne '') { Out-Line; Para $NoParams 4 }
    Out-Line
}

# ---------------------------------------------------------------------------
H1 'PART 2 - REFERENCE'

H2 '2.1  CONVENTIONS USED IN THE REFERENCE'
Para 'Each entry gives the keyword, a short description, an example line, and a table of its parameters with their types and default values. A parameter you leave out of a line takes its default value.'
Out-Line
Out-Line '    Type    Meaning'
Out-Line '    float   a single number (integers are accepted, eg. 2 or 2.0)'
Out-Line '    int     a whole number'
Out-Line '    bool    0 = false, 1 = true'
Out-Line '    vec2    two numbers, eg. 1.0 0.5'
Out-Line '    vec3    three numbers, eg. 1.0 0.5 0.25'
Out-Line '    name    the name of another parameter (used by propName)'
Out-Line '    varies  float, vec2 or vec3 to match the 1D, 2D or 3D animation variant'
Out-Line
Para 'Axis values are 0 = x, 1 = y, 2 = z. Colors are r g b with each channel ranging 0.0-1.0. Times are in seconds.'

# ---------------------------------------------------------------------------
H2 '2.2  COMBINATIONS'

Entry 'COMBAdd' -Desc @('Adds an object to the scene stack. Note that the properties of COMBAdd cannot be bound to an animation since they handle their own animations. Attempts to bind an animator to them will be ignored.') `
    -Eg @('COMBAdd onTime 1.0 offTime 0.5 phase 0.5 overwriteColor 0') `
    -Params @(
        'onTime|float|1.0|How long to show the object',
        'offTime|float|0.0|How long to hide the object (0 = always shown)',
        'phase|float|0.0|How far into the off/on cycle to start',
        "overwriteColor|bool|0|Whether or not to overwrite the existing object's color with that of the added object")

Entry 'COMBSubtract' -Desc @("Subtracts an object's shape from objects on the scene stack.") `
    -Eg @('COMBSubtract') -NoParams 'No parameters.'

Entry 'COMBIntersect' -Desc @('Subtracts non-intersecting volumes from the scene stack.') `
    -Eg @('COMBIntersect') -NoParams 'No parameters.'

Entry 'COMBExclude' -Desc @('Subtracts intersecting volumes from the scene stack. Combine with a subtraction to see the interior effect.') `
    -Eg @('COMBExclude colorMode 3') `
    -Params @('colorMode|int|0|How colors are assigned (see below)')
$Lines.RemoveAt($Lines.Count - 1)
Out-Line
Out-Line '    colorMode values:'
Out-Line '        0 = shapes retain their color'
Out-Line '        1 = shapes gain color of original'
Out-Line '        2 = internal surfaces get added color, outer surfaces retain original'
Out-Line '        3 = same as 2 but colors are swapped according to proximity'
Out-Line

Para 'The following four smooth combinations share these parameters:'
Out-Line
Table @(
    'smoothing|float|0.5|Distance to smooth over',
    'colorBlending|float|1.0|Distance to blend colors over smoothed sections',
    'colorShifting|float|0.0|Shifts the color smoothing closer to one object or the other')
Out-Line

Entry 'COMBAddSmooth' -Desc @('Add with a smoothed intersection.') `
    -Eg @('COMBAddSmooth smoothing 0.25 colorBlending 1.0 colorShifting 0.0') `
    -NoParams 'Parameters: the shared smooth parameters above.'
Entry 'COMBSubtractSmooth' -Desc @('Subtract with smoothed edges.') `
    -Eg @('COMBSubtractSmooth smoothing 0.25 colorBlending 1.0 colorShifting 0.0') `
    -NoParams 'Parameters: the shared smooth parameters above.'
Entry 'COMBIntersectSmooth' -Desc @('Intersect with smoothed edges.') `
    -Eg @('COMBIntersectSmooth smoothing 0.25 colorBlending 1.0 colorShifting 0.0') `
    -NoParams 'Parameters: the shared smooth parameters above.'
Entry 'COMBExcludeSmooth' -Desc @('Exclude with smoothed edges.') `
    -Eg @('COMBExcludeSmooth smoothing 0.25 colorBlending 1.0 colorShifting 0.0') `
    -NoParams 'Parameters: the shared smooth parameters above.'

# ---------------------------------------------------------------------------
H2 '2.3  SDF OBJECTS'

Para 'All SDF objects share this parameter:'
Out-Line
Table @('color|vec3|1 1 1|RGB color, each channel ranging 0.0-1.0')
Out-Line

Entry 'SDFObject' -Desc @('An empty object. Use it to add a layer to the stack without adding or subtracting shapes.') `
    -Eg @('SDFObject color 1 0.5 0.25') -NoParams 'Parameters: color only.'

Entry 'SDFPlane' -Desc @('A ground plane with a normal vector and an offset. A plane with normal 0 1 0 and offset 0 should be a natural ground plane for the initial camera position.') `
    -Eg @('SDFPlane color 1 0 0 normal 0 1 0 offset -1.5') `
    -Params @(
        'normal|vec3|0 1 0|The up vector of the plane (automatically normalized)',
        'offset|float|0.0|Offsets the plane along the normal vector from the origin point')

Entry 'SDFBox' -Desc @('A box with 6 sides and 90 degree angles.') `
    -Eg @('SDFBox color 1 0 0 dimensions 2.5 1.0 2.5') `
    -Params @('dimensions|vec3|1 1 1|Edge length of each dimension')

Entry 'SDFBoxFrame' -Desc @('A box with only its edges rendered, at a given thickness.') `
    -Eg @('SDFBoxFrame color 1 0 0 edgeThickness 0.1 dimensions 2 2 1.5') `
    -Params @(
        'dimensions|vec3|1 1 1|Edge length of each dimension',
        'edgeThickness|float|0.1|Thickness of the rendered edges')

Entry 'SDFSphere' -Desc @('A sphere defined by a radius.') `
    -Eg @('SDFSphere color 0 0.5 1 radius 1.5') `
    -Params @('radius|float|0.5|The radius of the sphere')

Entry 'SDFEllipsoid' -Desc @('An ellipsoid with 3 independent radii.') `
    -Eg @('SDFEllipsoid color 1 1 1 radii 1.0 0.5 0.25') `
    -Params @('radii|vec3|1 0.5 0.25|Individual radii for each axis')

Entry 'SDFCrystal' -Desc @('A regular octahedron.') `
    -Eg @('SDFCrystal color 0 0 0.5 edgeLength 1.3') `
    -Params @('edgeLength|float|1.0|The length of every edge of the crystal')

Entry 'SDFCone' -Desc @('A standard cone pointing up.') `
    -Eg @('SDFCone color 1 1 1 height 1.0 sweep 0.5') `
    -Params @(
        'height|float|1.0|The height of the cone',
        "sweep|float|0.5|The angle of the cone's sides (0.0-1.0). 0 produces a line")

Entry 'SDFSolidAngle' -Desc @('A section of a sphere defined by an angle radiating from the center.') `
    -Eg @('SDFSolidAngle color 0.4 0.2 1.0 radius 0.55 sweep 0.25') `
    -Params @(
        'radius|float|0.5|The radius of the sphere',
        'sweep|float|0.25|The angle (0.0-1.0)')

Para 'The following objects (SDFTorus through SDFTaperedCylinder) have an axis parameter which aligns the object along the selected axis (0=x, 1=y, 2=z). 1 (y) is the default axis.'
Out-Line

Entry 'SDFTorus' -Desc @('A donut shape defined by 2 radii, major and minor.') `
    -Eg @('SDFTorus color 0 0 1 radii 2.5 0.5 axis 0') `
    -Params @(
        'radii|vec2|1 0.25|The major and minor radii',
        'axis|int|1|The axis that passes through the hole')

Entry 'SDFTorusSegment' -Desc @('A donut shape defined by 2 radii, major and minor, limited to a partial angle.') `
    -Eg @('SDFTorusSegment color 0 0 1 radii 2.5 0.5 axis 0 sweep 0.5') `
    -Params @(
        'radii|vec2|1 0.25|The major and minor radii',
        'sweep|float|0.25|The portion of the ring that is rendered (0.0-1.0)',
        'axis|int|1|The axis that passes through the hole')

Entry 'SDFCapsule' -Desc @('A classic pill shape.') `
    -Eg @('SDFCapsule color 1 0 1 radius 0.5 bodyLength 2.0 axis 2') `
    -Params @(
        'radius|float|0.5|The radius of the capsule',
        'bodyLength|float|1.0|The length of the capsule, not including the rounding (the rounding adds the radius at both ends)',
        'axis|int|1|The axis that the body aligns with')

Entry 'SDFCylinder' -Desc @('A classic cylinder shape.') `
    -Eg @('SDFCylinder color 0 1 0 sideLength 1.0 radius 0.25 axis 1') `
    -Params @(
        'sideLength|float|1.0|The length of the cylinder',
        'radius|float|0.5|The radius of the cylinder',
        'axis|int|1|The axis that the body aligns with')

Entry 'SDFTaperedCapsule' -Desc @('A pill shape whose radius decreases/increases along its length.') `
    -Eg @('SDFTaperedCapsule color 1 1 1 bodyLength 2.5 radii 1.0 0.5 axis 2') `
    -Params @(
        'bodyLength|float|1.0|The length of the capsule, not including the rounded ends',
        'radii|vec2|0.5 0.25|Individual radii for each end of the capsule',
        'axis|int|1|The axis that the body aligns with')

Entry 'SDFTaperedCylinder' -Desc @('A cylinder shape whose radius decreases/increases along its length.') `
    -Eg @('SDFTaperedCylinder color 0.5 0.5 0.5 sideLength 1.5 radii 0.2 0.5 axis 0') `
    -Params @(
        'sideLength|float|1.0|The length of the cylinder',
        'radii|vec2|0.5 0.25|Individual radii for each end of the cylinder',
        'axis|int|1|The axis that the body aligns with')

Para 'The following SDF objects are 2D shapes that can be made 3D with 3 different operations: 0 = extrusion, 1 = revolve, 2 = infinite column. Extrusion is the default operation. In the case of extrusion, the magnitude parameter defines the height of the extrusion. In the case of revolve, magnitude is the radius of the ring around which the shape is revolved, and the two revolve parameters can also be used. They all share these parameters:'
Out-Line
Table @(
    'operation|int|0|0 = extrusion, 1 = revolve, 2 = infinite column',
    'magnitude|float|1.0|Extrusion: the height of the extrusion. Revolve: the radius of the ring around which the shape is revolved',
    'revolveRotations|float|0.0|The number of times to rotate the shape before it is revolved',
    'revolveRotateOffset|vec2|0 0|The offset of the center of rotation for the revolveRotations parameter')
Out-Line

Entry 'SDFRhombus' -Desc @('A 2D rhombus that can be extruded or revolved into a ring.') `
    -Eg @('SDFRhombus color 1 1 0.5 dimensions 1.0 1.5 operation 0 magnitude 1.0') `
    -Params @('dimensions|vec2|1 0.5|Width and length of the rhombus')

Entry 'SDFTriangle' -Desc @('A triangle with arbitrary 2D vertices defined by point1, point2, and point3.') `
    -Eg @('SDFTriangle color 0 1 1 point1 -1 0 point2 0 1 point3 0.5 -1.5 operation 0 magnitude 0.75') `
    -Params @(
        'point1|vec2|-0.5 -0.5|The first vertex',
        'point2|vec2|0 0.5|The second vertex',
        'point3|vec2|0.5 0|The third vertex')

Entry 'SDFEquilateralTriangle' -Desc @('A triangle with sides of equal length.') `
    -Eg @('SDFEquilateralTriangle color 0.5 0.5 0.0 base 2.0 operation 1 magnitude 2.5 revolveRotations 0.0 revolveRotateOffset 0.1 0.25') `
    -Params @('base|float|1.0|The length of the sides')

Entry 'SDFIsoscelesTriangle' -Desc @('A triangle with two equal sides; the third may be different.') `
    -Eg @('SDFIsoscelesTriangle color 0 0 1 dimensions 1 2 magnitude 2.0 operation 1 revolveRotations 0.234 revolveRotateOffset 0.2 0.0') `
    -Params @('dimensions|vec2|1 0.5|The width and height of the triangle')

Entry 'SDFPieSlice' -Desc @('A segment of a circle, like a piece of pie.') `
    -Eg @('SDFPieSlice color 1 1 1 sweep 0.2 radius 1.0 operation 0 magnitude 0.1') `
    -Params @(
        'sweep|float|0.25|The angle of the circle to render (0.0-1.0)',
        'radius|float|0.5|The radius of the circle')

Entry 'SDFPentagon' -Desc @('A regular pentagon.') `
    -Eg @('SDFPentagon color 1 0 1 radius 2.74 operation 0 magnitude 3.0') `
    -Params @('radius|float|0.5|The radius of the circle that touches all the points of the pentagon')

Entry 'SDFHexagon' -Desc @('A regular hexagon.') `
    -Eg @('SDFHexagon color 0 0 0.25 radius 1.75 operation 0 magnitude 1.0') `
    -Params @('radius|float|0.5|The radius of the circle that touches all the points of the hexagon')

Entry 'SDFOctagon' -Desc @('A regular octagon.') `
    -Eg @('SDFOctagon color 0 0 0.25 radius 1.75 operation 0 magnitude 1.0') `
    -Params @('radius|float|0.5|The radius of the circle that touches all the points of the octagon')

Entry 'SDFStar' -Desc @('A star with an arbitrary number of points.') `
    -Eg @('SDFStar color 1 1 0 radius 2.0 numPoints 5 ratio 0.5 operation 0 magnitude 0.5') `
    -Params @(
        'radius|float|0.5|The radius of the circle that touches all the points of the star',
        'numPoints|int|5|The number of points of the star',
        'ratio|float|0.5|The distance of the inner corners relative to the radius')

# ---------------------------------------------------------------------------
H2 '2.4  OPERATIONS'

Entry 'OPTranslate' -Desc @('Moves objects by a vector.') `
    -Eg @('OPTranslate translation 1 2.4 5.2') `
    -Params @('translation|vec3|0 1 0|The vector to move the object by')

Entry 'OPRotate' -Desc @('Rotates an object around a vector axis. The vector will be automatically normalized.') `
    -Eg @('OPRotate axis 1 0 0 rotations 0.75') `
    -Params @(
        'axis|vec3|0 1 0|The vector to rotate around',
        'rotations|float|0.5|The number of full rotations (1.0 = one full rotation)')

Entry 'OPElongate' -Desc @('Elongates objects along an axis.') `
    -Eg @('OPElongate axis 1 length 1.0 offset 0.25') `
    -Params @(
        'axis|int|1|The axis (0=x, 1=y, 2=z) to elongate on',
        'length|float|1.0|The length of the elongation along the axis',
        'offset|float|0.0|A distance along the axis that is the center of the elongation')

Entry 'OPRepeat' -Desc @('Repeats the object a certain number of times in each direction along the x/y/z axes. Note that non-symmetrical objects may result in artefacting.') `
    -Eg @('OPRepeat spacing 2 1 1 limits 1 1 0') `
    -Params @(
        'spacing|vec3|1 1 1|The size of each repeating cell',
        'limits|vec3|1 1 1|How many times to repeat the object in each direction (the default makes 3 x 3 x 3 = 27 copies)')

Entry 'OPRepeatCircle' -Desc @('Repeats the object in a circular pattern. Use OPTranslate following OPRepeatCircle to control the radius of the circle.') `
    -Eg @('OPRepeatCircle axis 2 repetitions 8') `
    -Params @(
        'axis|int|1|The axis (0=x, 1=y, 2=z) to revolve around',
        'repetitions|int|10|The number of times the object will be repeated')

Entry 'OPMirror' -Desc @('Mirrors the object over the selected axes. By default it mirrors over just the x axis.') `
    -Eg @('OPMirror mirrorX 1 mirrorY 0 mirrorZ 1') `
    -Params @(
        'mirrorX|bool|1|Whether or not to mirror over the x axis (yz plane)',
        'mirrorY|bool|0|Whether or not to mirror over the y axis (xz plane)',
        'mirrorZ|bool|0|Whether or not to mirror over the z axis (xy plane)')

Para 'The final three operations are resurface operations. Unlike other operations, which come before the objects they operate on are added to the stack, these operations operate on objects already on the stack.'
Out-Line

Entry 'OPRoundOff' -Desc @('Rounds-off objects by inflating them. Note that this does not work well with subtractions. Use a smooth subtraction if experiencing edges that do not round-off.') `
    -Eg @('OPRoundOff magnitude 0.05') `
    -Params @('magnitude|float|0.01|The amount to inflate the objects')

Entry 'OPOnion' -Desc @('Hollows out an object leaving a shell of defined thickness. Requires a subsequent subtraction combination to reveal the inside.') `
    -Eg @('OPOnion magnitude 0.1') `
    -Params @('magnitude|float|0.01|The thickness of the shell')

Entry 'OPInsideOut' -Desc @('Turns solid space to empty space and vice-versa.') `
    -Eg @('OPInsideOut') -NoParams 'No parameters.'

# ---------------------------------------------------------------------------
H2 '2.5  ANIMATIONS'

Para 'There are 3 main types of animations - Continuous, SmoothCycle, and LinearCycle. Each has a 1D, 2D, and 3D variant. Each must reference a valid property name of the object or operation it is animating. These property names are the same as the parameter names for each object and operation. Though in some instances a mismatch of dimensions might still run, some will not. It is important to use the animation variant that matches the dimensions of the property you are animating. Use the propName parameter to reference the property you wish to animate.'
Out-Line
Para 'All animations share this parameter:'
Out-Line
Table @('propName|name|(none)|Required. The property to animate')
Out-Line
Para 'In the tables below, each numeric parameter is a float, vec2 or vec3 to match the 1D, 2D or 3D variant, and its default applies to every component.'
Out-Line

Entry 'ANContinuous1D / ANContinuous2D / ANContinuous3D' `
    -Desc @('Continuously increases the property value by the amplitude.') `
    -Eg @('ANContinuous1D propName rotations amplitude 1.0 phase 0.5',
          'ANContinuous2D propName radii amplitude 0.45 0.22 phase 1.0 1.25',
          'ANContinuous3D propName translation amplitude 0.25 0.5 0.75 phase 0.0 1.0 2.0') `
    -Params @(
        'amplitude|varies|1.0|The amount to increase the property per second',
        'phase|varies|0.0|The amount of time into the animation to start')

Entry 'ANSmoothCycle1D / ANSmoothCycle2D / ANSmoothCycle3D' `
    -Desc @('Smoothly oscillates the property value with a sine function.') `
    -Eg @('ANSmoothCycle1D propName rotations period 4.0 amplitude 2.0 phase 0.25',
          'ANSmoothCycle2D propName radii period 2.0 4.0 amplitude 1.0 2.0 phase 1.75 0.25',
          'ANSmoothCycle3D propName translation period 2.0 4.0 5.5 amplitude 0.25 1.0 2.0 phase 1.75 0.25 1.25') `
    -Params @(
        'period|varies|1.0|The amount of time to complete one oscillation cycle',
        'amplitude|varies|1.0|The maximum distance (both positive and negative) from the starting value',
        'phase|varies|0.0|The amount of time into the animation to start')

Entry 'ANLinearCycle1D / ANLinearCycle2D / ANLinearCycle3D' `
    -Desc @('Animates the values with constant rates over distinct phases. The animation is split into a forward motion, a delay, a return motion, and a second delay. Each phase has its own duration. The cycle time is the sum of all four phases. A phase time of 0 means the object will instantly snap to the opposite end of the animation.') `
    -Eg @('ANLinearCycle1D propName rotations forwardTime 1.0 delayTime1 0.5 backwardTime 0.0 delayTime2 1.0 amplitude 2.0 phase 0.5',
          'ANLinearCycle2D propName radii forwardTime 1.0 2.0 delayTime1 0.5 0.25 backwardTime 0.0 0.15 delayTime2 1.0 2.0 amplitude 2.0 2.0 phase 0.5 1.5',
          'ANLinearCycle3D propName translation forwardTime 1.0 1.0 0.5 delayTime1 0.5 0.0 0.5 backwardTime 0.0 5.0 1.25 delayTime2 1.0 1.0 1.0 amplitude 2.0 2.1 2.2 phase 0.5 0.5 0.0') `
    -Params @(
        'forwardTime|varies|1.0|The time to animate the forward phase',
        'delayTime1|varies|1.0|The time to hold the value steady at the end of the forward phase',
        'backwardTime|varies|1.0|The time to animate the backward phase',
        'delayTime2|varies|1.0|The time to hold the value steady at the end of the backward phase',
        'amplitude|varies|1.0|The maximum distance (both positive and negative) from the starting value',
        'phase|varies|0.0|The amount of time into the animation to start')

# ---------------------------------------------------------------------------
H2 '2.6  LIGHT SOURCES'

Entry 'LightSource' -Desc @(
        'A light source to light the scene. These are not a part of the stack and can be defined at any point in the .scn file except for the first line, though it is best to define them all together at the end of the file. At this time, light sources cannot be animated.',
        "A light's power attenuates with distance [luminance / (distance * distance)]. A decent illumination may be 150 at a distance of 10 meters. If you double the distance, multiply the power by 4. Lower ambient power values (perhaps 0.1-0.2) work well for light sources close to the objects, while a higher value (perhaps 0.4-0.5) works well for larger, more distant lights such as for an outdoor scene.") `
    -Eg @('LightSource position 2.0 5.0 -3.0 luminance 150.0 ambientPower 0.2 color 1.0 1.0 1.0') `
    -Params @(
        'position|vec3|5 5 -3|The absolute position of the light source',
        'luminance|float|100.0|The power of the light directly at its source. Attenuates by 1.0 / (distance * distance)',
        'ambientPower|float|0.2|The amount of the light that still affects the surface even if it is shadowed or facing away (0.0-1.0)',
        'color|vec3|1 1 1|The color of the light')

# ---------------------------------------------------------------------------
H2 '2.7  stepBack'

Entry 'stepBack' -Desc @('Removes the top object from the stack and returns to the previous level. Note: it is not required to stepBack to the base level at the end of the file, though it is recommended for consistency and to facilitate any future edits to the .scn file. A stepBack that moves beyond the base layer will be ignored, but should be avoided for clarity.') `
    -Eg @('stepBack') -NoParams 'No parameters.'

# ---------------------------------------------------------------------------
# Drop trailing blank lines, then join with Part 1 using CRLF.
while ($Lines.Count -gt 0 -and $Lines[$Lines.Count - 1] -eq '') { $Lines.RemoveAt($Lines.Count - 1) }
$old = [IO.File]::ReadAllText($HowTo) -replace "`r`n", "`n"
$banner = "`n" + ('=' * $Width) + "`nPART 2 - REFERENCE`n"
$cut = $old.IndexOf($banner)
if ($cut -lt 0) { throw "Could not find the 'PART 2 - REFERENCE' banner in $HowTo" }
$part1 = $old.Substring(0, $cut).TrimEnd("`n")
$text = ($part1 + "`n" + ($Lines -join "`n") + "`n") -replace "`n", "`r`n"
[IO.File]::WriteAllText($HowTo, $text, (New-Object System.Text.UTF8Encoding($false)))
