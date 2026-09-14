#pragma once
#include "SDFBuilder.h"
#include "SDFObject.h"
#include "SDFScene.h"
#include "SDFOperationInc.h"

SDFBuilder::SDFBuilder() {
    m_baseObject = std::make_unique<SDFObject>();
    m_objChain.push_back(m_baseObject.get());
}

std::unique_ptr<SDFObject> SDFBuilder::get() {
    return std::move(m_baseObject);
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

SDFBuilder& SDFBuilder::position(Vec3 position)
{
    m_objChain.back()->setPosition(position);
    return *this;
}

SDFBuilder& SDFBuilder::rotation(Vec3 axis, double rotations)
{
    m_objChain.back()->setRotation(axis, rotations);
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
    for (size_t i{}; i < strVec.size(); ++i) {
        std::replace(strVec[i].begin(), strVec[i].end(), ',', ' ');
        std::stringstream ss{ strVec[i] };
        std::string opName;
        ss >> opName;

        if (s_operationsMap.contains(opName)) {
            if (opName.find("COMB") == 0) {
                ss << strVec[++i];
            }
            auto func = s_operationsMap.at(opName);
            func(*this, ss);
        }
    }
}

const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> SDFBuilder::s_operationsMap = {
    { "COMBAdd", [](SDFBuilder& builder, std::stringstream& ss) { 
        //builder.pushOp<COMBAdd>();
        //CombinationOperation* comboOpPtr = dynamic_cast<CombinationOperation*>(builder.m_lastOp);
        //std::string objName{};
        //auto objFunc = s_objectsMap.at(objName);
        //objFunc(builder, ss);
    }},
    { "COMBSubtract", [](SDFBuilder& builder, std::stringstream& ss) {
    
    }},
    { "COMBIntersect", [](SDFBuilder& builder, std::stringstream& ss) {
    
    }},
    { "COMBAddSmooth", [](SDFBuilder& builder, std::stringstream& ss) {

    }},
    { "COMBSubtractSmooth", [](SDFBuilder& builder, std::stringstream& ss) {

    }},
    { "COMBIntersectSmooth", [](SDFBuilder& builder, std::stringstream& ss) {

    }}
};

const std::unordered_map<std::string, std::function<void(SDFBuilder& builder, std::stringstream&)>> SDFBuilder::s_objectsMap = {

};