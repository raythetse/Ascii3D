#include "SDFScene.h"
#include "FileHandler.h"
#include "GLSLMarcher.h"
#include "SceneValidator.h"

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

    // The validator explains problems against the format spec; keep the ones where the
    // loader loads something other than what the line says (style rules are left out)
    std::string text;
    for (const auto& line : strVec) text += line + '\n';
    for (const auto& finding : SceneValidator::validate(text)) {
        if (SceneValidator::affectsLoading(finding)) {
            m_loadWarnings.push_back("Line " + std::to_string(finding.line) + ": [rule "
                + std::to_string(finding.rule) + "] " + finding.message);
        }
    }

    SDFBuilder builder;
    builder.loadSerialized(strVec);
    // The loader's own warnings are the fallback, in case it catches something the validator misses
    if (m_loadWarnings.empty()) m_loadWarnings = builder.getWarnings();
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
    m_baseObject->resetAnimations();
    m_baseObject->serialize(strVec, 0);
    for (auto& light : m_lights) {
        light->serialize(strVec);
    }
    strVec.push_back("end");
    return false;
}
