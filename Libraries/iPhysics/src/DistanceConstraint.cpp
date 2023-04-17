#include "DistanceConstraint.h"

namespace physics
{
	DistanceConstraint::DistanceConstraint(iRigidBody* rbA, iRigidBody* rbB, glm::vec3 positionA, glm::vec3 positionB)
		: iConstraint(ConstraintType::Distance)
		, m_RigidBodyA(rbA)
		, m_RigidBodyB(rbB)
		, m_PositionA(positionA)
		, m_PositionB(positionB)
	{
	}
	DistanceConstraint* DistanceConstraint::Cast(iConstraint* constraint)
	{
		return dynamic_cast<DistanceConstraint*>(constraint);
	}
}