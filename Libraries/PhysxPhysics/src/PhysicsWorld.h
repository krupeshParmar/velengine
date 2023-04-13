#pragma once
#include <PxPhysicsAPI.h>
#include <iPhysicsWorld.h>
#include <vector>
#include "RigidBody.h"
#include "SoftBody.h"
#include "CollisionHandler.h"
#include <map>

namespace physics
{
	namespace physxim
	{

		class PhysicsWorld : public iPhysicsWorld
		{
		public:
			PhysicsWorld();
			virtual ~PhysicsWorld();

			virtual void SetGravity(const Vector3& gravity) override;

			virtual void AddBody(iCollisionBody* body) override;
			virtual void RemoveBody(iCollisionBody* body) override;
			virtual iCharacterController* CreateCharacterController(CharacterControllerDesc desc) override;

			virtual void TimeStep(float dt) override;

			virtual void RegisterCollisionListener(iCollisionListener* listener) override
			{
				m_CollisionListener = listener;
			}

			RigidBody* AddActor(const RigidBodyDesc& desc, iShape* shape);

			physx::PxScene* GetScene() { return mScene; }
			physx::PxPhysics* GetPhysics() { return mPhysics; }

		private:
			physx::PxFoundation* mFoundation;
			physx::PxDefaultAllocator mDefaultAllocatorCallback;
			physx::PxDefaultErrorCallback mDefaultErrorCallback;
			physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
			physx::PxTolerancesScale mToleranceScale;
			physx::PxPhysics* mPhysics = NULL;
			physx::PxScene* mScene = NULL;
			physx::PxMaterial* mMaterial = NULL;
			physx::PxPvd* mPVD = NULL;
			physx::PxControllerManager* mControllerManager = NULL;
			Vector3 m_Gravity;

			std::vector<iCollisionBody*> m_Bodies;
			std::vector<physx::PxRigidActor*> m_RigidBodies;
			std::vector<SoftBody*> m_SoftBodies;

			CollisionHandler* m_CollisionHandler;

			iCollisionListener* m_CollisionListener;


			PhysicsWorld(const PhysicsWorld&) {}
			PhysicsWorld& operator=(const PhysicsWorld&) {
				return *this;
			}
		};
	}
}