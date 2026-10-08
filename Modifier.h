#pragma once
#include <functional>
#include <sstream>
#include <string>
#include "MathFunctions.h"
#include "GameTimer.h"
#include "Vec3.h"

inline std::string get_type_string(double) { return "double"; }
inline std::string get_type_string(size_t) { return "size_t"; }
inline std::string get_type_string(Vec2) { return "vec2"; }
inline std::string get_type_string(Vec3) { return "vec3"; }

class Animator
{
private:
	std::string m_propName;
protected:
	virtual std::string getClassName() const = 0;
	virtual void serializeInner(std::stringstream& ss) const = 0;
	virtual void getShaderModCall(std::string& call) const = 0;
	Animator(std::string_view propName) : m_propName(propName) {}
public:
	std::string_view propName() { return m_propName; }
	virtual void BindToProperty(void* ref) = 0;
	virtual void update() const = 0;
	virtual void resetProperty() const = 0;
	void resetMods();
	void serialize(std::vector<std::string>& strVec, size_t level, bool boundOnObj);
	bool getShaderCall(std::string& propName) const;
};

template <typename T>
class AnimatorTemplate : public Animator
{
protected:
	T* m_ptr{};
	T m_initVal{};
public:
	AnimatorTemplate(std::string_view propName)
		: Animator(propName) {}
	void BindToProperty(void* ref) override { m_ptr = reinterpret_cast<T*>(ref); m_initVal = *m_ptr; }
	void resetProperty() const override;
};

template<typename T>
inline void AnimatorTemplate<T>::resetProperty() const
{
	*m_ptr = m_initVal;
}

template <typename T, typename U>
class ANWave : public AnimatorTemplate<T>
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
	ANWave(std::string_view propName, U period, U amplitude, U phase)
		: AnimatorTemplate<T>(propName), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};

template <typename T, typename U>
void ANWave<T, U>::serializeInner(std::stringstream& ss) const
{
	ss << " period " << m_period;
	ss << " amplitude " << m_amplitude;
	ss << " phase " << m_phase;
}

class ANWaveSmooth : public ANWave<double, double>
{
protected:
	std::string getClassName() const override { return "ANWaveSmooth"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	ANWaveSmooth(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: ANWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class ANWaveSmoothV2 : public ANWave<Vec2, Vec2>
{
protected:
	std::string getClassName() const override { return "ANWaveSmoothV2"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	ANWaveSmoothV2(std::string_view propName, Vec2 period = DFLT_PERIOD, Vec2 amplitude = DFLT_AMPLITUDE, Vec2 phase = DFLT_PHASE)
		: ANWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class ANWaveSmoothV3 : public ANWave<Vec3, Vec3>
{
protected:
	std::string getClassName() const override { return "ANWaveSmoothV3"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	ANWaveSmoothV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: ANWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class ANWaveLinear : public ANWave<double, double>
{
protected:
	std::string getClassName() const override { return "ANWaveLinear"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	ANWaveLinear(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: ANWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class ANWaveLinearV2 : public ANWave<Vec2, Vec2>
{
protected:
	std::string getClassName() const override { return "ANWaveLinearV2"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	ANWaveLinearV2(std::string_view propName, Vec2 period = DFLT_PERIOD, Vec2 amplitude = DFLT_AMPLITUDE, Vec2 phase = DFLT_PHASE)
		: ANWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class ANWaveLinearV3 : public ANWave<Vec3, Vec3>
{
protected:
	std::string getClassName() const override { return "ANWaveLinearV3"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	ANWaveLinearV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: ANWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};
