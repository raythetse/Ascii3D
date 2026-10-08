#include "Modifier.h"
#include <format>

void Animator::serialize(std::vector<std::string>& strVec, size_t level, bool boundOnObj)
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " boundOnObj " << boundOnObj;
    ss << " propName " << m_propName;
    serializeInner(ss);
    strVec.push_back(ss.str());
}

bool Animator::getShaderCall(std::string& propName) const
{
    if (m_propName == propName) {
        getShaderModCall(propName);
        return true;
    }
    return false;
}

void ANWaveSmooth::getShaderModCall(std::string& call) const
{
    call = std::format("waveSmooth({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void ANWaveSmooth::update() const
{
    *m_ptr = m_initVal + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
}

void ANWaveSmoothV2::getShaderModCall(std::string& call) const
{
    call = std::format("waveSmoothV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANWaveSmoothV2::update() const
{
    *m_ptr = m_initVal + ((Vec2{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void ANWaveSmoothV3::getShaderModCall(std::string& call) const
{
    call = std::format("waveSmoothV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANWaveSmoothV3::update() const
{
    *m_ptr = m_initVal + ((Vec3{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void ANWaveLinear::getShaderModCall(std::string& call) const
{
    call = std::format("waveLinear({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void ANWaveLinear::update() const
{
    double progress = fmod(0.25 * m_period + (GameTimer::gameTime() + (m_phase * m_period)), m_period) / m_period;
    progress -= 0.5;
    progress *= 2.0;
    progress = abs(progress);
    progress -= 0.5;
    progress *= 2.0;

    *m_ptr = m_initVal + progress * m_amplitude;
}

void ANWaveLinearV2::getShaderModCall(std::string& call) const
{
    call = std::format("waveLinearV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANWaveLinearV2::update() const
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

void ANWaveLinearV3::getShaderModCall(std::string& call) const
{
    call = std::format("waveLinearV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANWaveLinearV3::update() const
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
