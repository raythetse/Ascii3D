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

void MODWaveSmooth::update() const
{
    *m_ptr = m_initVal + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
}

void MODWaveSmoothV2::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSineV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveSmoothV2::update() const
{
    *m_ptr = m_initVal + ((Vec2{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void MODWaveSmoothV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateSineV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveSmoothV3::update() const
{
    *m_ptr = m_initVal + ((Vec3{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void MODWaveLinear::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinear({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void MODWaveLinear::update() const
{
    double progress = fmod(0.25 * m_period + (GameTimer::gameTime() + (m_phase * m_period)), m_period) / m_period;
    progress -= 0.5;
    progress *= 2.0;
    progress = abs(progress);
    progress -= 0.5;
    progress *= 2.0;

    *m_ptr = m_initVal + progress * m_amplitude;
}

void MODWaveLinearV2::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveLinearV2::update() const
{
    double gameTime = GameTimer::gameTime();
    Vec2 progress{
        fmod(0.25 * m_period.x + (gameTime + (m_phase.x * m_period.x)), m_period.x) / m_period.x,
        fmod(0.25 * m_period.y + (gameTime + (m_phase.y * m_period.y)), m_period.y) / m_period.y
    };
    progress -= 0.5;
    progress *= 2.0;
    progress = progress.abs();
    progress -= 0.5;
    progress *= 2.0;

    *m_ptr = m_initVal + progress * m_amplitude;
}

void MODWaveLinearV3::getShaderModCall(std::string& call) const
{
    call = std::format("oscillateLinearV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void MODWaveLinearV3::update() const
{
    double gameTime = GameTimer::gameTime();
    Vec3 progress{
        fmod(0.25 * m_period.x + (gameTime + (m_phase.x * m_period.x)), m_period.x) / m_period.x,
        fmod(0.25 * m_period.y + (gameTime + (m_phase.y * m_period.y)), m_period.y) / m_period.y,
        fmod(0.25 * m_period.z + (gameTime + (m_phase.z * m_period.z)), m_period.z) / m_period.z
    };
    progress -= 0.5;
    progress *= 2.0;
    progress = progress.abs();
    progress -= 0.5;
    progress *= 2.0;

    *m_ptr = m_initVal + progress * m_amplitude;
}
