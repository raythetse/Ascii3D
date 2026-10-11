#include "DomainOperation.h"

void DomainOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level - 1, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing;

	strVec.push_back(ss.str());
}

Amination* DomainOperation::bindAnimation(std::unique_ptr<Amination> animation)
{
	if (animation->propName() == "spacing") {
		animation->BindToProperty(&m_spacing);
		m_animations.push_back(std::move(animation));
		return m_animations.back().get();
	}
	return nullptr;
}

void OPRepeatX::transform(Vec3& p) const
{
	if (!m_spacing.x) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
}

void OPRepeatX::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.x) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatX //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "spacing") ? propName + ".x" : std::to_string(m_spacing.x)) << ";\n";
	ss << "\tp" << level << ".x -= pmFloat0 * round(p" << level << ".x / pmFloat0);\n\n";
}

void OPRepeatY::transform(Vec3& p) const
{
	if (!m_spacing.y) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void OPRepeatY::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.y) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatY //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "spacing") ? propName + ".y" : std::to_string(m_spacing.y)) << ";\n";
	ss << "\tp" << level << ".y -= pmFloat0 * round(p" << level << ".y / pmFloat0);\n\n";
}

void OPRepeatZ::transform(Vec3& p) const
{
	if (!m_spacing.z) return;
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void OPRepeatZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatZ //\n";
	ss << "\tpmFloat0 = " << (getShaderAnimationCalls(propName = "spacing") ? propName + ".z" : std::to_string(m_spacing.z)) << ";\n";
	ss << "\tp" << level << ".z -= pmFloat0 * round(p" << level << ".z / pmFloat0);\n\n";
}

void OPRepeatXY::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void OPRepeatXY::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatXY //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << ".xy -= pmVec30.xy * round(p" << level << ".xy / pmVec30.xy);\n\n";
}

void OPRepeatXZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void OPRepeatXZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatXZ //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << ".xz -= pmVec30.xz * round(p" << level << ".xz / pmVec30.xz);\n\n";
}

void OPRepeatYZ::transform(Vec3& p) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void OPRepeatYZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatYZ //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << ".yz -= pmVec30.yz * round(p" << level << ".yz / pmVec30.yz);\n\n";
}

void OPRepeatXYZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round();
}

void OPRepeatXYZ::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatXYZ //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * round(p" << level << " / pmVec30);\n\n";
}

void OPRepeatLimited::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
}

void OPRepeatLimited::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level - 1, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing;
	ss << " limits " << m_limits;

	strVec.push_back(ss.str());
}

void OPRepeatLimited::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatLimited //\n";
	ss << "\tpmVec30 = " << (getShaderAnimationCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tpmVec31 = " << (getShaderAnimationCalls(propName = "limits") ? propName : m_limits.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * clamp(round(p" << level << " / pmVec30), -pmVec31, pmVec31);\n\n";
}

Amination* OPRepeatLimited::bindAnimation(std::unique_ptr<Amination> animation)
{
	if (animation->propName() == "spacing") {
		animation->BindToProperty(&m_spacing);
		m_animations.push_back(std::move(animation));
		return m_animations.back().get();
	}
	if (animation->propName() == "limits") {
		animation->BindToProperty(&m_limits);
		m_animations.push_back(std::move(animation));
		return m_animations.back().get();
	}
	return nullptr;
}