#include "Animation.h"
#include <format>
#include "Enumerations.h"

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

void ANSmoothCycle1D::getShaderModCall(std::string& call) const
{
    call = std::format("smoothCycle1D({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void ANSmoothCycle1D::update() const
{
    *m_ptr = m_initVal + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
}

void ANSmoothCycle2D::getShaderModCall(std::string& call) const
{
    call = std::format("smoothCycle2D({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANSmoothCycle2D::update() const
{
    *m_ptr = m_initVal + ((Vec2{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void ANSmoothCycle3D::getShaderModCall(std::string& call) const
{
    call = std::format("smoothCycle3D({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANSmoothCycle3D::update() const
{
    *m_ptr = m_initVal + ((Vec3{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void ANLinearCycle1D::getShaderModCall(std::string& call) const
{
    call = std::format("linearCycle1D({:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_amplitude, m_forwardTime, m_delayTime1, m_backwardTime, m_delayTime2, m_phase);
}

void ANLinearCycle1D::update() const
{
    *m_ptr = updateSingle(m_initVal, m_amplitude, m_forwardTime, m_delayTime1, m_backwardTime, m_delayTime2, m_phase);
}

void ANLinearCycle2D::getShaderModCall(std::string& call) const
{
    call = "linearCycle2D(";
    call.append(m_initVal.getShader() + ", ");
    call.append(m_amplitude.getShader() + ", ");
    call.append(m_forwardTime.getShader() + ", ");
    call.append(m_delayTime1.getShader() + ", ");
    call.append(m_backwardTime.getShader() + ", ");
    call.append(m_delayTime2.getShader() + ", ");
    call.append(m_phase.getShader() + ", time)");
}

void ANLinearCycle2D::update() const
{
    *m_ptr = Vec2{
        updateSingle(m_initVal.x, m_amplitude.x, m_forwardTime.x, m_delayTime1.x, m_backwardTime.x, m_delayTime2.x, m_phase.x),
        updateSingle(m_initVal.y, m_amplitude.y, m_forwardTime.y, m_delayTime1.y, m_backwardTime.y, m_delayTime2.y, m_phase.y)
    };
}

void ANLinearCycle3D::getShaderModCall(std::string& call) const
{
    call = "linearCycle3D(";
    call.append(m_initVal.getShader() + ", ");
    call.append(m_amplitude.getShader() + ", ");
    call.append(m_forwardTime.getShader() + ", ");
    call.append(m_delayTime1.getShader() + ", ");
    call.append(m_backwardTime.getShader() + ", ");
    call.append(m_delayTime2.getShader() + ", ");
    call.append(m_phase.getShader() + ", time)");
}

void ANLinearCycle3D::update() const
{
    *m_ptr = Vec3{
        updateSingle(m_initVal.x, m_amplitude.x, m_forwardTime.x, m_delayTime1.x, m_backwardTime.x, m_delayTime2.x, m_phase.x),
        updateSingle(m_initVal.y, m_amplitude.y, m_forwardTime.y, m_delayTime1.y, m_backwardTime.y, m_delayTime2.y, m_phase.y),
        updateSingle(m_initVal.z, m_amplitude.z, m_forwardTime.z, m_delayTime1.z, m_backwardTime.z, m_delayTime2.z, m_phase.z)
    };
}

//void ANLinearCycle2D::getShaderModCall(std::string& call) const
//{
//    call = std::format("waveLinearV2({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
//}
//
//void ANLinearCycle2D::update() const
//{
//    double gameTime = GameTimer::gameTime();
//    Vec2 progress{
//        fmod(0.25 * m_period.x + (gameTime + (m_phase.x * m_period.x)), m_period.x) / m_period.x,
//        fmod(0.25 * m_period.y + (gameTime + (m_phase.y * m_period.y)), m_period.y) / m_period.y
//    };
//    progress -= 0.5;
//    progress *= 2.0;
//    progress = progress.abs();
//    progress -= 0.5;
//    progress *= 2.0;
//
//    *m_ptr = m_initVal + progress * m_amplitude;
//}
//
//void ANLinearCycle3D::getShaderModCall(std::string& call) const
//{
//    call = std::format("waveLinearV3({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
//}
//
//void ANLinearCycle3D::update() const
//{
//    double gameTime = GameTimer::gameTime();
//    Vec3 progress{
//        fmod(0.25 * m_period.x + (gameTime + (m_phase.x * m_period.x)), m_period.x) / m_period.x,
//        fmod(0.25 * m_period.y + (gameTime + (m_phase.y * m_period.y)), m_period.y) / m_period.y,
//        fmod(0.25 * m_period.z + (gameTime + (m_phase.z * m_period.z)), m_period.z) / m_period.z
//    };
//    progress -= 0.5;
//    progress *= 2.0;
//    progress = progress.abs();
//    progress -= 0.5;
//    progress *= 2.0;
//
//    *m_ptr = m_initVal + progress * m_amplitude;
//}
