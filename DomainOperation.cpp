#include "DomainOperation.h"
#include "Mat2.h"

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

void OPRepeatX::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.x) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatX::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.x) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatX //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "spacing") ? propName + ".x" : std::to_string(m_spacing.x)) << ";\n";
	ss << "\tp" << level << ".x -= pmFloat0 * round(p" << level << ".x / pmFloat0);\n\n";
}

void OPRepeatY::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.y) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatY::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.y) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatY //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "spacing") ? propName + ".y" : std::to_string(m_spacing.y)) << ";\n";
	ss << "\tp" << level << ".y -= pmFloat0 * round(p" << level << ".y / pmFloat0);\n\n";
}

void OPRepeatZ::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.z) return;
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatZ::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatZ //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "spacing") ? propName + ".z" : std::to_string(m_spacing.z)) << ";\n";
	ss << "\tp" << level << ".z -= pmFloat0 * round(p" << level << ".z / pmFloat0);\n\n";
}

void OPRepeatXY::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatXY::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatXY //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << ".xy -= pmVec30.xy * round(p" << level << ".xy / pmVec30.xy);\n\n";
}

void OPRepeatXZ::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatXZ::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatXZ //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << ".xz -= pmVec30.xz * round(p" << level << ".xz / pmVec30.xz);\n\n";
}

void OPRepeatYZ::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatYZ::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatYZ //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << ".yz -= pmVec30.yz * round(p" << level << ".yz / pmVec30.yz);\n\n";
}

void OPRepeatXYZ::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round();
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatXYZ::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatXYZ //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * round(p" << level << " / pmVec30);\n\n";
}

void OPRepeatLimited::execute(Vec3& p, double& distanceA, const Color colorIn, Color* colorOut) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
	if (colorOut) *colorOut = colorIn;
}

void OPRepeatLimited::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing;
	ss << " limits " << m_limits;

	strVec.push_back(ss.str());
}

void OPRepeatLimited::getShaderOpCalls(std::stringstream& ss, size_t level) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatLimited //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tpmVec31 = " << (getShaderAnimationCalls(propName = "limits") ? propName : m_limits.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * clamp(round(p" << level << " / pmVec30), -pmVec31, pmVec31);\n\n";
}

void OPRepeatLimited::bindAnimation(std::unique_ptr<Animation> animation)
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

	strVec.push_back(ss.str());
}

void OPRepeatCircle::getShaderOpCalls(std::stringstream & ss, size_t level) const
{
	std::string propName{ "" };
	ss << "\t// OPRepeatCircle //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "repetitions") ? propName : std::to_string(m_repetitions)) << ";\n";
	ss << "\tp" << level << " = repeatCircle(p" << level << ", pmFloat0);\n\n";
}

void OPRepeatCircle::bindAnimation(std::unique_ptr<Animation> animation)
{
	if (animation->propName() == "repetitions") {
		animation->BindToProperty(&m_repetitions);
		m_animations.push_back(std::move(animation));
		return;
	}
}
