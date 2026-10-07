#pragma once
#include <functional>
#include <sstream>
#include <string>
#include "MathFunctions.h"
#include "GameTimer.h"
#include "Vec3.h"

class PropertyModifierBase
{
private:
	std::string m_propName;
protected:
	virtual std::string getClassName() const = 0;
	virtual void serializeInner(std::stringstream& ss) const = 0;
	virtual void getShaderModCall(std::string& call) const = 0;
	PropertyModifierBase(std::string_view propName) : m_propName(propName) {}
public:
	std::string_view propName() { return m_propName; }
	virtual void BindToProp(void* ref) = 0;
	virtual void update() = 0;
	virtual void resetProperty() const = 0;
	void resetMods();
	void serialize(std::vector<std::string>& strVec, size_t level, bool boundOnObj);
	bool getShaderCall(std::string& propName) const;
};

template <typename T>
class PropertyModifier : public PropertyModifierBase
{
private:
	std::function<void(T*, T)> m_updateFunction;
protected:
	T* m_ptr{};
	T m_initVal{};
public:
	PropertyModifier(std::string_view propName, std::function<void(T*, T)> updateFunction)
		: PropertyModifierBase(propName), m_updateFunction(updateFunction) {}
	void BindToProp(void* ref) override { m_ptr = reinterpret_cast<T*>(ref); m_initVal = *m_ptr; }
	void update() override { m_updateFunction(m_ptr, m_initVal); }
	void resetProperty() const override;
};

template<typename T>
inline void PropertyModifier<T>::resetProperty() const
{
	*m_ptr = m_initVal;
}

template <typename T, typename U>
class PMPeriodic : public PropertyModifier<T>
{
public:
	inline static const double DFLT_PERIOD{ 1.0 };
	inline static const double DFLT_AMPLITUDE{ 1.0 };
	inline static const double DFLT_PHASE{ 0.0 };
protected:
	U m_period{};
	U m_amplitude{};
	U m_phase{};
	void serializeInner(std::stringstream& ss) const override;
public:
	PMPeriodic(std::string_view propName, std::function<void(T*, T)> updateFunction, U period, U amplitude, U phase)
		: PropertyModifier<T>(propName, updateFunction), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};

template <typename T, typename U>
void PMPeriodic<T, U>::serializeInner(std::stringstream& ss) const
{
	ss << " period " << m_period;
	ss << " amplitude " << m_amplitude;
	ss << " phase " << m_phase;
}

class MODOscillateSine : public PMPeriodic<double, double>
{
protected:
	std::string getClassName() const override { return "PMOscillateSine"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateSine(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](double* ptr, double initVal) -> void {
		*ptr = m_initVal + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
	}, period, amplitude, phase) {}
};

class MODOscillateLinear : public PMPeriodic<double, double>
{
protected:
	std::string getClassName() const override { return "PMOscillateLinear"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateLinear(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](double* ptr, double initVal) -> void {
		double progress = fmod(0.25 * m_period + (GameTimer::gameTime() + (m_phase * m_period)), m_period) / m_period;
		progress -= 0.5;
		progress *= 2.0;
		progress = abs(progress);
		progress -= 0.5;
		progress *= 2.0;

		*ptr = m_initVal + progress * m_amplitude;
	}, period, amplitude, phase) {}
};

class MODOscillateSineV2 : public PMPeriodic<Vec2, Vec2>
{
protected:
	std::string getClassName() const override { return "PMOscillateSineV2"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateSineV2(std::string_view propName, Vec2 period = DFLT_PERIOD, Vec2 amplitude = DFLT_AMPLITUDE, Vec2 phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](Vec2* ptr, Vec2 initVal) -> void {
		*ptr = m_initVal + ((Vec2{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
	}, period, amplitude, phase) {}
};

class MODOscillateLinearV2 : public PMPeriodic<Vec2, Vec2>
{
protected:
	std::string getClassName() const override { return "PMOscillateLinearV2"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateLinearV2(std::string_view propName, Vec2 period = DFLT_PERIOD, Vec2 amplitude = DFLT_AMPLITUDE, Vec2 phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](Vec2* ptr, Vec2 initVal) -> void {
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

		*ptr = m_initVal + progress * m_amplitude;
	}, period, amplitude, phase) {}
};

class MODOscillateSineV3 : public PMPeriodic<Vec3, Vec3>
{
protected:
	std::string getClassName() const override { return "PMOscillateSineV3"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateSineV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](Vec3* ptr, Vec3 initVal) -> void {
		*ptr = m_initVal + ((Vec3{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
	}, period, amplitude, phase) {}
};

class MODOscillateLinearV3 : public PMPeriodic<Vec3, Vec3>
{
protected:
	std::string getClassName() const override { return "PMOscillateLinearV3"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateLinearV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](Vec3* ptr, Vec3 initVal) -> void {
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

		*ptr = m_initVal + progress * m_amplitude;
	}, period, amplitude, phase) {}
};

class MODOscillateSineST : public PMPeriodic<size_t, double>
{
protected:
	std::string getClassName() const override { return "PMOscillateSineST"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateSineST(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](size_t* ptr, size_t initVal) -> void {
		double dVal = static_cast<double>(m_initVal) + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
		dVal = dVal < 0.0 ? 0.0 : dVal;
		*ptr = static_cast<size_t>(std::round(dVal));
	}, period, amplitude, phase) {}
};

class MODOscillateLinearST : public PMPeriodic<size_t, double>
{
protected:
	std::string getClassName() const override { return "PMOscillateLinearST"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODOscillateLinearST(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: PMPeriodic(propName, [this](size_t* ptr, size_t initVal) -> void {
		double progress = fmod(0.25 * m_period + (GameTimer::gameTime() + (m_phase * m_period)), m_period) / m_period;
		progress -= 0.5;
		progress *= 2.0;
		progress = abs(progress);
		progress -= 0.5;
		progress *= 2.0;

		double dVal = static_cast<double>(m_initVal) + progress * m_amplitude;
		dVal = dVal < 0.0 ? 0.0 : dVal;
		*ptr = static_cast<size_t>(std::round(dVal));
	}, period, amplitude, phase) {}
};
