#pragma once

namespace physics
{
	enum class ConstraintType
	{
		ConeTwist,
		Distance,
		Gear,
		Hinge,
		Point2Point,
		Slider
	};
	class iConstraint
	{
	public:
		virtual ~iConstraint() {}

		ConstraintType GetConstraintType() const {
			return m_ConstraintType;
		}

	protected:
		iConstraint(ConstraintType constraintType)
			: m_ConstraintType(constraintType)
		{ }

	private:
		ConstraintType m_ConstraintType;

		iConstraint(const iConstraint&) = delete;
		iConstraint& operator=(const iConstraint&) {}
	};
}