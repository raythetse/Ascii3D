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

class Modifier
{
private:
	std::string m_propName;
protected:
	virtual std::string getClassName() const = 0;
	virtual void serializeInner(std::stringstream& ss) const = 0;
	virtual void getShaderModCall(std::string& call) const = 0;
	Modifier(std::string_view propName) : m_propName(propName) {}
public:
	std::string_view propName() { return m_propName; }
	virtual void BindToProp(void* ref) = 0;
	virtual void update() const = 0;
	virtual void resetProperty() const = 0;
	void resetMods();
	void serialize(std::vector<std::string>& strVec, size_t level, bool boundOnObj);
	bool getShaderCall(std::string& propName) const;
};

template <typename T>
class ModifierTemplate : public Modifier
{
protected:
	T* m_ptr{};
	T m_initVal{};
public:
	ModifierTemplate(std::string_view propName)
		: Modifier(propName) {}
	void BindToProp(void* ref) override { m_ptr = reinterpret_cast<T*>(ref); m_initVal = *m_ptr; }
	void resetProperty() const override;
};

template<typename T>
inline void ModifierTemplate<T>::resetProperty() const
{
	*m_ptr = m_initVal;
}

template <typename T, typename U>
class MODWave : public ModifierTemplate<T>
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
	MODWave(std::string_view propName, U period, U amplitude, U phase)
		: ModifierTemplate<T>(propName), m_period(period), m_amplitude(amplitude), m_phase(phase) {}
};

template <typename T, typename U>
void MODWave<T, U>::serializeInner(std::stringstream& ss) const
{
	ss << " period " << m_period;
	ss << " amplitude " << m_amplitude;
	ss << " phase " << m_phase;
}

class MODWaveSmooth : public MODWave<double, double>
{
protected:
	std::string getClassName() const override { return "MODWaveSmooth"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODWaveSmooth(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: MODWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class MODWaveSmoothV2 : public MODWave<Vec2, Vec2>
{
protected:
	std::string getClassName() const override { return "MODWaveSmoothV2"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODWaveSmoothV2(std::string_view propName, Vec2 period = DFLT_PERIOD, Vec2 amplitude = DFLT_AMPLITUDE, Vec2 phase = DFLT_PHASE)
		: MODWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class MODWaveSmoothV3 : public MODWave<Vec3, Vec3>
{
protected:
	std::string getClassName() const override { return "MODWaveSmoothV3"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODWaveSmoothV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: MODWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class MODWaveLinear : public MODWave<double, double>
{
protected:
	std::string getClassName() const override { return "MODWaveLinear"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODWaveLinear(std::string_view propName, double period = DFLT_PERIOD, double amplitude = DFLT_AMPLITUDE, double phase = DFLT_PHASE)
		: MODWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class MODWaveLinearV2 : public MODWave<Vec2, Vec2>
{
protected:
	std::string getClassName() const override { return "MODWaveLinearV2"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODWaveLinearV2(std::string_view propName, Vec2 period = DFLT_PERIOD, Vec2 amplitude = DFLT_AMPLITUDE, Vec2 phase = DFLT_PHASE)
		: MODWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};

class MODWaveLinearV3 : public MODWave<Vec3, Vec3>
{
protected:
	std::string getClassName() const override { return "MODWaveLinearV3"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	MODWaveLinearV3(std::string_view propName, Vec3 period = DFLT_PERIOD, Vec3 amplitude = DFLT_AMPLITUDE, Vec3 phase = DFLT_PHASE)
		: MODWave(propName, period, amplitude, phase) {}
	virtual void update() const override;
};
