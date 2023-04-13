#include "CharacterController.h"

namespace physics
{
	namespace physxim
	{
		CharacterController::CharacterController()
			:m_Controller(nullptr)
		{
			filters = physx::PxControllerFilters();
		}
		CharacterController::~CharacterController()
		{
			
		}
		void CharacterController::AddForce(glm::vec3 force)
		{
		}
		void CharacterController::Move(glm::vec3 displacement, float dt)
		{
			physx::PxVec3 disp = physx::PxVec3(displacement.x, displacement.y, displacement.z) * dt;
			m_Controller->move(
				disp,
				0.01f,
				dt, filters);
		}
		void CharacterController::SetMass(float mass)
		{
		}
		void CharacterController::SetMaxLinearVelocity(float vel)
		{
		}
		void CharacterController::SetPosition(glm::vec3 pos)
		{
		}
		void CharacterController::SetController(physx::PxController* cont)
		{
			m_Controller = cont;
		}
	}
}