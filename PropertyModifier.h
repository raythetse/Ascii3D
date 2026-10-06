#pragma once
#include <functional>
#include <sstream>
#include <string>
#include "MathFunctions.h"
#include "GameTimer.h"
#include "Vec3.h"

class PropertyModifierBase
{
public:
	inline static const double DFLT_PERIOD{ 1.0 };
	inline static const double DFLT_AMPLITUDE{ 1.0 };
	inline static const double DFLT_PHASE{ 0.0 };
private:
	std::string m_propName;
protected:
	virtual std::string getClassName() const = 0;
	virtual void serializeInner(std::stringstream& ss) const = 0;
	virtual void getShaderModCall(std::string& call) const = 0;
public:
	PropertyModifierBase(std::string_view propName) : m_propName(propName) {}
	std::string_view propName() { return m_propName; }
	virtual void BindToProp(void* ref) = 0;
	virtual void update() = 0;
	virtual void resetProperty() const = 0;
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

class PMOscillateSine : public PropertyModifier<double>
{
private:
	double m_period{};
	double m_amplitude{};
	double m_phase{};
protected:
	std::string getClassName() const override { return "PMOscillateSine"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderModCall(std::string& call) const override;
public:
	PMOscillateSine(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: PropertyModifier(propName, [this](double* ptr, double initVal) -> void {
		*ptr = m_initVal + std::sin(MF::TAU * (GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
	}), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};

class PMOscillateSineV3 : public PropertyModifier<Vec3>
{
private:
	Vec3 m_period{};
	Vec3 m_amplitude{};
	Vec3 m_phase{};
protected:
	std::string getClassName() const override { return "PMOscillateSineV3"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderModCall(std::string& call) const override;
public:
	PMOscillateSineV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: PropertyModifier(propName, [this](Vec3* ptr, Vec3 initVal) -> void {
		*ptr = m_initVal + ((Vec3{ GameTimer::gameTime() } + m_phase * m_period) * MF::TAU / m_period).sin() * m_amplitude;
	}), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};

class PMOscillateLinear : public PropertyModifier<double>
{
private:
	double m_period{};
	double m_amplitude{};
	double m_phase{};
protected:
	std::string getClassName() const override { return "PMOscillateLinear"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderModCall(std::string& call) const override;
public:
	PMOscillateLinear(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: PropertyModifier(propName, [this](double* ptr, double initVal) -> void {
		double progress = fmod(0.25 * m_period + (GameTimer::gameTime() + (m_phase * m_period)), m_period) / m_period;
		progress -= 0.5;
		progress *= 2.0;
		progress = abs(progress);
		progress -= 0.5;
		progress *= 2.0;

		*ptr = m_initVal + progress * m_amplitude;
	}), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};

class PMOscillateLinearV3 : public PropertyModifier<Vec3>
{
private:
	Vec3 m_period{};
	Vec3 m_amplitude{};
	Vec3 m_phase{};
protected:
	std::string getClassName() const override { return "PMOscillateLinearV3"; }
	void serializeInner(std::stringstream& ss) const override;
	virtual void getShaderModCall(std::string& call) const override;
public:
	PMOscillateLinearV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: PropertyModifier(propName, [this](Vec3* ptr, Vec3 initVal) -> void {
		double gameTime = GameTimer::gameTime();
		Vec3 progress { 
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
	}), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};
