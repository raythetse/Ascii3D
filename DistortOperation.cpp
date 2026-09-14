#include "DistortOperation.h"
#include <sstream>
#include "Mat2.h"

void OPTwist::transform(Vec3& p) const
{
	Vec2 sc = MF::fast_sincos(m_magnitude * p.y);
	Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y} };
	Vec2 xz = m * Vec2{ p.x, p.z };
	p = Vec3(xz.x, xz.y, p.y);
}

void OPBend::transform(Vec3& p) const
{
	Vec2 sc = MF::fast_sincos(m_magnitude * p.x);
	Mat2  m = Mat2{ { sc.y, -sc.x, sc.x, sc.y } };
	Vec2 xy = m * Vec2{ p.x, p.y };
	p = Vec3(xy.x, xy.y, p.z);
}

void DistortOperation::bindPropertyModifier(std::unique_ptr<PropertyModifierBase> propMod)
{
	if (propMod->propName() == "magnitude") {
		propMod->BindToProp(&m_magnitude);
		m_propMods.push_back(std::move(propMod));
	}
}

void DistortOperation::serialize(std::vector<std::string>& strVec, size_t level) const
{
	std::stringstream ss{};
	ss << std::string(level, '\t');
	ss << getClassName();
	ss << " m_magnitude " << m_magnitude;
	strVec.push_back(ss.str());
}
