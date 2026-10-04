#pragma once
#include "SDFBuilder.h"
#include "SDFDefaults.h"
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
        double smoothing{ DEF::COMBSmooth };
        double colorBlending{ DEF::COMBColBlend };
        double colorShifting{ DEF::COMBColShift };

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
        double smoothing{ DEF::COMBSmooth };
        double colorBlending{ DEF::COMBColBlend };
        double colorShifting{ DEF::COMBColShift };

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
        double smoothing{ DEF::COMBSmooth };
        double colorBlending{ DEF::COMBColBlend };
        double colorShifting{ DEF::COMBColShift };

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
        Color color{ DEF::OBJColor };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
        }
        builder.pushSDFObj<SDFObject>(color);
    }},
    { "SDFPlaneXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double yPos{ DEF::OBJPlanePos };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "yPos") ss >> yPos;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
        }
        builder.pushSDFObj<SDFPlaneXZ>(yPos, color);
    }},
    { "SDFPlaneXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double zPos{ DEF::OBJPlanePos };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "zPos") ss >> zPos;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
        }
        builder.pushSDFObj<SDFPlaneXY>(zPos, color);
    }},
    { "SDFPlaneYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double xPos{ DEF::OBJPlanePos };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "xPos") ss >> xPos;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
        }
        builder.pushSDFObj<SDFPlaneYZ>(xPos, color);
    }},
    { "SDFPlane", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        Vec3 normal{ DEF::OBJNormal };
        double offset{ DEF::OBJOffset };

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
        Color color{ DEF::OBJColor };
        Vec3 dimensions{ DEF::OBJBoxDimensions };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "dimensions") ss >> dimensions.x >> dimensions.y >> dimensions.z;
            else if (param == "color") ss >> color.x >> color.y >> color.z;
        }
        builder.pushSDFObj<SDFBox>(dimensions, color);
    } },
    { "SDFBoxFrame", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double edgeThickness{ DEF::OBJFrameThickness };
        Vec3 dimensions{ DEF::OBJBoxDimensions };

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
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJRadius };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radius") ss >> radius;
        }
        builder.pushSDFObj<SDFSphere>(radius, color);
    } },
    { "SDFTorusXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        Vec2 radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTorusXZ>(radii, color);
    } },
    { "SDFTorusYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        Vec2 radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTorusYZ>(radii, color);
    } },
    { "SDFTorusXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        Vec2 radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTorusXY>(radii, color);
    } },
    { "SDFTorusSegment", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        Vec2 radii{ DEF::OBJRadii };
        double sweep{ DEF::OBJSweep };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y;
            else if (param == "sweep") ss >> sweep;
        }
        builder.pushSDFObj<SDFTorusSegment>(radii, sweep, color);
    } },
    { "SDFCapsuleX", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJRadius };
        double bodyLength{ DEF::OBJLength };

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
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJRadius };
        double bodyLength{ DEF::OBJLength };

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
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJRadius };
        double bodyLength{ DEF::OBJLength };

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
        Color color{ DEF::OBJColor };
        double edgeLength{ DEF::OBJLength };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "edgeLength") ss >> edgeLength;
        }
        builder.pushSDFObj<SDFCrystal>(edgeLength, color);
    } },
    { "SDFCylinderX", [](SDFBuilder& builder, std::stringstream& ss) {
        Color col{ DEF::OBJColor };
        double sideLength{ DEF::OBJLength };
        double radius{ DEF::OBJRadius };

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
        Color color{ DEF::OBJColor };
        double sideLength{ DEF::OBJLength };
        double radius{ DEF::OBJRadius };

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
        Color color{ DEF::OBJColor };
        double sideLength{ DEF::OBJLength };
        double radius{ DEF::OBJRadius };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radius") ss >> radius;
        }
        builder.pushSDFObj<SDFCylinderZ>(radius, sideLength, color);
    } },
    { "SDFChainLink", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double sideLength{ DEF::OBJLength };
        Vec2 radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFChainLink>(sideLength, radii, color);
    } },
    { "SDFTube", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double sideLength{ DEF::OBJLength };
        Vec2 radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "sideLength") ss >> sideLength;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTube>(sideLength, radii, color);
    } },
    { "SDFCone", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double height{ DEF::OBJLength };
        double ratio{ DEF::OBJRatio };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "height") ss >> height;
            else if (param == "ratio") ss >> ratio;
        }
        builder.pushSDFObj<SDFCone>(height, ratio, color);
    } },
    { "SDFSolidAngle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJRadius };
        double sweep{ DEF::OBJSweep };

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
        Color color{ DEF::OBJColor };
        Vec3 radii{ DEF::OBJRadiiV3 };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "radii") ss >> radii.x >> radii.y >> radii.z;
        }
        builder.pushSDFObj<SDFEllipsoid>(radii, color);
    } },
    { "SDFTaperedCapsule", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double height{ DEF::OBJLength };
        Vec2 radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "color") ss >> color.x >> color.y >> color.z;
            else if (param == "height") ss >> height;
            else if (param == "radii") ss >> radii.x >> radii.y;
        }
        builder.pushSDFObj<SDFTaperedCapsule>(radii, height, color);
    } },
    { "SDFRhombus", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        Vec2 dimensions{ DEF::OBJOP2DDimensions };
        double magnitude{ DEF::OBJLength };
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
        Color color{ DEF::OBJColor };
        double base{ DEF::OBJLength };
        double magnitude{ DEF::OBJOP2DMagnitude };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };


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
        Color color{ DEF::OBJColor };
        Vec2 dimensions{ DEF::OBJOP2DDimensions };
        double magnitude{ DEF::OBJOP2DMagnitude };
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
        builder.pushSDFObj<SDFIsoscelesTriangle>(operation, dimensions, magnitude, color);
    } },
    { "SDFPieSlice", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double sweep{ DEF::OBJLength };
        double radius{ DEF::OBJLength };
        double magnitude{ DEF::OBJLength };
        SDFObject::OP2D operation{ SDFObject::OP2D::Extrude };

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
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJLength };
        double magnitude{ DEF::OBJLength };
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
        builder.pushSDFObj<SDFPentagon>(operation, radius, magnitude, color);
    } },
    { "SDFHexagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJLength };
        double magnitude{ DEF::OBJLength };
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
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJLength };
        double magnitude{ DEF::OBJLength };
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
        Color color{ DEF::OBJColor };
        double radius{ DEF::OBJLength };
        double ratio{ DEF::OBJLength };
        double magnitude{ DEF::OBJLength };
        size_t numPoints{ DEF::OBJOP2DNumPoints };
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
        Color color{ DEF::OBJColor };
        double magnitude{ DEF::OBJLength };
        Vec2 point1{ DEF::OBJOP2DTPoint1 };
        Vec2 point2{ DEF::OBJOP2DTPoint2 };
        Vec2 point3{ DEF::OBJOP2DTPoint3 };
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
        Vec3 translation{ DEF::TRANSVector };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "translation") ss >> translation.x >> translation.y >> translation.z;
        }
        builder.pushOp<OPTranslate>(translation);
    } },
    { "OPTranslateSine", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ DEF::TRANSVector };
        double amplitude{ DEF::TRANSAmplitude };
        double period{ DEF::TRANSPeriod };
        double phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis.x >> axis.y >> axis.z;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "period") ss >> period;
            else if (param == "phase") ss >> phase;
        }
        builder.pushOp<OPTranslateSine>(axis, amplitude, period, phase);
    } },
    { "OPTranslateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 vector{ DEF::TRANSVector };
        Vec2 transitTimes{ DEF::TRANSTransTimes };
        Vec2 endDelays{ DEF::TRANSEndDelays };
        double phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "vector") ss >> vector.x >> vector.y >> vector.z;
            else if (param == "transitTimes") ss >> transitTimes.x >> transitTimes.y;
            else if (param == "endDelays") ss >> endDelays.x >> endDelays.y;
            else if (param == "phase") ss >> phase;
        }
        builder.pushOp<OPTranslateLinear>(vector, transitTimes, endDelays, phase);
    } },
    { "OPCubeRollHeight", [](SDFBuilder& builder, std::stringstream& ss) {
        double cubeHeight{ DEF::TRANSCubeHeight };
        double period{ DEF::TRANSPeriod };
        double phase{ DEF::TRANSPhase };

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
        Vec3 axis{ DEF::TRANSVector };
        double rotations{ DEF::TRANSRotations };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "axis") ss >> axis.x >> axis.y >> axis.z;
            else if (param == "rotations") ss >> rotations;
        }
        builder.pushOp<OPRotate>(axis, rotations);
    } },
    { "OPRotateContinuous", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 axis{ DEF::TRANSVector };
        double rotationsPerSecond{ DEF::TRANSRotsPerSec };
        double phase{ DEF::TRANSPhase };

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
        Vec3 axis{ DEF::TRANSVector };
        double rotations{ DEF::TRANSRotations };
        Vec2 transitTimes{ DEF::TRANSTransTimes };
        Vec2 endDelays{ DEF::TRANSEndDelays };
        double phase{ DEF::TRANSPhase };

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
        Vec3 axis{ DEF::TRANSVector };
        double rotations{ DEF::TRANSRotations };
        double rotateTime{ DEF::TRANSRotateTime };
        double delayTime{ DEF::TRANSDelayTime };
        double phase{ DEF::TRANSPhase };

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
        std::vector<Vec3> axes{ DEF::TRANSCubeRotAxes };
        double quarterTurnTime{ DEF::TRANSQuarterTurnTime };
        double phase{ DEF::TRANSPhase };

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
        double length{ DEF::TRANSElongateLength };
        double offset{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
        }
        builder.pushOp<OPElongateX>(length, offset);
    } },
    { "OPElongateY", [](SDFBuilder& builder, std::stringstream& ss) {
        double length{ DEF::TRANSElongateLength };
        double offset{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
        }
        builder.pushOp<OPElongateY>(length, offset);
    } },
    { "OPElongateZ", [](SDFBuilder& builder, std::stringstream& ss) {
        double length{ DEF::TRANSElongateLength };
        double offset{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "length") ss >> length;
            else if (param == "offset") ss >> offset;
        }
        builder.pushOp<OPElongateZ>(length, offset);
    } },
    { "OPElongateXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 lengths{ DEF::TRANSElongateLength };
        Vec3 offsets{ DEF::TRANSElongateOffset };

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
        double magnitude{ DEF::RESURFRoundMagnitude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "magnitude") ss >> magnitude;
        }
        builder.pushOp<OPRoundOff>(magnitude);
    } },
    { "OPOnion", [](SDFBuilder& builder, std::stringstream& ss) {
        double magnitude{ DEF::RESURFOnionMagnitude };

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
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatX>(spacing);
    } },
    { "OPRepeatY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatY>(spacing);
    } },
    { "OPRepeatZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatZ>(spacing);
    } },
    { "OPRepeatXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatXY>(spacing);
    } },
    { "OPRepeatXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatXZ>(spacing);
    } },
    { "OPRepeatYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatYZ>(spacing);
    } },
    { "OPRepeatXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "spacing") ss >> spacing.x >> spacing.y >> spacing.z;
        }
        builder.pushOp<OPRepeatXYZ>(spacing);
    } },
    { "OPRepeatLimited", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 spacing{ DEF::DOMSpacing };
        Vec3 limits{ DEF::DOMLimits };

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
        double period{ DEF::PMPeriod };
        double amplitude{ DEF::PMAmplitude };
        double phase{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.bindPropMod<PMOscillateSine>(propName, period, amplitude, phase);
    } },
    { "PMOscillateSineV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ DEF::PMPeriod };
        Vec3 amplitudes{ DEF::PMAmplitude };
        Vec3 phases{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> periods.x >> periods.y >> periods.z;
            else if (param == "amplitude") ss >> amplitudes.x >> amplitudes.y >> amplitudes.z;
            else if (param == "phase") ss >> phases.x >> phases.y >> phases.z;
        }
        builder.bindPropMod<PMOscillateSineV3>(propName, periods, amplitudes, phases);
    } },
    { "PMOscillateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        double period{ DEF::PMPeriod };
        double amplitude{ DEF::PMAmplitude };
        double phase{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> period;
            else if (param == "amplitude") ss >> amplitude;
            else if (param == "phase") ss >> phase;
        }
        builder.bindPropMod<PMOscillateLinear>(propName, period, amplitude, phase);
    } },
    { "PMOscillateLinearV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string propName;
        Vec3 periods{ DEF::PMPeriod };
        Vec3 amplitudes{ DEF::PMAmplitude };
        Vec3 phases{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "propName") ss >> propName;
            else if (param == "period") ss >> periods.x >> periods.y >> periods.z;
            else if (param == "amplitude") ss >> amplitudes.x >> amplitudes.y >> amplitudes.z;
            else if (param == "phase") ss >> phases.x >> phases.y >> phases.z;
        }
        builder.bindPropMod<PMOscillateLinearV3>(propName, periods, amplitudes, phases);
    } },
    { "LightSource", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 position{ DEF::LSPosition };
        double luminance{ DEF::LSLuminance };
        double ambientPower{ DEF::LSAmbientPower };
        Color color;

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
