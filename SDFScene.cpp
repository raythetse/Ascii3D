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

    SDFBuilder builder{};
    builder.loadSerialized(strVec);

    add(builder);
}

void SDFScene::add(SDFBuilder& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
}

void SDFScene::add(SDFBuilder&& builder)
{
    m_baseObject = std::move(builder.getBaseObject());
    m_lights = std::move(builder.getLights());
}

void SDFScene::update()
{
    m_baseObject->update();
}

bool SDFScene::serialize(std::vector<std::string>& strVec) const
{
    m_baseObject->resetMods();
    m_baseObject->serialize(strVec, 0);
    for (auto& light : m_lights) {
        light->serialize(strVec);
    }
    strVec.push_back("end");
    return false;
}
