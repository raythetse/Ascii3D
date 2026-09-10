#include "DomainOperation.h"

void DomainOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "spacing") {
		propMod->BindToProp(&m_spacing);
		m_propMods.push_back(std::move(propMod));
	}
}

void RepeatX::transform(Vec3& p) const
{
	if (!m_spacing.x) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
}

void RepeatY::transform(Vec3& p) const
{
	if (!m_spacing.y) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void RepeatZ::transform(Vec3& p) const
{
	if (!m_spacing.z) return;
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void RepeatXY::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
}

void RepeatXZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.z) return;
	p.x -= m_spacing.x * round(p.x / m_spacing.x);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void RepeatYZ::transform(Vec3& p) const
{
	if (!m_spacing.y || !m_spacing.z) return;
	p.y -= m_spacing.y * round(p.y / m_spacing.y);
	p.z -= m_spacing.z * round(p.z / m_spacing.z);
}

void RepeatXYZ::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round();
}

void RepeatLimited::transform(Vec3& p) const
{
	if (!m_spacing.x || !m_spacing.y || !m_spacing.z) return;
	p -= m_spacing * (p / m_spacing).round().clamp(-m_limits, m_limits);
}

void RepeatLimited::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "limits") {
		propMod->BindToProp(&m_limits);
		m_propMods.push_back(std::move(propMod));
	}
}