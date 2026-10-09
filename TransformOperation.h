#pragma once
#include "SDFOperation.h"

class TransformOperation : public SDFOperation
{
protected:
	void innerUpdate() override {};
	virtual std::string getClassName() const = 0;
	virtual void innerInnerSerialize(std::stringstream& ss) const {};
	void innerSerialize(std::vector<std::string>& strVec, size_t level) const override;
	TransformOperation() : SDFOperation(SDFOperationType::Transform) {};
public:
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override {};
	friend void SDFOperation::execute(Vec3& p, double& distance, const Color colorIn, Color* colorOut);
};

class OPMirrorX : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorX"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorX() {};
};

class OPMirrorY : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorY"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorY() {};
};

class OPMirrorZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorZ() {};
};

class OPMirrorXY : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorXY"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorXY() {};
};

class OPMirrorXZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorXZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorXZ() {};
};

class OPMirrorYZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorYZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorYZ() {};
};

class OPMirrorXYZ : public TransformOperation
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPMirrorXYZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPMirrorXYZ() {};
};

class OPTranslate : public TransformOperation
{
public:
	inline static const Vec3 DFLT_TRANSLATION{ 0.0, 1.0, 0.0 };
private:
	Vec3 m_translation;
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPTranslate"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPTranslate(Vec3 translation = DFLT_TRANSLATION) : m_translation(translation) {}
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override;
};

class OPRotate : public TransformOperation
{
public:
	inline static const Vec3 DFLT_AXIS{ 0.0, 1.0, 0.0 };
	inline static const double DFLT_ROTATIONS{ 0.5 };
private:
	Vec3 m_axis;
	double m_rotations;
	Mat3 m_rotMat;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPRotate"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPRotate(Vec3 axis = DFLT_AXIS, double rotations = DFLT_ROTATIONS)
		: m_axis(axis), m_rotations(rotations) {}
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override;
};

class OPCubeRollHeight : public TransformOperation
{
public:
	inline static const double DFLT_CUBEHEIGHT{ 1.0 };
	inline static const double DFLT_PERIOD{ 1.0 };
	inline static const double DFLT_PHASE{ 0.0 };
private:
	double m_cubeHeight;
	double m_period;
	double m_phase;
	double m_updateVal{};
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPCubeRollHeight"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPCubeRollHeight(double cubeHeight = DFLT_CUBEHEIGHT, double period = DFLT_PERIOD, double phase = DFLT_PHASE)
		: m_cubeHeight(cubeHeight), m_period(period), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override;
};

class OPCubeRoller : public TransformOperation
{
public:
	inline static std::vector<Vec3>	DFLT_AXES{ Vec3::Forward, Vec3::Right, Vec3::Backward, Vec3::Left };
	inline static const double DFLT_QUARTERTURNTIME{ 0.5 };
	inline static const double DFLT_PHASE{ 0.0 };
private:
	std::vector<Vec3> m_axes;
	double m_quarterTurnTime;
	double m_phase;
	Mat3 m_updateVal;
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPCubeRoller"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPCubeRoller(std::vector<Vec3> axes = DFLT_AXES, double quarterTurnTime = DFLT_QUARTERTURNTIME, double phase = DFLT_PHASE)
		: m_axes(axes), m_quarterTurnTime(quarterTurnTime), m_phase(phase) {}
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override;
};

class Elongate : public TransformOperation
{
public:
	inline static const double DFLT_LENGTH{ 1.0 };
	inline static const double DFLT_OFFSET{ 0.0 };
protected:
	double m_length;
	double m_offset;
	double m_halfLen{};
	double m_halfLenNeg{};
	void innerUpdate() override;
	std::string getClassName() const { return "Elongate"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override {};
public:
	Elongate(double length = DFLT_LENGTH, double offset = DFLT_OFFSET)
		: m_length(length), m_offset(offset) {}
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override;
};

class OPElongateX : public Elongate
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPElongateX"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPElongateX(double length = DFLT_LENGTH, double offset = DFLT_OFFSET)
		: Elongate(length, offset) {}
};

class OPElongateY : public Elongate
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPElongateY"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPElongateY(double length = DFLT_LENGTH, double offset = DFLT_OFFSET)
		: Elongate(length, offset) {}
};

class OPElongateZ : public Elongate
{
protected:
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPElongateZ"; }
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPElongateZ(double length = DFLT_LENGTH, double offset = DFLT_OFFSET)
		: Elongate(length, offset) {}
};

class OPElongateXYZ : public TransformOperation
{
public:
	inline static const Vec3 DFLT_LENGTHS{ 1.0 };
	inline static const Vec3 DFLT_OFFSETS{ 0.0 };
private:
	Vec3 m_lengths;
	Vec3 m_offsets;
	Vec3 m_halfLens{};
	Vec3 m_halfLenNegs{};
protected:
	void innerUpdate() override;
	void transform(Vec3& p) const override;
	std::string getClassName() const { return "OPElongateXYZ"; }
	void innerInnerSerialize(std::stringstream& ss) const override;
	virtual void getShaderOpCalls(std::stringstream& ss, size_t level, size_t& maxLevel) const override;
public:
	OPElongateXYZ(Vec3 lengths = DFLT_LENGTHS, Vec3 offsets = DFLT_OFFSETS)
		: m_lengths(lengths), m_offsets(offsets) {}
	void bindPropertyModifier(std::unique_ptr<Animator> propMod) override;
};