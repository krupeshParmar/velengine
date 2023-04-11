#include "pch.h"
#include "RigidBody.h"
#include <PxRigidDynamic.h>

namespace physics
{
	namespace physxim
	{
		RigidBody* RigidBody::Cast(iCollisionBody* body)
		{
			return dynamic_cast<RigidBody*>(body);
		}

		RigidBody::RigidBody(const RigidBodyDesc& desc, iShape* shape)
			:mDesc(desc)
		{
		}

		RigidBody::~RigidBody(void)
		{
		}

		bool RigidBody::IsStatic(void) const
		{
			return mDesc.isStatic;
		}

		iShape* RigidBody::GetShape(void)
		{
			return nullptr;
		}

		//void RigidBody::GetWorldTransform(glm::mat4& transformOut)
		//{
		//}

		void RigidBody::GetPosition(glm::vec3& positionOut)
		{
			positionOut = mPosition;
		}

		void RigidBody::SetPosition(const glm::vec3& positionIn)
		{
			mPosition = positionIn;
		}

		void RigidBody::GetRotation(glm::quat& orientationOut)
		{
			orientationOut = mRotation;
		}

		void RigidBody::SetRotation(const glm::quat& orientationIn)
		{
			mRotation = orientationIn;
		}

		void RigidBody::ApplyForce(const glm::vec3& force)
		{
			physx::PxRigidDynamic* rigidBody = (physx::PxRigidDynamic*)physxRigidbody;
			rigidBody->addForce({force.x, force.y, force.z});
		}

		void RigidBody::ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& relativePoint)
		{
		}

		void RigidBody::ApplyImpulse(const glm::vec3& impulse)
		{
			physx::PxRigidDynamic* rigidBody = (physx::PxRigidDynamic*)physxRigidbody;
			rigidBody->addForce({ impulse.x, impulse.y, impulse.z }, physx::PxForceMode::eIMPULSE);
		}

		void RigidBody::ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint)
		{
		}

		void RigidBody::ApplyTorque(const glm::vec3& torque)
		{
		}

		void RigidBody::ApplyTorqueImpulse(const glm::vec3& torqueImpulse)
		{
		}

		void RigidBody::SetRenderPosition(Vector3* position)
		{

		}

	}
}