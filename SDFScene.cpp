#include "SDFScene.h"
#include "FileHandler.h"
#include "GLSLMarcher.h"

std::string SDFScene::buildComputeShader() const
{
    std::stringstream ssSample;
    std::stringstream ssLight;
    GLSLMarcher::buildSampleFunction(ssSample, m_baseObject);
    GLSLMarcher::buildLightPasses(ssLight, m_lights);
    return GLSLMarcher::s_shaderLayout + GLSLMarcher::s_shaderFunctions + ssSample.str() + GLSLMarcher::s_shaderMainFunction + ssLight.str() + GLSLMarcher::s_shaderMainEnd + "\t}\n}\n";
}

SDFScene::SDFScene(std::string fileName)
{
    std::vector<std::string> strVec;
    FileHandler::readSDFScene(strVec, fileName);
    add(SDFBuilder{}.loadSerialized(strVec));
}

void SDFScene::add(SDFBuilder& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
    m_branchMap = std::move(builder.getBranchMap());
}

void SDFScene::add(SDFBuilder&& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
    m_branchMap = std::move(builder.getBranchMap());
}

void SDFScene::update()
{
    m_baseObject->update();
}

bool SDFScene::serialize(std::vector<std::string>& strVec) const
{
    for (auto& branch : *m_branchMap) {
        strVec.push_back("startBranch " + branch.first);
        strVec.insert(strVec.end(), branch.second.begin(), branch.second.end());
        strVec.push_back("endBranch");
    }
    m_baseObject->resetAnimations();
    m_baseObject->serialize(strVec, 0);
    for (auto& light : m_lights) {
        light->serialize(strVec);
    }
    strVec.push_back("end");
    return false;
}
