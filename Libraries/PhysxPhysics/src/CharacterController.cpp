#include "CharacterController.h"

namespace physics
{
	namespace physxim
	{
		CharacterController::CharacterController()
			:m_Controller(nullptr)
		{
		}
		CharacterController::~CharacterController()
		{
			
		}
		void CharacterController::AddForce(glm::vec3 force)
		{
			m_Controller->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eIMPULSE);
		}
		void CharacterController::SetMass(float mass)
		{
			m_Controller->setMass(physx::PxReal(mass));
		}
		void CharacterController::SetMaxLinearVelocity(float vel)
		{
			m_Controller->setMaxLinearVelocity(physx::PxReal(vel));
		}
		void CharacterController::SetPosition(glm::vec3 pos)
		{
			m_Controller->setGlobalPose(physx::PxTransform(physx::PxVec3(pos.x,pos.y,pos.z)));
		}
		void CharacterController::SetController(physx::PxRigidDynamic* cont)
		{
			m_Controller = cont;
		}
	}
}