#include "PropertyModifier.h"

void PropertyModifierBase::serialize(std::vector<std::string> strVec, size_t level)
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << ",m_propName," << m_propName;
    serializeInner(ss);
    strVec.push_back(ss.str());
}

void PMOscillateSine::serializeInner(std::stringstream& ss) const
{
    ss << ",m_period," << m_period;
    ss << ",m_amplitude," << m_amplitude;
    ss << ",m_phase," << m_phase;
}

void PMOscillateSineV3::serializeInner(std::stringstream& ss) const
{
    ss << ",m_period," << m_period.x << ',' << m_period.y << ',' << m_period.z;
    ss << ",m_amplitude," << m_amplitude.x << ',' << m_amplitude.y << ',' << m_amplitude.z;
    ss << ",m_phase," << m_phase.x << ',' << m_phase.y << ',' << m_phase.z;
}

void PMOscillateLinear::serializeInner(std::stringstream& ss) const
{
    ss << ",m_period," << m_period;
    ss << ",m_amplitude," << m_amplitude;
    ss << ",m_phase," << m_phase;
}

void PMOscillateLinearV3::serializeInner(std::stringstream& ss) const
{
    ss << ",m_period," << m_period.x << ',' << m_period.y << ',' << m_period.z;
    ss << ",m_amplitude," << m_amplitude.x << ',' << m_amplitude.y << ',' << m_amplitude.z;
    ss << ",m_phase," << m_phase.x << ',' << m_phase.y << ',' << m_phase.z;
}
