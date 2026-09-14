#include "DomainOperation.h"

void DomainOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " m_spacing " << m_spacing;

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

void OPRepeatY::transform(Vec3& p) const
{
	if (!m_spacing.y) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void OPRepeatZ::transform(Vec3& p) const
{
	if (!m_spacing.z) return;
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void OPRepeatXY::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void OPRepeatXZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void OPRepeatYZ::transform(Vec3& p) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void OPRepeatXYZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round();
}

void OPRepeatLimited::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
}

void OPRepeatLimited::serialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " m_spacing " << m_spacing;
	ss << " m_limits " << m_limits.x << ' ' << m_limits.y << ' ' << m_limits.z;

	strVec.push_back(ss.str());
}

void OPRepeatLimited::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "limits") {
		propMod->BindToProp(&m_limits);
		m_propMods.push_back(std::move(propMod));
	}
}