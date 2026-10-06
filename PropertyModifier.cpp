#include "PropertyModifier.h"
#include <format>

void PropertyModifierBase::serialize(std::vector<std::string>& strVec, size_t level, bool boundOnObj)
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " boundOnObj " << boundOnObj;
    ss << " propName " << m_propName;
    serializeInner(ss);
    strVec.push_back(ss.str());
}

bool PropertyModifierBase::getShaderCall(std::string& propName) const
{
    if (m_propName == propName) {
        getShaderModCall(propName);
        return true;
    }
    return false;
}

void PMOscillateSine::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSine({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateSineV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSineV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateLinear::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinear({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateLinearV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateLinearV2::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateSineV2::getShaderModCall(std::string & call) const
{
    call = std::format("oscillateSineV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateSineST::getShaderModCall(std::string& call) const
{
    call = std::format("round(oscillateSine({}, {:.7f}, {:.7f}, {:.7f}, time))", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateLinearST::getShaderModCall(std::string& call) const
{
    call = std::format("round(oscillateLinear({}, {:.7f}, {:.7f}, {:.7f}, time))", m_initVal, m_period, m_amplitude, m_phase);
}
