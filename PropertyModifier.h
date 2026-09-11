#pragma once
#include <functional>
#include <string>
#include "GameTimer.h"

class PropertyModifierBase
{
private:
	std::string m_propName;
public:
	PropertyModifierBase(std::string_view propName) : m_propName(propName) {}
	std::string_view propName() { return m_propName; }
	virtual void BindToProp(void* ref) = 0;
	virtual void update() = 0;
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
};

class PMSineOscillation : public PropertyModifier<double>
{
private:
	double m_period{};
	double m_amplitude{};
	double m_phase{};
public:
	PMSineOscillation(std::string_view propName, double period = 1.0, double amplitude = 1.0, double phase = 0.0)
		: PropertyModifier(propName, [this](double* ptr, double initVal) -> void {
		*ptr = m_initVal + std::sin((GameTimer::gameTime() + (m_phase * m_period)) / m_period) * m_amplitude;
	}), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};