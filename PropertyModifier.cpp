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

void PMOscillateSine::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period;
    ss << " amplitude " << m_amplitude;
    ss << " phase " << m_phase;
}

void PMOscillateSine::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSine({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateSineV3::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period.x << ' ' << m_period.y << ' ' << m_period.z;
    ss << " amplitude " << m_amplitude.x << ' ' << m_amplitude.y << ' ' << m_amplitude.z;
    ss << " phase " << m_phase.x << ' ' << m_phase.y << ' ' << m_phase.z;
}

void PMOscillateSineV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSineV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateLinear::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period;
    ss << " amplitude " << m_amplitude;
    ss << " phase " << m_phase;
}

void PMOscillateLinear::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinear({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateLinearV3::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period.x << ' ' << m_period.y << ' ' << m_period.z;
    ss << " amplitude " << m_amplitude.x << ' ' << m_amplitude.y << ' ' << m_amplitude.z;
    ss << " phase " << m_phase.x << ' ' << m_phase.y << ' ' << m_phase.z;
}

void PMOscillateLinearV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateLinearV2::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period.x << ' ' << m_period.y;
    ss << " amplitude " << m_amplitude.x << ' ' << m_amplitude.y;
    ss << " phase " << m_phase.x << ' ' << m_phase.y;
}

void PMOscillateLinearV2::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateSineV2::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period.x << ' ' << m_period.y;
    ss << " amplitude " << m_amplitude.x << ' ' << m_amplitude.y;
    ss << " phase " << m_phase.x << ' ' << m_phase.y;
}

void PMOscillateSineV2::getShaderModCall(std::string & call) const
{
    call = std::format("oscillateSineV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void PMOscillateSineST::getShaderModCall(std::string& call) const
{
    call = std::format("round(oscillateSine({}, {:.7f}, {:.7f}, {:.7f}, time))", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateSineST::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period;
    ss << " amplitude " << m_amplitude;
    ss << " phase " << m_phase;
}

void PMOscillateLinearST::getShaderModCall(std::string& call) const
{
    call = std::format("round(oscillateLinear({}, {:.7f}, {:.7f}, {:.7f}, time))", m_initVal, m_period, m_amplitude, m_phase);
}

void PMOscillateLinearST::serializeInner(std::stringstream& ss) const
{
    ss << " period " << m_period;
    ss << " amplitude " << m_amplitude;
    ss << " phase " << m_phase;
}
