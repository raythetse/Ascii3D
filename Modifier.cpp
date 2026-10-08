#include "Modifier.h"
#include <format>

void Modifier::serialize(std::vector<std::string>& strVec, size_t level, bool boundOnObj)
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " boundOnObj " << boundOnObj;
    ss << " propName " << m_propName;
    serializeInner(ss);
    strVec.push_back(ss.str());
}

bool Modifier::getShaderCall(std::string& propName) const
{
    if (m_propName == propName) {
        getShaderModCall(propName);
        return true;
    }
    return false;
}

void MODWaveSmooth::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSine({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void MODWaveSmoothV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSineV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveLinear::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinear({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void MODWaveLinearV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveLinearV2::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveSmoothV2::getShaderModCall(std::string & call) const
{
    call = std::format("oscillateSineV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveSmoothST::getShaderModCall(std::string& call) const
{
    call = std::format("round(oscillateSine({}, {:.7f}, {:.7f}, {:.7f}, time))", m_initVal, m_period, m_amplitude, m_phase);
}

void MODWaveLinearST::getShaderModCall(std::string& call) const
{
    call = std::format("round(oscillateLinear({}, {:.7f}, {:.7f}, {:.7f}, time))", m_initVal, m_period, m_amplitude, m_phase);
}
