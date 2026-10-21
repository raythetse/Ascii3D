#include "DomainOperation.h"
#include "Mat2.h"

Vec3 DomainOperation::getPForAxis(Vec3 p, Axis axis) const
{
	switch (axis)
	{
	case AXIS_X:
		return Vec3(p.y, p.x, p.z);
	case AXIS_Y:
		return p;
	case AXIS_Z:
		return Vec3(p.x, p.z, p.y);
	}
	return p;
}

std::string DomainOperation::getPSwizzleForAxis(Axis axis) const
{
	switch (axis)
	{
	case AXIS_X:
		return ".yxz";
	case AXIS_Y:
		return ".xyz";
	case AXIS_Z:
		return ".xzy";
	}
	return ".xyz";
}

void DomainOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing;

	strVec.push_back(ss.str());
}

void DomainOperation::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "spacing") {
		animation->BindToProperty(&m_spacing);
		m_animations.push_back(std::move(animation));
	}
}

void OPRepeat::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeat::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing;
	ss << " limits " << m_limits;

	strVec.push_back(ss.str());
}

void OPRepeat::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeat //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tpmVec31 = " << (getShaderAnimationCalls(propName = "limits") ? propName : m_limits.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * clamp(round(p" << level << " / pmVec30), -pmVec31, pmVec31);\n\n";
}

void OPRepeat::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "spacing") {
		animation->BindToProperty(&m_spacing);
		m_animations.push_back(std::move(animation));
		return;
	}
	else if (animation->propName() == "limits") {
		animation->BindToProperty(&m_limits);
		m_animations.push_back(std::move(animation));
	}
}

void OPRepeatCircle::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	p = getPForAxis(p, m_axis);

	double r = static_cast<double>(m_repetitions);
	double b = MF::TAU / r;
	double a = std::atan2(p.z, p.x);

	if (a < 0.0) a += MF::TAU;

	double i = std::floor((a + b * 0.5) / b);

	if (i >= r) i = 0.0;
	double c = b * i;
	Vec2 sc = MF::fast_sincos(c);

	Vec2 p2D{ p.x, p.z };
	p2D = Mat2{ sc.y, -sc.x, sc.x, sc.y } * p2D;
	p.x = p2D.x;
	p.z = p2D.y;
}

void OPRepeatCircle::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " repetitions " << m_repetitions;
	ss << " axis " << m_axis;

	strVec.push_back(ss.str());
}

void OPRepeatCircle::getShaderOpCalls(std::stringstream & ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPRepeatCircle //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "repetitions") ? propName : std::to_string(m_repetitions)) << ";\n";
	ss << "\tp" << level << " = repeatCircle(p" << level << getPSwizzleForAxis(m_axis) << ", pmFloat0);\n\n";
}

void OPRepeatCircle::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "repetitions") {
		animation->BindToProperty(&m_repetitions);
		m_animations.push_back(std::move(animation));
		return;
	}
}
