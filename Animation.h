#pragma once
#include <concepts>
#include <functional>
#include <sstream>
#include <string>
#include "MathFunctions.h"
#include "GameTimer.h"
#include "Vec3.h"

template <typename T>
concept AnimType =	std::same_as<T, double> || std::same_as<T, Vec2> || std::same_as<T, Vec3>;

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
	void serialize(std::vector<std::string>& strVec, size_t level);
	bool getShaderCall(std::string& propName) const;
};

template <AnimType T>
class AnimatorTemplate : public Animator
{
protected:
	T* m_ptr{};
	T m_initVal{};
	T m_amplitude{};
	T m_phase{};
public:
	AnimatorTemplate(std::string_view propName, T amplitude, T phase)
		: Animator(propName), m_amplitude(amplitude), m_phase(phase) {}
	void BindToProperty(void* ref) override { m_ptr = reinterpret_cast<T*>(ref); m_initVal = *m_ptr; }
	void resetProperty() const override;
};

template<AnimType T>
inline void AnimatorTemplate<T>::resetProperty() const
{
	*m_ptr = m_initVal;
}

template <AnimType T>
class ANContinuous : public AnimatorTemplate<T>
{
public:
	inline static const double DFLT_AMPLITUDE{ 1.0 };
	inline static const double DFLT_PHASE{ 0.0 };
protected:
	void serializeInner(std::stringstream& ss) const override;
public:
	ANContinuous(std::string propName, T amplitude = DFLT_AMPLITUDE, T phase = DFLT_PHASE)
		: AnimatorTemplate<T>(propName, amplitude, phase) {}
	virtual void update() const override;
};

template<AnimType T>
inline void ANContinuous<T>::serializeInner(std::stringstream& ss) const
{
	ss << " amplitude " << ANContinuous::m_amplitude;
	ss << " phase " << ANContinuous::m_phase;
}

template<AnimType T>
inline void ANContinuous<T>::update() const
{
	*ANContinuous::m_ptr = ANContinuous::m_initVal + (ANContinuous::m_phase + GameTimer::gameTime()) * ANContinuous::m_amplitude;
}

class ANContinuous1D : public ANContinuous<double>
{
protected:
	std::string getClassName() const override { return "ANContinuous1D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANContinuous<double>::ANContinuous;
};

class ANContinuous2D : public ANContinuous<Vec2>
{
protected:
	std::string getClassName() const override { return "ANContinuous2D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANContinuous<Vec2>::ANContinuous;
};

class ANContinuous3D : public ANContinuous<Vec3>
{
protected:
	std::string getClassName() const override { return "ANContinuous3D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANContinuous<Vec3>::ANContinuous;
};

template <AnimType T>
class ANSmoothCycle : public AnimatorTemplate<T>
{
public:
	inline static const double DFLT_PERIOD{ 1.0 };
	inline static const double DFLT_AMPLITUDE{ 1.0 };
	inline static const double DFLT_PHASE{ 0.0 };
protected:
	T m_period{};
	void serializeInner(std::stringstream& ss) const override;
public:
	ANSmoothCycle(std::string_view propName, T period = DFLT_PERIOD, T amplitude = DFLT_AMPLITUDE, T phase = DFLT_PHASE)
		: AnimatorTemplate<T>(propName, amplitude, phase), m_period(period) {}
};

template <AnimType T>
void ANSmoothCycle<T>::serializeInner(std::stringstream& ss) const
{
	ss << " period " << m_period;
	ss << " amplitude " << ANSmoothCycle::m_amplitude;
	ss << " phase " << ANSmoothCycle::m_phase;
}

class ANSmoothCycle1D : public ANSmoothCycle<double>
{
protected:
	std::string getClassName() const override { return "ANSmoothCycle1D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANSmoothCycle<double>::ANSmoothCycle;
	virtual void update() const override;
};

class ANSmoothCycle2D : public ANSmoothCycle<Vec2>
{
protected:
	std::string getClassName() const override { return "ANSmoothCycle2D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANSmoothCycle<Vec2>::ANSmoothCycle;
	virtual void update() const override;
};

class ANSmoothCycle3D : public ANSmoothCycle<Vec3>
{
protected:
	std::string getClassName() const override { return "ANSmoothCycle3D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANSmoothCycle<Vec3>::ANSmoothCycle;
	virtual void update() const override;
};


template <AnimType T>
class ANLinearCycle : public AnimatorTemplate<T>
{
public:
	inline static const double DFLT_AMPLITUDE{ 1.0 };
	inline static const double DFLT_FORWARDTIME{ 1.0 };
	inline static const double DFLT_DELAYTIME1{ 1.0 };
	inline static const double DFLT_BACKWARDTIME{ 1.0 };
	inline static const double DFLT_DELAYTIME2{ 1.0 };
	inline static const double DFLT_PHASE{ 1.0 };
protected:
	T m_forwardTime;
	T m_delayTime1;
	T m_backwardTime;
	T m_delayTime2;
	void serializeInner(std::stringstream& ss) const override;
	double updateSingle(double initVal, double amplitude, double forwardTime, double delayTime1, double backwardTime, double delayTime2, double phase) const;
public:
	ANLinearCycle<T>(std::string propName, T amplitude = DFLT_AMPLITUDE, T forwardTime = DFLT_FORWARDTIME, T delayTime1 = DFLT_DELAYTIME1
		, T backwardTime = DFLT_BACKWARDTIME, T delayTime2 = DFLT_DELAYTIME2, T phase = DFLT_PHASE)
		: AnimatorTemplate<T>(propName, amplitude, phase), m_forwardTime(forwardTime), m_delayTime1(delayTime1), m_backwardTime(backwardTime), m_delayTime2(delayTime2) {}
};

template <AnimType T>
void ANLinearCycle<T>::serializeInner(std::stringstream& ss) const
{
	ss << " forwardTime " << m_forwardTime;
	ss << " delayTime1 " << m_delayTime1;
	ss << " backwardTime " << m_backwardTime;
	ss << " delayTime2 " << m_delayTime2;
	ss << " phase " << ANLinearCycle::m_phase;
	ss << " amplitude " << ANLinearCycle::m_amplitude;
}

template<AnimType T>
inline double ANLinearCycle<T>::updateSingle(double initVal, double amplitude, double forwardTime, double delayTime1, double backwardTime, double delayTime2, double phase) const
{
	double totalTime{ forwardTime + delayTime1 + backwardTime + delayTime2 };

	double time = std::fmod(GameTimer::gameTime() + phase, totalTime);
	double progress{};

	if (time < forwardTime) {
		progress = time / forwardTime;
	}
	else if ((time -= forwardTime) < delayTime1) {
		progress = 1.0;
	}
	else if ((time -= delayTime1) < backwardTime) {
		progress = 1.0 - (time / backwardTime);
	}
	else {
		progress = 0.0;
	}

	return initVal + std::lerp(-1.0, 1.0, progress) * amplitude;
}

class ANLinearCycle1D : public ANLinearCycle<double>
{
protected:
	std::string getClassName() const override { return "ANLinearCycle1D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
	using ANLinearCycle<double>::ANLinearCycle;
	virtual void update() const override;
};

class ANLinearCycle2D : public ANLinearCycle<Vec2>
{
protected:
	std::string getClassName() const override { return "ANLinearCycle2D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
using ANLinearCycle<Vec2>::ANLinearCycle;
	virtual void update() const override;
};

class ANLinearCycle3D : public ANLinearCycle<Vec3>
{
protected:
	std::string getClassName() const override { return "ANLinearCycle3D"; }
	virtual void getShaderModCall(std::string& call) const override;
public:
 using ANLinearCycle<Vec3>::ANLinearCycle;
	virtual void update() const override;
};
