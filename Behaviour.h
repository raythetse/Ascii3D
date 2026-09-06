#pragma once
#include "Vec3.h"
#include "Mat3.h"

class Behaviour
{
protected:
	double m_speed;
	double m_magnitude;
	Vec3 m_vector;
public:
	Behaviour(double speed = 1.0, double magnitude = 1.0, Vec3 vector = {}) : m_speed(speed), m_magnitude(magnitude), m_vector(vector) {}
	Vec3 m_initPosition;
	virtual Vec3 getPosition(Vec3 position, double time) = 0;
	virtual ~Behaviour() = default;
};

class BobBehaviour : public Behaviour {
public:
	BobBehaviour(double speed, double magnitude, Vec3 vector = Vec3::Up) : Behaviour(speed, magnitude, vector) {}
	~BobBehaviour() override = default;
	Vec3 getPosition(Vec3 position, double time) override {
		return position + m_vector * m_magnitude * sin(time * m_speed);
	}
};

class RevolveBehaviour : public Behaviour {
public:
	RevolveBehaviour(double speed, double magnitude, Vec3 vector = Vec3::Up) : Behaviour(speed, magnitude, vector) {}
	~RevolveBehaviour() override = default;
	Vec3 getPosition(Vec3 position, double time) override {
		Mat3 m = Mat3::fromAxisAngle(m_vector, time * m_speed);
		Vec3 dir = m * (Vec3::Forward * m_magnitude);
		return m_initPosition + dir;
	}
};

