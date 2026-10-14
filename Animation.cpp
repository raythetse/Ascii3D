#include "Animation.h"
#include <format>
#include "Enumerations.h"

void Animation::serialize(std::vector<std::string>& strVec, size_t level)
{
    std::stringstream ss{};
    ss << std::string(level, '\t');
    ss << getClassName();
    ss << " propName " << m_propName;
    serializeInner(ss);
    strVec.push_back(ss.str());
}

bool Animation::getShaderCall(std::string& propName) const
{
    if (m_propName == propName) {
        getShaderCallInner(propName);
        return true;
    }
    return false;
}

void ANContinuous1D::getShaderCallInner(std::string& call) const
{
    call = std::format("({:.7f} + {:.7f} * ({:.7f} + time))", m_initVal, m_amplitude, m_phase);
}

void ANContinuous2D::getShaderCallInner(std::string& call) const
{
    call = std::format("({} + {} * ({} +  time))", m_initVal.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANContinuous3D::getShaderCallInner(std::string& call) const
{
    call = std::format("({} + {} * ({} +  time))", m_initVal.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANSmoothCycle1D::getShaderCallInner(std::string& call) const
{
    call = std::format("smoothCycle1D({:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_period, m_amplitude, m_phase);
}

void ANSmoothCycle1D::update() const
{
    *m_ptr = m_initVal + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
}

void ANSmoothCycle2D::getShaderCallInner(std::string& call) const
{
    call = std::format("smoothCycle2D({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANSmoothCycle2D::update() const
{
    *m_ptr = m_initVal + ((Vec2{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void ANSmoothCycle3D::getShaderCallInner(std::string& call) const
{
    call = std::format("smoothCycle3D({}, {}, {}, {}, time)", m_initVal.getShader(), m_period.getShader(), m_amplitude.getShader(), m_phase.getShader());
}

void ANSmoothCycle3D::update() const
{
    *m_ptr = m_initVal + ((Vec3{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
}

void ANLinearCycle1D::getShaderCallInner(std::string& call) const
{
    call = std::format("linearCycle1D({:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, {:.7f}, time)", m_initVal, m_amplitude, m_forwardTime, m_delayTime1, m_backwardTime, m_delayTime2, m_phase);
}

void ANLinearCycle1D::update() const
{
    *m_ptr = updateSingle(m_initVal, m_amplitude, m_forwardTime, m_delayTime1, m_backwardTime, m_delayTime2, m_phase);
}

void ANLinearCycle2D::getShaderCallInner(std::string& call) const
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

void ANLinearCycle3D::getShaderCallInner(std::string& call) const
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
