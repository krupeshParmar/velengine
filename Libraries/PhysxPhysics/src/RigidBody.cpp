#include "pch.h"
#include "RigidBody.h"
#include <PxRigidDynamic.h>
#include <PxRigidActor.h>

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
			if(physxRigidbody)
				physxRigidbody->setGlobalPose(physx::PxTransform(
					physx::PxVec3(positionIn.x, positionIn.y, positionIn.z)));
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

		void RigidBody::GetWorldSpaceTransform(glm::mat4& transform)
		{
			if (!physxRigidbody)
				return;
			physx::PxRigidDynamic* rigidbody = (physx::PxRigidDynamic*)physxRigidbody;
			physx::PxTransform rgtransform = rigidbody->getGlobalPose();
			transform = glm::translate(glm::mat4(1.f), glm::vec3(
				rgtransform.p.x, rgtransform.p.y, rgtransform.p.z)
			) * glm::toMat4(glm::quat(
				rgtransform.q.x,
				rgtransform.q.y,
				rgtransform.q.z,
				rgtransform.q.w
			));
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

		bool RigidBody::HasJoints()
		{
			return hasJoint;
		}

		void RigidBody::ReleaseJoints()
		{
			if (joints != nullptr)
			{
				joints->release();
				joints = nullptr;
				hasJoint = false;
			}
		}

	}
}