#pragma once
#include <PhyMath.h>
#include <iShape.h>
#include <iRigidBody.h>
#include <RigidBodyDesc.h>
#include <PxActor.h>
#include <extensions/PxJoint.h>

namespace physics
{
	namespace physxim
	{
		class CollisionHandler;
		class RigidBody : public iRigidBody
		{
		public:
			RigidBody(const RigidBodyDesc& desc, iShape* shape);
			~RigidBody();

			static RigidBody* Cast(iCollisionBody* body);

			iShape* GetShape(void);

			bool IsStatic(void) const;

			//virtual void GetWorldTransform(glm::mat4& transformOut) override;

			virtual void GetPosition(glm::vec3& positionOut) override;
			virtual void SetPosition(const glm::vec3& positionIn) override;

			virtual void GetRotation(glm::quat& orientationOut) override;
			virtual void SetRotation(const glm::quat& orientationIn) override;

			virtual void ApplyForce(const glm::vec3& force) override;
			virtual void ApplyForceAtPoint(const glm::vec3& force, const glm::vec3& relativePoint) override;

			virtual void ApplyImpulse(const glm::vec3& impulse) override;
			virtual void ApplyImpulseAtPoint(const glm::vec3& impulse, const glm::vec3& relativePoint) override;

			virtual void ApplyTorque(const glm::vec3& torque) override;
			virtual void ApplyTorqueImpulse(const glm::vec3& torqueImpulse) override;
			virtual void SetRenderPosition(Vector3* position) override;

			virtual void GetWorldSpaceTransform(glm::mat4& transform) override;
			virtual bool HasJoints() override;
			virtual void ReleaseJoints() override;

			physx::PxRigidActor* physxRigidbody;
			physx::PxJoint* joints = nullptr;
			bool hasJoint = false;
		protected:
			glm::vec3 mPosition;
			glm::quat mRotation;
			RigidBodyDesc mDesc;

		private:
			RigidBody(const RigidBody& other) {  }
			RigidBody& operator=(const RigidBody& other) { return *this; }

		};
	}
}