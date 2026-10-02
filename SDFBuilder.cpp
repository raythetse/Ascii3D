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
    { "COMBAddSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_smoothing{ DEF::COMBSmooth };
        double m_colorBlending{ DEF::COMBColBlend };
        double m_colorShifting{ DEF::COMBColShift };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_smoothing") ss >> m_smoothing;
            else if (param == "m_colorBlending") ss >> m_colorBlending;
            else if (param == "m_colorShifting") ss >> m_colorShifting;
        }
        builder.pushOp<COMBAddSmooth>(nullptr, m_smoothing, m_colorBlending, m_colorShifting);
    }},
    { "COMBSubtractSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_smoothing{ DEF::COMBSmooth };
        double m_colorBlending{ DEF::COMBColBlend };
        double m_colorShifting{ DEF::COMBColShift };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_smoothing") ss >> m_smoothing;
            else if (param == "m_colorBlending") ss >> m_colorBlending;
            else if (param == "m_colorShifting") ss >> m_colorShifting;
        }
        builder.pushOp<COMBSubtractSmooth>(nullptr, m_smoothing, m_colorBlending, m_colorShifting);
    }},
    { "COMBIntersectSmooth", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_smoothing{ DEF::COMBSmooth };
        double m_colorBlending{ DEF::COMBColBlend };
        double m_colorShifting{ DEF::COMBColShift };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_smoothing") ss >> m_smoothing;
            else if (param == "m_colorBlending") ss >> m_colorBlending;
            else if (param == "m_colorShifting") ss >> m_colorShifting;
        }
        builder.pushOp<COMBIntersectSmooth>(nullptr, m_smoothing, m_colorBlending, m_colorShifting);
    }},

    // Objects
    { "SDFObject", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
        }
        builder.pushSDFObj<SDFObject>(m_color);
    }},
    { "SDFPlaneXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_yPos{ DEF::OBJPlanePos };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_yPos") ss >> m_yPos;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
        }
        builder.pushSDFObj<SDFPlaneXZ>(m_yPos, m_color);
    }},
    { "SDFPlaneXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_zPos{ DEF::OBJPlanePos };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_zPos") ss >> m_zPos;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
        }
        builder.pushSDFObj<SDFPlaneXY>(m_zPos, m_color);
    }},
    { "SDFPlaneYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_xPos{ DEF::OBJPlanePos };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_xPos") ss >> m_xPos;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
        }
        builder.pushSDFObj<SDFPlaneYZ>(m_xPos, m_color);
    }},
    { "SDFPlane", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec3 m_normal{ DEF::OBJNormal };
        double m_offset{ DEF::OBJOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_normal") ss >> m_normal.x >> m_normal.y >> m_normal.z;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_offset") ss >> m_offset;
        }
        builder.pushSDFObj<SDFPlane>(m_normal, m_offset, m_color);
    } },
    { "SDFBox", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec3 m_dimensions{ DEF::OBJBoxDimensions };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_dimensions") ss >> m_dimensions.x >> m_dimensions.y >> m_dimensions.z;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
        }
        builder.pushSDFObj<SDFBox>(m_dimensions, m_color);
    } },
    { "SDFBoxFrame", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_edgeThickness{ DEF::OBJFrameThickness };
        Vec3 m_dimensions{ DEF::OBJBoxDimensions };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_dimensions") ss >> m_dimensions.x >> m_dimensions.y >> m_dimensions.z;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_edgeThickness") ss >> m_edgeThickness;
        }
        builder.pushSDFObj<SDFBoxFrame>(m_dimensions, m_edgeThickness, m_color);
    }},
    { "SDFSphere", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJRadius };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
        }
        builder.pushSDFObj<SDFSphere>(m_radius, m_color);
    } },
    { "SDFTorus", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec2 m_radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radii") ss >> m_radii.x >> m_radii.y;
        }
        builder.pushSDFObj<SDFTorus>(m_radii, m_color);
    } },
    { "SDFTorusSegment", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec2 m_radii{ DEF::OBJRadii };
        double m_sweep{ DEF::OBJSweep };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radii") ss >> m_radii.x >> m_radii.y;
            else if (param == "m_sweep") ss >> m_sweep;
        }
        builder.pushSDFObj<SDFTorusSegment>(m_radii, m_sweep, m_color);
    } },
    { "SDFCapsuleX", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJRadius };
        double m_bodyLength{ DEF::OBJLength };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_bodyLength") ss >> m_bodyLength;
        }
        builder.pushSDFObj<SDFCapsuleX>(m_radius, m_bodyLength, m_color);
    } },
    { "SDFCapsuleY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJRadius };
        double m_bodyLength{ DEF::OBJLength };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_bodyLength") ss >> m_bodyLength;
        }
        builder.pushSDFObj<SDFCapsuleY>(m_radius, m_bodyLength, m_color);
    } },
    { "SDFCapsuleZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJRadius };
        double m_bodyLength{ DEF::OBJLength };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_bodyLength") ss >> m_bodyLength;
        }
        builder.pushSDFObj<SDFCapsuleZ>(m_radius, m_bodyLength, m_color);
    } },
    { "SDFCrystal", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_edgeLength{ DEF::OBJLength };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_edgeLength") ss >> m_edgeLength;
        }
        builder.pushSDFObj<SDFCrystal>(m_edgeLength, m_color);
    } },
    { "SDFCylinderX", [](SDFBuilder& builder, std::stringstream& ss) {
        Color col{ DEF::OBJColor };
        double m_sideLength{ DEF::OBJLength };
        double m_radius{ DEF::OBJRadius };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> col.x >> col.y >> col.z;
            else if (param == "m_sideLength") ss >> m_sideLength;
            else if (param == "m_radius") ss >> m_radius;
        }
        builder.pushSDFObj<SDFCylinderX>(m_radius, m_sideLength, col);
    } },
    { "SDFCylinderY", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_sideLength{ DEF::OBJLength };
        double m_radius{ DEF::OBJRadius };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_sideLength") ss >> m_sideLength;
            else if (param == "m_radius") ss >> m_radius;
        }
        builder.pushSDFObj<SDFCylinderY>(m_radius, m_sideLength, m_color);
    } },
    { "SDFCylinderZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_sideLength{ DEF::OBJLength };
        double m_radius{ DEF::OBJRadius };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_sideLength") ss >> m_sideLength;
            else if (param == "m_radius") ss >> m_radius;
        }
        builder.pushSDFObj<SDFCylinderZ>(m_radius, m_sideLength, m_color);
    } },
    { "SDFChainLink", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_sideLength{ DEF::OBJLength };
        Vec2 m_radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_sideLength") ss >> m_sideLength;
            else if (param == "m_radii") ss >> m_radii.x >> m_radii.y;
        }
        builder.pushSDFObj<SDFChainLink>(m_sideLength, m_radii, m_color);
    } },
    { "SDFTube", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_sideLength{ DEF::OBJLength };
        Vec2 m_radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_sideLength") ss >> m_sideLength;
            else if (param == "m_radii") ss >> m_radii.x >> m_radii.y;
        }
        builder.pushSDFObj<SDFTube>(m_sideLength, m_radii, m_color);
    } },
    { "SDFCone", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_height{ DEF::OBJLength };
        double m_ratio{ DEF::OBJRatio };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_height") ss >> m_height;
            else if (param == "m_ratio") ss >> m_ratio;
        }
        builder.pushSDFObj<SDFCone>(m_height, m_ratio, m_color);
    } },
    { "SDFSolidAngle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJRadius };
        double m_sweep{ DEF::OBJSweep };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_sweep") ss >> m_sweep;
        }
        builder.pushSDFObj<SDFSolidAngle>(m_radius, m_sweep, m_color);
    } },
    { "SDFEllipsoid", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec3 m_radii{ DEF::OBJRadiiV3 };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radii") ss >> m_radii.x >> m_radii.y >> m_radii.z;
        }
        builder.pushSDFObj<SDFEllipsoid>(m_radii, m_color);
    } },
    { "SDFTaperedCapsule", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_height{ DEF::OBJLength };
        Vec2 m_radii{ DEF::OBJRadii };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_height") ss >> m_height;
            else if (param == "m_radii") ss >> m_radii.x >> m_radii.y;
        }
        builder.pushSDFObj<SDFTaperedCapsule>(m_radii, m_height, m_color);
    } },
    { "SDFRhombus", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec2 m_dimensions{ DEF::OBJOP2DDimensions };
        double m_magnitude{ DEF::OBJLength };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_dimensions") ss >> m_dimensions.x >> m_dimensions.y;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFRhombus>(m_operation, m_dimensions, m_magnitude, m_color);
    } },
    { "SDFEquilateralTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_base{ DEF::OBJLength };
        double m_magnitude{ DEF::OBJOP2DMagnitude };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };


        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_base") ss >> m_base;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFEquilateralTriangle>(m_operation, m_base, m_magnitude, m_color);
    } },
    { "SDFIsoscelesTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        Vec2 m_dimensions{ DEF::OBJOP2DDimensions };
        double m_magnitude{ DEF::OBJOP2DMagnitude };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };


        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_dimensions") ss >> m_dimensions.x >> m_dimensions.y;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFIsoscelesTriangle>(m_operation, m_dimensions, m_magnitude, m_color);
    } },
    { "SDFPieSlice", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_sweep{ DEF::OBJLength };
        double m_radius{ DEF::OBJLength };
        double m_magnitude{ DEF::OBJLength };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_sweep") ss >> m_sweep;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFPieSlice>(m_operation, m_sweep, m_radius, m_magnitude, m_color);
    } },
    { "SDFPentagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJLength };
        double m_magnitude{ DEF::OBJLength };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFPentagon>(m_operation, m_radius, m_magnitude, m_color);
    } },
    { "SDFHexagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJLength };
        double m_magnitude{ DEF::OBJLength };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFHexagon>(m_operation, m_radius, m_magnitude, m_color);
    } },
    { "SDFOctagon", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJLength };
        double m_magnitude{ DEF::OBJLength };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFOctagon>(m_operation, m_radius, m_magnitude, m_color);
    } },
    { "SDFStar", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_radius{ DEF::OBJLength };
        double m_ratio{ DEF::OBJLength };
        double m_magnitude{ DEF::OBJLength };
        size_t m_numPoints{ DEF::OBJOP2DNumPoints };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_radius") ss >> m_radius;
            else if (param == "m_numPoints") ss >> m_numPoints;
            else if (param == "m_ratio") ss >> m_ratio;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFStar>(m_operation, m_numPoints, m_radius, m_ratio, m_magnitude, m_color);
    } },
    { "SDFTriangle", [](SDFBuilder& builder, std::stringstream& ss) {
        Color m_color{ DEF::OBJColor };
        double m_magnitude{ DEF::OBJLength };
        Vec2 m_point1{ DEF::OBJOP2DTPoint1 };
        Vec2 m_point2{ DEF::OBJOP2DTPoint2 };
        Vec2 m_point3{ DEF::OBJOP2DTPoint3 };
        SDFObject::OP2D m_operation{ SDFObject::OP2D::Extrude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
            else if (param == "m_point1") ss >> m_point1.x >> m_point1.y;
            else if (param == "m_point2") ss >> m_point2.x >> m_point2.y;
            else if (param == "m_point3") ss >> m_point3.x >> m_point3.y;
            else if (param == "m_magnitude") ss >> m_magnitude;
            else if (param == "m_operation") {
                int opID;
                ss >> opID;
                m_operation = static_cast<SDFObject::OP2D>(opID);
            }
        }
        builder.pushSDFObj<SDFTriangle>(m_operation, m_point1, m_point2, m_point3, m_magnitude, m_color);
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
        Vec3 m_translation{ DEF::TRANSVector };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_translation") ss >> m_translation.x >> m_translation.y >> m_translation.z;
        }
        builder.pushOp<OPTranslate>(m_translation);
    } },
    { "OPTranslateSine", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_axis{ DEF::TRANSVector };
        double m_amplitude{ DEF::TRANSAmplitude };
        double m_period{ DEF::TRANSPeriod };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_axis") ss >> m_axis.x >> m_axis.y >> m_axis.z;
            else if (param == "m_amplitude") ss >> m_amplitude;
            else if (param == "m_period") ss >> m_period;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPTranslateSine>(m_axis, m_amplitude, m_period, m_phase);
    } },
    { "OPTranslateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_vector{ DEF::TRANSVector };
        Vec2 m_transitTimes{ DEF::TRANSTransTimes };
        Vec2 m_endDelays{ DEF::TRANSEndDelays };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_vector") ss >> m_vector.x >> m_vector.y >> m_vector.z;
            else if (param == "m_transitTimes") ss >> m_transitTimes.x >> m_transitTimes.y;
            else if (param == "m_endDelays") ss >> m_endDelays.x >> m_endDelays.y;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPTranslateLinear>(m_vector, m_transitTimes, m_endDelays, m_phase);
    } },
    { "OPCubeRollHeight", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_cubeHeight{ DEF::TRANSCubeHeight };
        double m_period{ DEF::TRANSPeriod };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_cubeHeight") ss >> m_cubeHeight;
            else if (param == "m_period") ss >> m_period;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPCubeRollHeight>(m_cubeHeight, m_period, m_phase);
    } },
    { "OPRotate", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_axis{ DEF::TRANSVector };
        double m_rotations{ DEF::TRANSRotations };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_axis") ss >> m_axis.x >> m_axis.y >> m_axis.z;
            else if (param == "m_rotations") ss >> m_rotations;
        }
        builder.pushOp<OPRotate>(m_axis, m_rotations);
    } },
    { "OPRotateContinuous", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_axis{ DEF::TRANSVector };
        double m_rotationsPerSecond{ DEF::TRANSRotsPerSec };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_axis") ss >> m_axis.x >> m_axis.y >> m_axis.z;
            else if (param == "m_rotationsPerSecond") ss >> m_rotationsPerSecond;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPRotateContinuous>(m_axis, m_rotationsPerSecond, m_phase);
    } },
    { "OPRotateScan", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_axis{ DEF::TRANSVector };
        double m_rotations{ DEF::TRANSRotations };
        Vec2 m_transitTimes{ DEF::TRANSTransTimes };
        Vec2 m_endDelays{ DEF::TRANSEndDelays };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_axis") ss >> m_axis.x >> m_axis.y >> m_axis.z;
            else if (param == "m_rotations") ss >> m_rotations;
            else if (param == "m_transitTimes") ss >> m_transitTimes.x >> m_transitTimes.y;
            else if (param == "m_endDelays") ss >> m_endDelays.x >> m_endDelays.y;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPRotateScan>(m_axis, m_rotations, m_transitTimes, m_endDelays, m_phase);
    } },
    { "OPRotateDelay", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_axis{ DEF::TRANSVector };
        double m_rotations{ DEF::TRANSRotations };
        double m_rotateTime{ DEF::TRANSRotateTime };
        double m_delayTime{ DEF::TRANSDelayTime };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_axis") ss >> m_axis.x >> m_axis.y >> m_axis.z;
            else if (param == "m_rotations") ss >> m_rotations;
            else if (param == "m_rotateTime") ss >> m_rotateTime;
            else if (param == "m_delayTime") ss >> m_delayTime;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPRotateDelay>(m_axis, m_rotations, m_rotateTime, m_delayTime, m_phase);
    } },
    { "OPCubeRoller", [](SDFBuilder& builder, std::stringstream& ss) {
        std::vector<Vec3> m_axes{ DEF::TRANSCubeRotAxes };
        double m_quarterTurnTime{ DEF::TRANSQuarterTurnTime };
        double m_phase{ DEF::TRANSPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_axes") {
                m_axes.clear();
                int numAxes{};
                ss >> numAxes;
                for (size_t i{}; i < numAxes; ++i) {
                    Vec3 axis{};
                    ss >> axis.x >> axis.y >> axis.z;
                    m_axes.push_back(axis);
                }
            }
            else if (param == "m_quarterTurnTime") ss >> m_quarterTurnTime;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.pushOp<OPCubeRoller>(m_axes, m_quarterTurnTime, m_phase);
    } },
    { "OPElongateX", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_length{ DEF::TRANSElongateLength };
        double m_offset{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_length") ss >> m_length;
            else if (param == "m_offset") ss >> m_offset;
        }
        builder.pushOp<OPElongateX>(m_length, m_offset);
    } },
    { "OPElongateY", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_length{ DEF::TRANSElongateLength };
        double m_offset{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_length") ss >> m_length;
            else if (param == "m_offset") ss >> m_offset;
        }
        builder.pushOp<OPElongateY>(m_length, m_offset);
    } },
    { "OPElongateZ", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_length{ DEF::TRANSElongateLength };
        double m_offset{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_length") ss >> m_length;
            else if (param == "m_offset") ss >> m_offset;
        }
        builder.pushOp<OPElongateZ>(m_length, m_offset);
    } },
    { "OPElongateXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_lengths{ DEF::TRANSElongateLength };
        Vec3 m_offsets{ DEF::TRANSElongateOffset };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_lengths") ss >> m_lengths.x >> m_lengths.y >> m_lengths.z;
            else if (param == "m_offsets") ss >> m_offsets.x >> m_offsets.y >> m_offsets.z;
        }
        builder.pushOp<OPElongateXYZ>(m_lengths, m_offsets);
    } },

    // Resurface Ops
    { "OPRoundOff", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_magnitude{ DEF::RESURFRoundMagnitude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_magnitude") ss >> m_magnitude;
        }
        builder.pushOp<OPRoundOff>(m_magnitude);
    } },
    { "OPOnion", [](SDFBuilder& builder, std::stringstream& ss) {
        double m_magnitude{ DEF::RESURFOnionMagnitude };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_magnitude") ss >> m_magnitude;
        }
        builder.pushOp<OPOnion>(m_magnitude);
    } },
    { "OPInsideOut", [](SDFBuilder& builder, std::stringstream& ss) {
        builder.pushOp<OPInsideOut>();
    } },

    // Domain Ops
    { "OPRepeatX", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatX>(m_spacing);
    } },
    { "OPRepeatY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatY>(m_spacing);
    } },
    { "OPRepeatZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatZ>(m_spacing);
    } },
    { "OPRepeatXY", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatXY>(m_spacing);
    } },
    { "OPRepeatXZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatXZ>(m_spacing);
    } },
    { "OPRepeatYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatYZ>(m_spacing);
    } },
    { "OPRepeatXYZ", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
        }
        builder.pushOp<OPRepeatXYZ>(m_spacing);
    } },
    { "OPRepeatLimited", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_spacing{ DEF::DOMSpacing };
        Vec3 m_limits{ DEF::DOMLimits };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_spacing") ss >> m_spacing.x >> m_spacing.y >> m_spacing.z;
            else if (param == "m_limits") ss >> m_limits.x >> m_limits.y >> m_limits.z;
        }
        builder.pushOp<OPRepeatLimited>(m_limits, m_spacing);
    } },

    // Property Modifiers
    { "PMOscillateSine", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string m_propName;
        double m_period{ DEF::PMPeriod };
        double m_amplitude{ DEF::PMAmplitude };
        double m_phase{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_propName") ss >> m_propName;
            else if (param == "m_period") ss >> m_period;
            else if (param == "m_amplitude") ss >> m_amplitude;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.bindPropMod<PMOscillateSine>(m_propName, m_period, m_amplitude, m_phase);
    } },
    { "PMOscillateSineV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string m_propName;
        Vec3 m_periods{ DEF::PMPeriod };
        Vec3 m_amplitudes{ DEF::PMAmplitude };
        Vec3 m_phases{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_propName") ss >> m_propName;
            else if (param == "m_period") ss >> m_periods.x >> m_periods.y >> m_periods.z;
            else if (param == "m_amplitude") ss >> m_amplitudes.x >> m_amplitudes.y >> m_amplitudes.z;
            else if (param == "m_phase") ss >> m_phases.x >> m_phases.y >> m_phases.z;
        }
        builder.bindPropMod<PMOscillateSineV3>(m_propName, m_periods, m_amplitudes, m_phases);
    } },
    { "PMOscillateLinear", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string m_propName;
        double m_period{ DEF::PMPeriod };
        double m_amplitude{ DEF::PMAmplitude };
        double m_phase{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_propName") ss >> m_propName;
            else if (param == "m_period") ss >> m_period;
            else if (param == "m_amplitude") ss >> m_amplitude;
            else if (param == "m_phase") ss >> m_phase;
        }
        builder.bindPropMod<PMOscillateLinear>(m_propName, m_period, m_amplitude, m_phase);
    } },
    { "PMOscillateLinearV3", [](SDFBuilder& builder, std::stringstream& ss) {
        std::string m_propName;
        Vec3 m_periods{ DEF::PMPeriod };
        Vec3 m_amplitudes{ DEF::PMAmplitude };
        Vec3 m_phases{ DEF::PMPhase };

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_propName") ss >> m_propName;
            else if (param == "m_period") ss >> m_periods.x >> m_periods.y >> m_periods.z;
            else if (param == "m_amplitude") ss >> m_amplitudes.x >> m_amplitudes.y >> m_amplitudes.z;
            else if (param == "m_phase") ss >> m_phases.x >> m_phases.y >> m_phases.z;
        }
        builder.bindPropMod<PMOscillateLinearV3>(m_propName, m_periods, m_amplitudes, m_phases);
    } },
    { "LightSource", [](SDFBuilder& builder, std::stringstream& ss) {
        Vec3 m_position{ DEF::LSPosition };
        double m_luminance{ DEF::LSLuminance };
        double m_ambientPower{ DEF::LSAmbientPower };
        Color m_color;

        while (ss.peek() != EOF) {
            std::string param;
            ss >> param;
            if (param == "m_position") ss >> m_position.x >> m_position.y >> m_position.z;
            else if (param == "m_luminance") ss >> m_luminance;
            else if (param == "m_ambientPower") ss >> m_ambientPower;
            else if (param == "m_color") ss >> m_color.x >> m_color.y >> m_color.z;
        }
        builder.addLightSource(m_position, m_luminance, m_color, m_ambientPower);
    } },

};
