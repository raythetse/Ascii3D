#pragma once
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

void SDFBuilder::loadSerialized(std::vector<std::string>& strVec)
{
    m_baseObject = std::make_unique<SDFObject>();
    m_objChain.push_back(m_baseObject.get());

    SDFBuilder builder{};
    for (size_t i{1}; i < strVec.size(); ++i) {
        std::replace(strVec[i].begin(), strVec[i].end(), ',', ' ');
        std::stringstream ss{ strVec[i] };
        std::string opName;
        ss >> opName;
        //std::cout << opName << '\n';
        if (s_opMap.contains(opName)) {
            s_opMap.at(opName)(*this, ss);
        }
    }
}

const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> SDFBuilder::s_opMap = {
    // Step Back
    { "stepBack", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.stepBack();
    }},

    // Combinations
    { "COMBAdd", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<COMBAdd>();
    }},
    { "COMBSubtract", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<COMBSubtract>();
    }},
    { "COMBIntersect", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<COMBIntersect>();
    }},
    { "COMBIntersect", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<COMBIntersect>();
    }},
    { "COMBExclude", [](SDFBuilder& builder, std::stringstream& ss) {
        int colorMode{ColorMode::COLORMODE_DOUBLE};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "colorMode") ss >> colorMode;
        }
        builder.op<COMBExclude>(nullptr, static_cast<ColorMode>(colorMode));
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
        builder.op<COMBAddSmooth>(nullptr, smoothing, colorBlending, colorShifting);
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
        builder.op<COMBSubtractSmooth>(nullptr, smoothing, colorBlending, colorShifting);
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
        builder.op<COMBIntersectSmooth>(nullptr, smoothing, colorBlending, colorShifting);
    }},

    // Objects
    { "SDFObject", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "normal") ss >> normal.x >> normal.y >> normal.z;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "offset") ss >> offset;
        }
        builder.pushSDFObj<SDFPlane>(normal, offset, color);
    } },
    { "SDFBox", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec3 dimensions{ SDFBox::DFLT_DIMENSIONS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "dimensions") ss >> dimensions.x >> dimensions.y >> dimensions.z;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "dimensions") ss >> dimensions.x >> dimensions.y >> dimensions.z;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "edgeLength") ss >> edgeLength;
        }
        builder.pushSDFObj<SDFCrystal>(edgeLength, color);
    } },
    { "SDFCylinder", [](SDFBuilder& builder, std::stringstream& ss) {
        Color col{ Color::White };
        double sideLength{ SDFCylinder::DFLT_SIDELENGTH };
        double radius{ SDFCylinder::DFLT_RADIUS };
        int axis{ Axis::AXIS_Y };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> col.x >> col.y >> col.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radius") ss >> radius;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFCylinder>(static_cast<Axis>(axis), radius, sideLength, col);
    } },
    { "SDFCone", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double height{ SDFCone::DFLT_HEIGHT };
        double sweep{ SDFCone::DFLT_SWEEP };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y >> radii.z;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "bodyLength") ss >> bodyLength;
            else if (param == "radii") ss >> radii.x >> radii.y;
            else if (param == "axis") ss >> axis;
        }
        builder.pushSDFObj<SDFTaperedCapsule>(static_cast<Axis>(axis), radii, bodyLength, color);
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "dimensions") ss >> dimensions.x >> dimensions.y;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "base") ss >> base;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "dimensions") ss >> dimensions.x >> dimensions.y;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sweep") ss >> sweep;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "numPoints") ss >> numPoints;
            else if (param == "ratio") ss >> ratio;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
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
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "point1") ss >> point1.x >> point1.y;
            else if (param == "point2") ss >> point2.x >> point2.y;
            else if (param == "point3") ss >> point3.x >> point3.y;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") ss >> operation;
            else if (param == "revolveRotations") ss >> revolveRotations;
            else if (param == "revolveRotateOffset") ss >> revolveRotateOffset.x >> revolveRotateOffset.y;
        }
        builder.pushSDFObj<SDFTriangle>(static_cast<OP2D>(operation), point1, point2, point3, magnitude, revolveRotations, revolveRotateOffset, color);
    } },

    // Mirrors
    { "OPMirrorX", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorX>();
    } },
    { "OPMirrorY", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorY>();
    } },
    { "OPMirrorZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorZ>();
    } },
    { "OPMirrorXY", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorXY>();
    } },
    { "OPMirrorXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorXZ>();
    } },
    { "OPMirrorYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorYZ>();
    } },
    { "OPMirrorXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPMirrorXYZ>();
    } },

    // Transformations
    { "OPTranslate", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 translation{ OPTranslate::DFLT_TRANSLATION };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "translation") ss >> translation.x >> translation.y >> translation.z;
        }
        builder.op<OPTranslate>(translation);
    } },
    { "OPTranslateSine", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 translation{ OPTranslateSine::DFLT_TRANSLATION };
        double amplitude{ OPTranslateSine::DFLT_AMPLITUDE };
        double period{ OPTranslateSine::DFLT_PERIOD };
        double phase{ OPTranslateSine::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "translation") ss >> translation.x >> translation.y >> translation.z;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "period") ss >> period;
            else if (param == "phase") ss >> phase;
        }
        builder.op<OPTranslateSine>(translation, amplitude, period, phase);
    } },
    { "OPTranslateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 translation{ OPTranslateLinear::DFLT_TRANSLATION };
        Vec2 transitTimes{ OPTranslateLinear::DFLT_TRANSITTIMES };
        Vec2 endDelays{ OPTranslateLinear::DFLT_ENDDELAYS };
        double phase{ OPTranslateLinear::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "translation") ss >> translation.x >> translation.y >> translation.z;
            else if (param == "transitTimes") ss >> transitTimes.x >> transitTimes.y;
            else if (param == "endDelays") ss >> endDelays.x >> endDelays.y;
            else if (param == "phase") ss >> phase;
        }
        builder.op<OPTranslateLinear>(translation, transitTimes, endDelays, phase);
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
        builder.op<OPCubeRollHeight>(cubeHeight, period, phase);
    } },
    { "OPRotate", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ OPRotate::DFLT_AXIS };
        double rotations{ OPRotate::DFLT_ROTATIONS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis.x >> axis.y >> axis.z;
            else if (param == "rotations") ss >> rotations;
        }
        builder.op<OPRotate>(axis, rotations);
    } },
    { "OPRotateContinuous", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ OPRotateContinuous::DFLT_AXIS };
        double rotationsPerSecond{ OPRotateContinuous::DFLT_ROTATIONSPERSECOND };
        double phase{ OPRotateContinuous::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis.x >> axis.y >> axis.z;
            else if (param == "rotationsPerSecond") ss >> rotationsPerSecond;
            else if (param == "phase") ss >> phase;
        }
        builder.op<OPRotateContinuous>(axis, rotationsPerSecond, phase);
    } },
    { "OPRotateScan", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ OPRotateScan::DFLT_AXIS };
        double rotations{ OPRotateScan::DFLT_ROTATIONS };
        Vec2 transitTimes{ OPRotateScan::DFLT_TRANSTIMES };
        Vec2 endDelays{ OPRotateScan::DFLT_ENDDELAYS };
        double phase{ OPRotateScan::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis.x >> axis.y >> axis.z;
            else if (param == "rotations") ss >> rotations;
            else if (param == "transitTimes") ss >> transitTimes.x >> transitTimes.y;
            else if (param == "endDelays") ss >> endDelays.x >> endDelays.y;
            else if (param == "phase") ss >> phase;
        }
        builder.op<OPRotateScan>(axis, rotations, transitTimes, endDelays, phase);
    } },
    { "OPRotateDelay", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ OPRotateDelay::DFLT_AXIS };
        double rotations{ OPRotateDelay::DFLT_ROTATIONS };
        double rotateTime{ OPRotateDelay::DFLT_ROTATETIME };
        double delayTime{ OPRotateDelay::DFLT_DELAYTIME };
        double phase{ OPRotateDelay::DFLT_PHASE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis.x >> axis.y >> axis.z;
            else if (param == "rotations") ss >> rotations;
            else if (param == "rotateTime") ss >> rotateTime;
            else if (param == "delayTime") ss >> delayTime;
            else if (param == "phase") ss >> phase;
        }
        builder.op<OPRotateDelay>(axis, rotations, rotateTime, delayTime, phase);
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
                int numAxes{};
                ss >> numAxes;
                for (size_t i{}; i < numAxes; ++i) {
                    Vec3 axis{};
                    ss >> axis.x >> axis.y >> axis.z;
                    axes.push_back(axis);
                }
            }
            else if (param == "quarterTurnTime") ss >> quarterTurnTime;
            else if (param == "phase") ss >> phase;
        }
        builder.op<OPCubeRoller>(axes, quarterTurnTime, phase);
    } },
    { "OPElongateX", [](SDFBuilder& builder, std::stringstream& ss) {
        double length{ OPElongateX::DFLT_LENGTH };
        double offset{ OPElongateX::DFLT_OFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
        }
        builder.op<OPElongateX>(length, offset);
    } },
    { "OPElongateY", [](SDFBuilder& builder, std::stringstream& ss) {
        double length{ OPElongateY::DFLT_LENGTH };
        double offset{ OPElongateY::DFLT_OFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
        }
        builder.op<OPElongateY>(length, offset);
    } },
    { "OPElongateZ", [](SDFBuilder& builder, std::stringstream& ss) {
        double length{ OPElongateZ::DFLT_LENGTH };
        double offset{ OPElongateZ::DFLT_OFFSET };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
        }
        builder.op<OPElongateZ>(length, offset);
    } },
    { "OPElongateXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 lengths{ OPElongateXYZ::DFLT_LENGTHS };
        Vec3 offsets{ OPElongateXYZ::DFLT_OFFSETS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "lengths") ss >> lengths.x >> lengths.y >> lengths.z;
            else if (param == "offsets") ss >> offsets.x >> offsets.y >> offsets.z;
        }
        builder.op<OPElongateXYZ>(lengths, offsets);
    } },

    // Resurface Ops
    { "OPRoundOff", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ OPRoundOff::DFLT_MAGNITUDE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.op<OPRoundOff>(magnitude);
    } },
    { "OPOnion", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ OPOnion::DFLT_MAGNITUDE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.op<OPOnion>(magnitude);
    } },
    { "OPInsideOut", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.op<OPInsideOut>();
    } },

    // Domain Ops
    { "OPRepeatX", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatX>(spacing);
    } },
    { "OPRepeatY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatY>(spacing);
    } },
    { "OPRepeatZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatZ>(spacing);
    } },
    { "OPRepeatXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatXY>(spacing);
    } },
    { "OPRepeatXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatXZ>(spacing);
    } },
    { "OPRepeatYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatYZ>(spacing);
    } },
    { "OPRepeatXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.op<OPRepeatXYZ>(spacing);
    } },
    { "OPRepeatLimited", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };
        Vec3 limits{ OPRepeatLimited::DFLT_LIMITS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
            else if (param == "limits") ss >> limits.x >> limits.y >> limits.z;
        }
        builder.op<OPRepeatLimited>(limits, spacing);
    } },

    // Property Modifiers
    { "PMOscillateSine", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ MODOscillateSine::DFLT_PERIOD };
        double amplitude{ MODOscillateSine::DFLT_AMPLITUDE };
        double phase{ MODOscillateSine::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        boundOnObj ? builder.modObj<MODOscillateSine>(propName, period, amplitude, phase) : builder.modOp<MODOscillateSine>(propName, period, amplitude, phase);
    } },
    { "PMOscillateSineST", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ MODOscillateSineST::DFLT_PERIOD };
        double amplitude{ MODOscillateSineST::DFLT_AMPLITUDE };
        double phase{ MODOscillateSineST::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        boundOnObj ? builder.modObj<MODOscillateSineST>(propName, period, amplitude, phase) : builder.modOp<MODOscillateSineST>(propName, period, amplitude, phase);
    } },
    { "PMOscillateLinearST", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ MODOscillateLinearST::DFLT_PERIOD };
        double amplitude{ MODOscillateLinearST::DFLT_AMPLITUDE };
        double phase{ MODOscillateLinearST::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        boundOnObj ? builder.modObj<MODOscillateLinearST>(propName, period, amplitude, phase) : builder.modOp<MODOscillateLinearST>(propName, period, amplitude, phase);
    } },
    { "PMOscillateSineV2", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 periods{ MODOscillateSineV2::DFLT_PERIOD };
        Vec2 amplitudes{ MODOscillateSineV2::DFLT_AMPLITUDE };
        Vec2 phases{ MODOscillateSineV2::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> periods.x >> periods.y;
            else if (param == "amplitude") ss >> amplitudes.x >> amplitudes.y;
            else if (param == "phase") ss >> phases.x >> phases.y;
        }
        boundOnObj ? builder.modObj<MODOscillateSineV2>(propName, periods, amplitudes, phases) : builder.modOp<MODOscillateSineV2>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateSineV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ MODOscillateSineV3::DFLT_PERIOD };
        Vec3 amplitudes{ MODOscillateSineV3::DFLT_AMPLITUDE };
        Vec3 phases{ MODOscillateSineV3::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> periods.x >> periods.y >> periods.z;
            else if (param == "amplitude") ss >> amplitudes.x >> amplitudes.y >> amplitudes.z;
            else if (param == "phase") ss >> phases.x >> phases.y >> phases.z;
        }
        boundOnObj ? builder.modObj<MODOscillateSineV3>(propName, periods, amplitudes, phases) : builder.modOp<MODOscillateSineV3>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ MODOscillateLinear::DFLT_PERIOD };
        double amplitude{ MODOscillateLinear::DFLT_AMPLITUDE };
        double phase{ MODOscillateLinear::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        boundOnObj ? builder.modObj<MODOscillateLinear>(propName, period, amplitude, phase) : builder.modOp<MODOscillateLinear>(propName, period, amplitude, phase);
    } },
    { "PMOscillateLinearV2", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 periods{ MODOscillateLinearV2::DFLT_PERIOD };
        Vec2 amplitudes{ MODOscillateLinearV2::DFLT_AMPLITUDE };
        Vec2 phases{ MODOscillateLinearV2::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> periods.x >> periods.y;
            else if (param == "amplitude") ss >> amplitudes.x >> amplitudes.y;
            else if (param == "phase") ss >> phases.x >> phases.y;
        }
        boundOnObj ? builder.modObj<MODOscillateLinearV2>(propName, periods, amplitudes, phases) : builder.modOp<MODOscillateLinearV2>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateLinearV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ MODOscillateLinearV3::DFLT_PERIOD };
        Vec3 amplitudes{ MODOscillateLinearV3::DFLT_AMPLITUDE };
        Vec3 phases{ MODOscillateLinearV3::DFLT_PHASE };
        bool boundOnObj{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "boundOnObj") ss >> boundOnObj;
            else if (param == "period") ss >> periods.x >> periods.y >> periods.z;
            else if (param == "amplitude") ss >> amplitudes.x >> amplitudes.y >> amplitudes.z;
            else if (param == "phase") ss >> phases.x >> phases.y >> phases.z;
        }
        boundOnObj ? builder.modObj<MODOscillateLinearV3>(propName, periods, amplitudes, phases) : builder.modOp<MODOscillateLinearV3>(propName, periods, amplitudes, phases);
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
            if (param == "position") ss >> position.x >> position.y >> position.z;
            else if (param == "luminance") ss >> luminance;
            else if (param == "ambientPower") ss >> ambientPower;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
        }
        builder.addLightSource(position, luminance, color, ambientPower);
    } },
};
