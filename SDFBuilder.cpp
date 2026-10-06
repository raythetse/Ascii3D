#pragma once
#include "SDFBuilder.h"
#include "SDFObject.h"
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
        builder.pushOp<COMBAdd>();
    }},
    { "COMBSubtract", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<COMBSubtract>();
    }},
    { "COMBIntersect", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<COMBIntersect>();
    }},
    { "COMBIntersect", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<COMBIntersect>();
    }},
    { "COMBExclude", [](SDFBuilder& builder, std::stringstream& ss) {
        COMBExclude::ColorMode colorMode{};

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "colorMode") {
                int colorMode;
                ss >> colorMode;
                colorMode = static_cast<COMBExclude::ColorMode>(colorMode);
            }
        }
        builder.pushOp<COMBExclude>(nullptr, colorMode);
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
        builder.pushOp<COMBAddSmooth>(nullptr, smoothing, colorBlending, colorShifting);
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
        builder.pushOp<COMBSubtractSmooth>(nullptr, smoothing, colorBlending, colorShifting);
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
        builder.pushOp<COMBIntersectSmooth>(nullptr, smoothing, colorBlending, colorShifting);
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
    { "SDFTorusXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusXZ::DFLT_RADII };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTorusXZ>(radii, color);
    } },
    { "SDFTorusYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusYZ::DFLT_RADII };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTorusYZ>(radii, color);
    } },
    { "SDFTorusXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusXY::DFLT_RADII };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTorusXY>(radii, color);
    } },
    { "SDFTorusSegmentXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusSegmentXY::DFLT_RADII };
        double sweep{ SDFTorusSegmentXY::DFLT_SWEEP };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
            else if (param == "sweep") ss >> sweep;
        }
        builder.pushSDFObj<SDFTorusSegmentXY>(radii, sweep, color);
    } },
    { "SDFTorusSegmentXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusSegmentXZ::DFLT_RADII };
        double sweep{ SDFTorusSegmentXZ::DFLT_SWEEP };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
            else if (param == "sweep") ss >> sweep;
        }
        builder.pushSDFObj<SDFTorusSegmentXZ>(radii, sweep, color);
    } },
    { "SDFTorusSegmentYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 radii{ SDFTorusSegmentYZ::DFLT_RADII };
        double sweep{ SDFTorusSegmentYZ::DFLT_SWEEP };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
            else if (param == "sweep") ss >> sweep;
        }
        builder.pushSDFObj<SDFTorusSegmentYZ>(radii, sweep, color);
    } },
    { "SDFCapsuleX", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFCapsuleX::DFLT_RADIUS };
        double bodyLength{ SDFCapsuleX::DFLT_BODYLENGTH };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "bodyLength") ss >> bodyLength;
        }
        builder.pushSDFObj<SDFCapsuleX>(radius, bodyLength, color);
    } },
    { "SDFCapsuleY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFCapsuleY::DFLT_RADIUS };
        double bodyLength{ SDFCapsuleY::DFLT_BODYLENGTH };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "bodyLength") ss >> bodyLength;
        }
        builder.pushSDFObj<SDFCapsuleY>(radius, bodyLength, color);
    } },
    { "SDFCapsuleZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFCapsuleZ::DFLT_RADIUS };
        double bodyLength{ SDFCapsuleZ::DFLT_BODYLENGTH };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "bodyLength") ss >> bodyLength;
        }
        builder.pushSDFObj<SDFCapsuleZ>(radius, bodyLength, color);
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
    { "SDFCylinderX", [](SDFBuilder& builder, std::stringstream& ss) {
        Color col{ Color::White };
        double sideLength{ SDFCylinderX::DFLT_SIDELENGTH };
        double radius{ SDFCylinderX::DFLT_RADIUS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> col.x >> col.y >> col.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radius") ss >> radius;
        }
        builder.pushSDFObj<SDFCylinderX>(radius, sideLength, col);
    } },
    { "SDFCylinderY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double sideLength{ SDFCylinderY::DFLT_SIDELENGTH };
        double radius{ SDFCylinderY::DFLT_RADIUS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radius") ss >> radius;
        }
        builder.pushSDFObj<SDFCylinderY>(radius, sideLength, color);
    } },
    { "SDFCylinderZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double sideLength{ SDFCylinderZ::DFLT_SIDELENGTH };
        double radius{ SDFCylinderZ::DFLT_RADIUS };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radius") ss >> radius;
        }
        builder.pushSDFObj<SDFCylinderZ>(radius, sideLength, color);
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

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "bodyLength") ss >> bodyLength;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTaperedCapsule>(radii, bodyLength, color);
    } },
    { "SDFRhombus", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 dimensions{ SDFRhombus::DFLT_DIMENSIONS };
        double magnitude{ SDFRhombus::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "dimensions") ss >> dimensions.x >> dimensions.y;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFRhombus>(operation, dimensions, magnitude, color);
    } },
    { "SDFEquilateralTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double base{ SDFEquilateralTriangle::DFLT_BASE };
        double magnitude{ SDFEquilateralTriangle::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFEquilateralTriangle::DFLT_OPERATION };


        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "base") ss >> base;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFEquilateralTriangle>(operation, base, magnitude, color);
    } },
    { "SDFIsoscelesTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        Vec2 dimensions{ SDFIsoscelesTriangle::DFLT_DIMENSIONS };
        double magnitude{ SDFIsoscelesTriangle::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFIsoscelesTriangle::DFLT_OPERATION };


        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "dimensions") ss >> dimensions.x >> dimensions.y;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFIsoscelesTriangle>(operation, dimensions, magnitude, color);
    } },
    { "SDFPieSlice", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double sweep{ SDFPieSlice::DFLT_SWEEP };
        double radius{ SDFPieSlice::DFLT_RADIUS };
        double magnitude{ SDFPieSlice::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFPieSlice::DFLT_OPERATION };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sweep") ss >> sweep;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFPieSlice>(operation, sweep, radius, magnitude, color);
    } },
    { "SDFPentagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFPentagon::DFLT_RADIUS };
        double magnitude{ SDFPentagon::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFPentagon::DFLT_OPERATION };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFPentagon>(operation, radius, magnitude, color);
    } },
    { "SDFHexagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFHexagon::DFLT_RADIUS };
        double magnitude{ SDFHexagon::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFHexagon>(operation, radius, magnitude, color);
    } },
    { "SDFOctagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFOctagon::DFLT_RADIUS };
        double magnitude{ SDFOctagon::DFLT_MAGNITUDE };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFOctagon>(operation, radius, magnitude, color);
    } },
    { "SDFStar", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double radius{ SDFStar::DFLT_RADIUS };
        double ratio{ SDFStar::DFLT_RATIO };
        double magnitude{ SDFStar::DFLT_MAGNITUDE };
        size_t numPoints{ SDFStar::DFLT_NUMPOINT };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
            else if (param == "numPoints") ss >> numPoints;
            else if (param == "ratio") ss >> ratio;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFStar>(operation, numPoints, radius, ratio, magnitude, color);
    } },
    { "SDFTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ Color::White };
        double magnitude{ SDFTriangle::DFLT_MAGNITUDE };
        Vec2 point1{ SDFTriangle::DFLT_POINT1 };
        Vec2 point2{ SDFTriangle::DFLT_POINT2 };
        Vec2 point3{ SDFTriangle::DFLT_POINT3 };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "point1") ss >> point1.x >> point1.y;
            else if (param == "point2") ss >> point2.x >> point2.y;
            else if (param == "point3") ss >> point3.x >> point3.y;
            else if (param == "magnitude") ss >> magnitude;
            else if (param == "operation") {
                int opID;
                ss >> opID;
                operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFTriangle>(operation, point1, point2, point3, magnitude, color);
    } },

    // Mirrors
    { "OPMirrorX", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorX>();
    } },
    { "OPMirrorY", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorY>();
    } },
    { "OPMirrorZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorZ>();
    } },
    { "OPMirrorXY", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorXY>();
    } },
    { "OPMirrorXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorXZ>();
    } },
    { "OPMirrorYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorYZ>();
    } },
    { "OPMirrorXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPMirrorXYZ>();
    } },

    // Transformations
    { "OPTranslate", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 translation{ OPTranslate::DFLT_TRANSLATION };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "translation") ss >> translation.x >> translation.y >> translation.z;
        }
        builder.pushOp<OPTranslate>(translation);
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
        builder.pushOp<OPTranslateSine>(translation, amplitude, period, phase);
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
        builder.pushOp<OPTranslateLinear>(translation, transitTimes, endDelays, phase);
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
        builder.pushOp<OPCubeRollHeight>(cubeHeight, period, phase);
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
        builder.pushOp<OPRotate>(axis, rotations);
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
        builder.pushOp<OPRotateContinuous>(axis, rotationsPerSecond, phase);
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
        builder.pushOp<OPRotateScan>(axis, rotations, transitTimes, endDelays, phase);
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
        builder.pushOp<OPRotateDelay>(axis, rotations, rotateTime, delayTime, phase);
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
        builder.pushOp<OPCubeRoller>(axes, quarterTurnTime, phase);
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
        builder.pushOp<OPElongateX>(length, offset);
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
        builder.pushOp<OPElongateY>(length, offset);
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
        builder.pushOp<OPElongateZ>(length, offset);
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
        builder.pushOp<OPElongateXYZ>(lengths, offsets);
    } },

    // Resurface Ops
    { "OPRoundOff", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ OPRoundOff::DFLT_MAGNITUDE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.pushOp<OPRoundOff>(magnitude);
    } },
    { "OPOnion", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ OPOnion::DFLT_MAGNITUDE };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.pushOp<OPOnion>(magnitude);
    } },
    { "OPInsideOut", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPInsideOut>();
    } },

    // Domain Ops
    { "OPRepeatX", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatX>(spacing);
    } },
    { "OPRepeatY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatY>(spacing);
    } },
    { "OPRepeatZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatZ>(spacing);
    } },
    { "OPRepeatXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatXY>(spacing);
    } },
    { "OPRepeatXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatXZ>(spacing);
    } },
    { "OPRepeatYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatYZ>(spacing);
    } },
    { "OPRepeatXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DomainOperation::DFLT_SPACING };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatXYZ>(spacing);
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
        builder.pushOp<OPRepeatLimited>(limits, spacing);
    } },

    // Property Modifiers
    { "PMOscillateSine", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ PropertyModifierBase::DFLT_PERIOD };
        double amplitude{ PropertyModifierBase::DFLT_AMPLITUDE };
        double phase{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modOp<PMOscillateSine>(propName, period, amplitude, phase) : builder.modOp<PMOscillateSine>(propName, period, amplitude, phase);
    } },
    { "PMOscillateSineST", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ PropertyModifierBase::DFLT_PERIOD };
        double amplitude{ PropertyModifierBase::DFLT_AMPLITUDE };
        double phase{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modOp<PMOscillateSineST>(propName, period, amplitude, phase) : builder.modOp<PMOscillateSineST>(propName, period, amplitude, phase);
    } },
    { "PMOscillateLinearST", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ PropertyModifierBase::DFLT_PERIOD };
        double amplitude{ PropertyModifierBase::DFLT_AMPLITUDE };
        double phase{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modOp<PMOscillateLinearST>(propName, period, amplitude, phase) : builder.modOp<PMOscillateLinearST>(propName, period, amplitude, phase);
    } },
    { "PMOscillateSineV2", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 periods{ PropertyModifierBase::DFLT_PERIOD };
        Vec2 amplitudes{ PropertyModifierBase::DFLT_AMPLITUDE };
        Vec2 phases{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modObj<PMOscillateSineV2>(propName, periods, amplitudes, phases) : builder.modOp<PMOscillateSineV2>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateSineV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ PropertyModifierBase::DFLT_PERIOD };
        Vec3 amplitudes{ PropertyModifierBase::DFLT_AMPLITUDE };
        Vec3 phases{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modObj<PMOscillateSineV3>(propName, periods, amplitudes, phases) : builder.modOp<PMOscillateSineV3>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ PropertyModifierBase::DFLT_PERIOD };
        double amplitude{ PropertyModifierBase::DFLT_AMPLITUDE };
        double phase{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modObj<PMOscillateLinear>(propName, period, amplitude, phase) : builder.modOp<PMOscillateLinear>(propName, period, amplitude, phase);
    } },
    { "PMOscillateLinearV2", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec2 periods{ PropertyModifierBase::DFLT_PERIOD };
        Vec2 amplitudes{ PropertyModifierBase::DFLT_AMPLITUDE };
        Vec2 phases{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modObj<PMOscillateLinearV2>(propName, periods, amplitudes, phases) : builder.modOp<PMOscillateLinearV2>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateLinearV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ PropertyModifierBase::DFLT_PERIOD };
        Vec3 amplitudes{ PropertyModifierBase::DFLT_AMPLITUDE };
        Vec3 phases{ PropertyModifierBase::DFLT_PHASE };
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
        boundOnObj ? builder.modObj<PMOscillateLinearV3>(propName, periods, amplitudes, phases) : builder.modOp<PMOscillateLinearV3>(propName, periods, amplitudes, phases);
    } },
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
