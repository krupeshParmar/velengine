#include "CharacterController.h"
#include <characterkinematic/PxCapsuleController.h>
#include <BoxShape.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

CRITICAL_SECTION MoveLock;

namespace physics
{
	namespace physxim
	{
		CharacterController::CharacterController()
			:m_Controller(nullptr)
		{
			InitializeCriticalSection(&MoveLock);
			filters = physx::PxControllerFilters();
		}
		CharacterController::~CharacterController()
		{
			DeleteCriticalSection(&MoveLock);
		}
		void CharacterController::AddForce(glm::vec3 force)
		{
			physx::PxControllerState state;
			m_Controller->getState(state);
		}
		void CharacterController::Move(glm::vec3 displacement, float dt)
		{
			//EnterCriticalSection(&MoveLock);

			physx::PxVec3 disp = physx::PxVec3(displacement.x, displacement.y, displacement.z) * dt;
			m_Controller->move(
				disp,
				0.01f,
				dt, filters);

			//LeaveCriticalSection(&MoveLock);
		}
		void CharacterController::SetMass(float mass)
		{
		}
		void CharacterController::SetMaxLinearVelocity(float vel)
		{
		}
		void CharacterController::Reset(CharacterControllerDesc desc)
		{
			physx::PxCapsuleController* capCont = (physx::PxCapsuleController*)(m_Controller);
			capCont->setPosition(physx::PxExtendedVec3(desc.position.x, desc.position.y, desc.position.z));
			capCont->setRadius(desc.radius);
			capCont->setHeight(desc.height);
		}
		void CharacterController::SetPosition(glm::vec3 pos)
		{
		}
		void CharacterController::SetController(physx::PxController* cont)
		{
			m_Controller = cont;
			m_Rigidbody = new RigidBody(RigidBodyDesc(),new BoxShape(glm::vec3()));
			m_Rigidbody->physxRigidbody = m_Controller->getActor();
		}
	}
}