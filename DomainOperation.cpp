#include "DomainOperation.h"

void DomainOperation::innerSerialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing.x << ' ' << m_spacing.y << ' ' << m_spacing.z;

	strVec.push_back(ss.str());
}

void DomainOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "spacing") {
		propMod->BindToProp(&m_spacing);
		m_propMods.push_back(std::move(propMod));
	}
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
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "spacing") ? propName + ".x" : std::to_string(m_spacing.x)) << ";\n";
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
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "spacing") ? propName + ".y" : std::to_string(m_spacing.y)) << ";\n";
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
	ss << "\tpmFloat0 = " << (getShaderPropModCalls(propName = "spacing") ? propName + ".z" : std::to_string(m_spacing.z)) << ";\n";
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
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
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
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
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
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
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
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
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
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " spacing " << m_spacing.x << ' ' << m_spacing.y << ' ' << m_spacing.z;
	ss << " limits " << m_limits.x << ' ' << m_limits.y << ' ' << m_limits.z;

	strVec.push_back(ss.str());
}

void OPRepeatLimited::getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	std::string propName{ "" };
	ss << "\t// OPRepeatLimited //\n";
	ss << "\tpmVec30 = " << (getShaderPropModCalls(propName = "spacing") ? propName : m_spacing.getShader()) << ";\n";
	ss << "\tpmVec31 = " << (getShaderPropModCalls(propName = "limits") ? propName : m_limits.getShader()) << ";\n";
	ss << "\tp" << level << " -= pmVec30 * clamp(round(p" << level << " / pmVec30), -pmVec31, pmVec31);\n\n";
}

void OPRepeatLimited::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "spacing") {
		propMod->BindToProp(&m_spacing);
		m_propMods.push_back(std::move(propMod));
		return;
	}
	if (propMod->propName() == "limits") {
		propMod->BindToProp(&m_limits);
		m_propMods.push_back(std::move(propMod));
	}
}