#pragma once
#include "iConstraint.h"
#include "iRigidBody.h"
#include <glm/glm.hpp>
namespace physics
{
	class DistanceConstraint : public iConstraint
	{
	public:
		DistanceConstraint(iRigidBody* rbA, iRigidBody* rbB,
			glm::vec3 positionA, glm::vec3 positionB);

		virtual ~DistanceConstraint() {}

		static DistanceConstraint* Cast(iConstraint* constraint);

		iRigidBody* GetRididBodyA() const { return m_RigidBodyA; }
		iRigidBody* GetRididBodyB() const { return m_RigidBodyB; }
		const glm::vec3& GetPositionA() const { return m_PositionA; }
		const glm::vec3& GetPositionB() const { return m_PositionB; }

	private:
		iRigidBody* m_RigidBodyA;
		iRigidBody* m_RigidBodyB;
		glm::vec3 m_PositionA, m_PositionB;

		DistanceConstraint& operator=(const DistanceConstraint&) {
			return *this;
		}
	};
}