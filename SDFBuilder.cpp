#include "SDFBuilder.h"
#include "SDFObjectInc.h"
#include "SDFScene.h"

SDFBuilder::SDFBuilder() {
    m_baseObject = std::make_unique<SDFObject>();
    m_objChain.push_back(m_baseObject.get());
}

std::unique_ptr<SDFObject> SDFBuilder::getBaseObject() {
    return std::move(m_baseObject);
}

std::vector<std::unique_ptr<LightSource>> SDFBuilder::getLights()
{
    return std::move(m_lights);
}

SDFBuilder& SDFBuilder::addLightSource(Vec3 position, double luminance, Color color, double ambientPower)
{
    m_lights.push_back(std::make_unique<LightSource>(position, luminance, color, ambientPower));
    return *this;
}

SDFBuilder& SDFBuilder::stepBack(size_t steps)
{
    if (!steps) steps = m_objChain.size() - 1;
    for (size_t i{}; i < steps; ++i)
    {
        if (m_objChain.size() == 1) return *this;
        m_objChain.pop_back();
    }
    return *this;
}

SDFBuilder& SDFBuilder::color(Vec3 color)
{
    m_objChain.back()->setColor(color);
    return *this;
}

SDFBuilder& SDFBuilder::loadSerialized(std::vector<std::string>& strVec)
{
    m_baseObject = std::make_unique<SDFObject>();
    m_objChain.push_back(m_baseObject.get());

    for (size_t i{ 1 }; i < strVec.size(); ++i) {
        std::stringstream ss{ strVec[i] };
        std::string opName;
        ss >> opName;
        if (s_opMap.contains(opName)) {
            s_opMap.at(opName)(*this, ss);
        }
        //if (ss.fail()) {
        //    std::cout << "\nFailed to parse line " << i << '\n';
        //    std::exit(1);
        //}
    }
    return *this;
}

const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> SDFBuilder::s_opMap = {
    // Step Back
    { "stepBack", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.stepBack();
    }},

    // Combinations
    { "COMBAdd", [](SDFBuilder& builder, std::stringstream& ss) {
        double onTime{ 1.0 };
        double offTime{ 0.0 };
        double phase{ 0.0 };
        bool overwriteColor{ false };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "onTime") ss >> onTime;
            else if (param == "offTime") ss >> offTime;
            else if (param == "phase") ss >> phase;
            else if (param == "overwriteColor") ss >> overwriteColor;
        }
        builder.operate<COMBAdd>(nullptr, overwriteColor, onTime, offTime, phase);
    }},
    { "COMBSubtract", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.operate<COMBSubtract>();
    }},
    { "COMBIntersect", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.operate<COMBIntersect>();
    }},
    { "COMBExclude", [](SDFBuilder& builder, std::stringstream& ss) {
        int colorMode{ColorMode::COLORMODE_DOUBLE};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "colorMode") ss >> colorMode;
        }
        builder.operate<COMBExclude>(nullptr, static_cast<ColorMode>(colorMode));
    }},
    { "COMBAddSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double smoothing{ SmoothCombination::DFLT_SMOOTHING };
        double colorBlending{ SmoothCombination::DFLT_COLORBLENDING };
        double colorShifting{ SmoothCombination::DFLT_COLORSHIFTING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "smoothing") ss >> smoothing;
            else if (param == "colorBlending") ss >> colorBlending;
            else if (param == "colorShifting") ss >> colorShifting;
        }
        builder.operate<COMBAddSmooth>(nullptr, smoothing, colorBlending, colorShifting);
    }},
    { "COMBSubtractSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double smoothing{ SmoothCombination::DFLT_SMOOTHING };
        double colorBlending{ SmoothCombination::DFLT_COLORBLENDING };
        double colorShifting{ SmoothCombination::DFLT_COLORSHIFTING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "smoothing") ss >> smoothing;
            else if (param == "colorBlending") ss >> colorBlending;
            else if (param == "colorShifting") ss >> colorShifting;
        }
        builder.operate<COMBSubtractSmooth>(nullptr, smoothing, colorBlending, colorShifting);
    }},
    { "COMBIntersectSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double smoothing{ SmoothCombination::DFLT_SMOOTHING };
        double colorBlending{ SmoothCombination::DFLT_COLORBLENDING };
        double colorShifting{ SmoothCombination::DFLT_COLORSHIFTING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "smoothing") ss >> smoothing;
            else if (param == "colorBlending") ss >> colorBlending;
            else if (param == "colorShifting") ss >> colorShifting;
        }
        builder.operate<COMBIntersectSmooth>(nullptr, smoothing, colorBlending, colorShifting);
    }},
    { "COMBExcludeSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double smoothing{ SmoothCombination::DFLT_SMOOTHING };
        double colorBlending{ SmoothCombination::DFLT_COLORBLENDING };
        double colorShifting{ SmoothCombination::DFLT_COLORSHIFTING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "smoothing") ss >> smoothing;
            else if (param == "colorBlending") ss >> colorBlending;
            else if (param == "colorShifting") ss >> colorShifting;
        }
        builder.operate<COMBExcludeSmooth>(nullptr, smoothing, colorBlending, colorShifting);
    }},

    // Objects
    { "SDFObject", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
        }
        builder.pushSDFObj<SDFObject>(color);
    }},
    { "SDFPlane", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec3 normal{ SDFPlane::DFLT_NORMAL };
        double offset{ SDFPlane::DFLT_OFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "normal") ss >> normal;
            else if (param == "color") ss >> color;
            else if (param == "offset") ss >> offset;
        }
        builder.pushSDFObj<SDFPlane>(normal.getNormal(), offset, color);
    } },
    { "SDFBox", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec3 dimensions{ SDFBox::DFLT_DIMENSIONS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "dimensions") ss >> dimensions;
            else if (param == "color") ss >> color;
        }
        builder.pushSDFObj<SDFBox>(dimensions, color);
    } },
    { "SDFBoxFrame", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double edgeThickness{ SDFBoxFrame::DFLT_EDGETHICKNESS };
        Vec3 dimensions{ SDFBoxFrame::DFLT_DIMENSIONS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "dimensions") ss >> dimensions;
            else if (param == "color") ss >> color;
            else if (param == "edgeThickness") ss >> edgeThickness;
        }
        builder.pushSDFObj<SDFBoxFrame>(dimensions, edgeThickness, color);
    }},
    { "SDFSphere", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFSphere::DFLT_RADIUS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
        }
        builder.pushSDFObj<SDFSphere>(radius, color);
    } },
    { "SDFTorus", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorus::DFLT_RADII };
        int axis{ Axis::AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radii") ss >> radii;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFTorus>(static_cast<Axis>(axis), radii, color);
    } },
    { "SDFTorusSegment", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusSegment::DFLT_RADII };
        double sweep{ SDFTorusSegment::DFLT_SWEEP };
        int axis{ Axis::AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radii") ss >> radii;
            else if (param == "sweep") ss >> sweep;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFTorusSegment>(static_cast<Axis>(axis), radii, sweep, color);
    } },
    { "SDFCapsule", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFCapsule::DFLT_RADIUS };
        double bodyLength{ SDFCapsule::DFLT_BODYLENGTH };
        int axis{ AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
            else if (param == "bodyLength") ss >> bodyLength;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFCapsule>(static_cast<Axis>(axis), radius, bodyLength, color);
    } },
    { "SDFCrystal", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double edgeLength{ SDFCrystal::DFLT_EDGELENGTH };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "edgeLength") ss >> edgeLength;
        }
        builder.pushSDFObj<SDFCrystal>(edgeLength, color);
    } },
    { "SDFCylinder", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double sideLength{ SDFCylinder::DFLT_SIDELENGTH };
        double radius{ SDFCylinder::DFLT_RADIUS };
        int axis{ Axis::AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radius") ss >> radius;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFCylinder>(static_cast<Axis>(axis), radius, sideLength, color);
    } },
    { "SDFCone", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double height{ SDFCone::DFLT_HEIGHT };
        double sweep{ SDFCone::DFLT_SWEEP };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "height") ss >> height;
            else if (param == "sweep") ss >> sweep;
        }
        builder.pushSDFObj<SDFCone>(height, sweep, color);
    } },
    { "SDFSolidAngle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFSolidAngle::DFLT_RADIUS };
        double sweep{ SDFSolidAngle::DFLT_SWEEP };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
            else if (param == "sweep") ss >> sweep;
        }
        builder.pushSDFObj<SDFSolidAngle>(radius, sweep, color);
    } },
    { "SDFEllipsoid", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec3 radii{ SDFEllipsoid::DFLT_RADII };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radii") ss >> radii;
        }
        builder.pushSDFObj<SDFEllipsoid>(radii, color);
    } },
    { "SDFTaperedCapsule", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double bodyLength{ SDFTaperedCapsule::DFLT_BODYLENGTH };
        Vec2 radii{ SDFTaperedCapsule::DFLT_RADII };
        int axis{ AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "bodyLength") ss >> bodyLength;
            else if (param == "radii") ss >> radii;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFTaperedCapsule>(static_cast<Axis>(axis), radii, bodyLength, color);
    } },
    { "SDFTaperedCylinder", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double bodyLength{ SDFTaperedCylinder::DFLT_SIDELENGTH };
        Vec2 radii{ SDFTaperedCylinder::DFLT_RADII };
        int axis{ AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "sideLength") ss >> bodyLength;
            else if (param == "radii") ss >> radii;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFTaperedCylinder>(static_cast<Axis>(axis), radii, bodyLength, color);
    } },
    { "SDFRhombus", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 dimensions{ SDFRhombus::DFLT_DIMENSIONS };
        double magnitude{ SDFRhombus::DFLT_MAGNITUDE };
        int operation{ SDFRhombus::DFLT_OPERATION };
        double revolveRotations{ SDFRhombus::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFRhombus::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "dimensions") ss >> dimensions;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFRhombus>(static_cast<OP2D>(operation), dimensions, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFEquilateralTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double base{ SDFEquilateralTriangle::DFLT_BASE };
        double magnitude{ SDFEquilateralTriangle::DFLT_MAGNITUDE };
        int operation{ SDFEquilateralTriangle::DFLT_OPERATION };
        double revolveRotations{ SDFEquilateralTriangle::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFEquilateralTriangle::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "base") ss >> base;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFEquilateralTriangle>(static_cast<OP2D>(operation), base, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFIsoscelesTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 dimensions{ SDFIsoscelesTriangle::DFLT_DIMENSIONS };
        double magnitude{ SDFIsoscelesTriangle::DFLT_MAGNITUDE };
        int operation{ SDFIsoscelesTriangle::DFLT_OPERATION };
        double revolveRotations{ SDFIsoscelesTriangle::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFIsoscelesTriangle::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "dimensions") ss >> dimensions;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFIsoscelesTriangle>(static_cast<OP2D>(operation), dimensions, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFPieSlice", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double sweep{ SDFPieSlice::DFLT_SWEEP };
        double radius{ SDFPieSlice::DFLT_RADIUS };
        double magnitude{ SDFPieSlice::DFLT_MAGNITUDE };
        int operation{ SDFPieSlice::DFLT_OPERATION };
        double revolveRotations{ SDFPieSlice::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFPieSlice::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "sweep") ss >> sweep;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFPieSlice>(static_cast<OP2D>(operation), sweep, radius, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFPentagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFPentagon::DFLT_RADIUS };
        double magnitude{ SDFPentagon::DFLT_MAGNITUDE };
        int operation{ SDFPentagon::DFLT_OPERATION };
        double revolveRotations{ SDFPentagon::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFPentagon::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFPentagon>(static_cast<OP2D>(operation), radius, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFHexagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFHexagon::DFLT_RADIUS };
        double magnitude{ SDFHexagon::DFLT_MAGNITUDE };
        int operation{ SDFHexagon::DFLT_OPERATION };
        double revolveRotations{ SDFHexagon::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFHexagon::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFHexagon>(static_cast<OP2D>(operation), radius, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFOctagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFOctagon::DFLT_RADIUS };
        double magnitude{ SDFOctagon::DFLT_MAGNITUDE };
        int operation{ SDFOctagon::DFLT_OPERATION };
        double revolveRotations{ SDFOctagon::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFOctagon::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFOctagon>(static_cast<OP2D>(operation), radius, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFStar", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFStar::DFLT_RADIUS };
        double ratio{ SDFStar::DFLT_RATIO };
        double magnitude{ SDFStar::DFLT_MAGNITUDE };
        size_t numPoints{ SDFStar::DFLT_NUMPOINT };
        int operation{ SDFStar::DFLT_OPERATION };
        double revolveRotations{ SDFStar::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFStar::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "radius") ss >> radius;
            else if (param == "numPoints") ss >> numPoints;
            else if (param == "ratio") ss >> ratio;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFStar>(static_cast<OP2D>(operation), numPoints, radius, ratio, magnitude, revolveRotations, revolveRotateOffset, color);
    } },
    { "SDFTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double magnitude{ SDFTriangle::DFLT_MAGNITUDE };
        Vec2 point1{ SDFTriangle::DFLT_POINT1 };
        Vec2 point2{ SDFTriangle::DFLT_POINT2 };
        Vec2 point3{ SDFTriangle::DFLT_POINT3 };
        int operation{ SDFTriangle::DFLT_OPERATION };
        double revolveRotations{ SDFTriangle::DFLT_REVOLVEROTATIONS };
        Vec2 revolveRotateOffset{ SDFTriangle::DFLT_REVOLVEROTATEOFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color;
            else if (param == "point1") ss >> point1;
            else if (param == "point2") ss >> point2;
            else if (param == "point3") ss >> point3;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset;
        }
        builder.pushSDFObj<SDFTriangle>(static_cast<OP2D>(operation), point1, point2, point3, magnitude, revolveRotations, revolveRotateOffset, color);
    } },

    // Transformations
    { "OPTranslate", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 translation{ OPTranslate::DFLT_TRANSLATION };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "translation") ss >> translation;
        }
        builder.operate<OPTranslate>(translation);
    } },
    { "OPCubeRollHeight", [](SDFBuilder& builder, std::stringstream& ss) {
        double cubeHeight{ OPCubeRollHeight::DFLT_CUBEHEIGHT };
        double period{ OPCubeRollHeight::DFLT_PERIOD };
        double phase{ OPCubeRollHeight::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "cubeHeight") ss >> cubeHeight;
            else if (param == "period") ss >> period;
            else if (param == "phase") ss >> phase;
        }
        builder.operate<OPCubeRollHeight>(cubeHeight, period, phase);
    } },
    { "OPRotate", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ OPRotate::DFLT_AXIS };
        double rotations{ OPRotate::DFLT_ROTATIONS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis;
            else if (param == "rotations") ss >> rotations;
        }
        builder.operate<OPRotate>(axis.getNormal(), rotations);
    } },
    { "OPCubeRoller", [](SDFBuilder& builder, std::stringstream& ss) {
        std::vector<Vec3> axes{ OPCubeRoller::DFLT_AXES };
        double quarterTurnTime{ OPCubeRoller::DFLT_QUARTERTURNTIME };
        double phase{ OPCubeRoller::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axes") {
                axes.clear();
                size_t numAxes{};
                ss >> numAxes;
                for (size_t i{}; i < numAxes; ++i) {
                    Vec3 axis{};
                    ss >> axis.x >> axis.y >> axis.z;
                    axes.push_back(axis.getNormal());
                }
            }
            else if (param == "quarterTurnTime") ss >> quarterTurnTime;
            else if (param == "phase") ss >> phase;
        }
        builder.operate<OPCubeRoller>(axes, quarterTurnTime, phase);
    } },
    { "OPElongate", [](SDFBuilder& builder, std::stringstream& ss) {
        double length{ OPElongate::DFLT_LENGTH };
        double offset{ OPElongate::DFLT_OFFSET };
        int axis{ AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
            else if (param == "axis") ss >> axis;
        }
        builder.operate<OPElongate>(static_cast<Axis>(axis), length, offset);
    } },

    // Resurface Ops
    { "OPRoundOff", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ OPRoundOff::DFLT_MAGNITUDE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.operate<OPRoundOff>(magnitude);
    } },
    { "OPOnion", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ OPOnion::DFLT_MAGNITUDE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.operate<OPOnion>(magnitude);
    } },
    { "OPInsideOut", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.operate<OPInsideOut>();
    } },

    // Domain Ops
    { "OPRepeat", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };
        Vec3 limits{ OPRepeat::DFLT_LIMITS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing;
            else if (param == "limits") ss >> limits;
        }
        builder.operate<OPRepeat>(limits, spacing);
    } },
    { "OPRepeatCircle", [](SDFBuilder& builder, std::stringstream& ss) {
        size_t repetitions{ OPRepeatCircle::DFLT_REPETITIONS };
        int axis{ AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "repetitions") ss >> repetitions;
            else if (param == "axis") ss >> axis;
        }
        builder.operate<OPRepeatCircle>(static_cast<Axis>(axis), repetitions);
    } },
    { "OPMirror", [](SDFBuilder& builder, std::stringstream& ss) {

        bool mirrorX{ true };
        bool mirrorY{};
        bool mirrorZ{};
        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "mirrorX") ss >> mirrorX;
            else if (param == "mirrorY") ss >> mirrorY;
            else if (param == "mirrorZ") ss >> mirrorZ;
        }
        builder.operate<OPMirror>(mirrorX, mirrorY, mirrorZ);
    } },

    // Animations
    { "ANContinuous1D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double amplitude{ ANContinuous1D::DFLT_AMPLITUDE };
        double phase{ ANContinuous1D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANContinuous1D>(propName, amplitude, phase);
    } },
    { "ANContinuous2D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 amplitude{ ANContinuous2D::DFLT_AMPLITUDE };
        Vec2 phase{ ANContinuous2D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANContinuous2D>(propName, amplitude, phase);
    } },
    { "ANContinuous3D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 amplitude{ ANContinuous3D::DFLT_AMPLITUDE };
        Vec3 phase{ ANContinuous3D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANContinuous3D>(propName, amplitude, phase);
    } },
    { "ANSmoothCycle1D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ ANSmoothCycle1D::DFLT_PERIOD };
        double amplitude{ ANSmoothCycle1D::DFLT_AMPLITUDE };
        double phase{ ANSmoothCycle1D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANSmoothCycle1D>(propName, period, amplitude, phase);
    } },
    { "ANSmoothCycle2D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 periods{ ANSmoothCycle2D::DFLT_PERIOD };
        Vec2 amplitudes{ ANSmoothCycle2D::DFLT_AMPLITUDE };
        Vec2 phases{ ANSmoothCycle2D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> periods;
            else if (param == "amplitude") ss >> amplitudes;
            else if (param == "phase") ss >> phases;
        }
        builder.animate<ANSmoothCycle2D>(propName, periods, amplitudes, phases);
    } },
    { "ANSmoothCycle3D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ ANSmoothCycle3D::DFLT_PERIOD };
        Vec3 amplitudes{ ANSmoothCycle3D::DFLT_AMPLITUDE };
        Vec3 phases{ ANSmoothCycle3D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> periods;
            else if (param == "amplitude") ss >> amplitudes;
            else if (param == "phase") ss >> phases;
        }
        builder.animate<ANSmoothCycle3D>(propName, periods, amplitudes, phases);
    } },
    { "ANLinearCycle1D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double forwardTime{ ANLinearCycle1D::DFLT_FORWARDTIME };
        double delayTime1{ ANLinearCycle1D::DFLT_DELAYTIME1 };
        double backwardTime{ ANLinearCycle1D::DFLT_BACKWARDTIME };
        double delayTime2{ ANLinearCycle1D::DFLT_DELAYTIME2 };
        double amplitude{ ANLinearCycle1D::DFLT_AMPLITUDE };
        double phase{ ANLinearCycle1D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "forwardTime") ss >> forwardTime;
            else if (param == "delayTime1") ss >> delayTime1;
            else if (param == "backwardTime") ss >> backwardTime;
            else if (param == "delayTime2") ss >> delayTime2;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANLinearCycle1D>(propName, amplitude, forwardTime, delayTime1, backwardTime, delayTime2, phase);
    } },
    { "ANLinearCycle2D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 forwardTime{ ANLinearCycle1D::DFLT_FORWARDTIME };
        Vec2 delayTime1{ ANLinearCycle1D::DFLT_DELAYTIME1 };
        Vec2 backwardTime{ ANLinearCycle1D::DFLT_BACKWARDTIME };
        Vec2 delayTime2{ ANLinearCycle1D::DFLT_DELAYTIME2 };
        Vec2 amplitude{ ANLinearCycle1D::DFLT_AMPLITUDE };
        Vec2 phase{ ANLinearCycle1D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "forwardTime") ss >> forwardTime;
            else if (param == "delayTime1") ss >> delayTime1;
            else if (param == "backwardTime") ss >> backwardTime;
            else if (param == "delayTime2") ss >> delayTime2;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANLinearCycle2D>(propName, amplitude, forwardTime, delayTime1, backwardTime, delayTime2, phase);
    } },
    { "ANLinearCycle3D", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 forwardTime{ ANLinearCycle1D::DFLT_FORWARDTIME };
        Vec3 delayTime1{ ANLinearCycle1D::DFLT_DELAYTIME1 };
        Vec3 backwardTime{ ANLinearCycle1D::DFLT_BACKWARDTIME };
        Vec3 delayTime2{ ANLinearCycle1D::DFLT_DELAYTIME2 };
        Vec3 amplitude{ ANLinearCycle1D::DFLT_AMPLITUDE };
        Vec3 phase{ ANLinearCycle1D::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "forwardTime") ss >> forwardTime;
            else if (param == "delayTime1") ss >> delayTime1;
            else if (param == "backwardTime") ss >> backwardTime;
            else if (param == "delayTime2") ss >> delayTime2;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.animate<ANLinearCycle3D>(propName, amplitude, forwardTime, delayTime1, backwardTime, delayTime2, phase);
    } },

    // LightSources
    { "LightSource", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 position{ LightSource::DFLT_POSITION };
        double luminance{ LightSource::DFLT_LUMINANCE };
        double ambientPower{ LightSource::DFLT_AMBIENTPOWER };
        Color color{ LightSource::DFLT_COLOR };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "position") ss >> position;
            else if (param == "luminance") ss >> luminance;
            else if (param == "ambientPower") ss >> ambientPower;
            else if (param == "color") ss >> color;
        }
        builder.addLightSource(position, luminance, color, ambientPower);
    } },
};
