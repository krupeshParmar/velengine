#pragma once
#include <PxPhysicsAPI.h>
#include <iPhysicsWorld.h>
#include <vector>
#include "RigidBody.h"
#include "SoftBody.h"
#include "CollisionHandler.h"
#include <map>
#include <toolkit/NvBlastTk.h>
#include <extensions/shaders/NvBlastExtDamageShaders.h>

namespace physics
{
	namespace physxim
	{
		struct NvBlastProgramParams
		{
			NvBlastProgramParams(NvBlastExtRadialDamageDesc* desc, int count, NvBlastExtMaterial* mat)
				: damageDesc(desc), damageCount(count), material(mat)
			{

			}
			NvBlastExtRadialDamageDesc* damageDesc;
			int damageCount;
			NvBlastExtMaterial* material;

		};

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
			virtual void AddConstraint(iConstraint* constraint) override;
			virtual void CreateBlastFrameWork() override;
			virtual void CreateBlastAsset() override;
			virtual void DamageAsset() override;

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
			Nv::Blast::TkFramework* blastFramework = nullptr;
			Nv::Blast::TkAsset* blastAsset = nullptr; 
			Nv::Blast::TkActor* blastActor = nullptr;
			Nv::Blast::TkGroup* group = nullptr;
			NvBlastProgramParams* blastParams = nullptr;


			Nv::Blast::TkAssetDesc* desc;
			Nv::Blast::TkActorDesc* actorDesc; 

			Vector3 m_Gravity;

			std::vector<iCollisionBody*> m_Bodies;
			std::vector<physx::PxRigidActor*> m_RigidBodies;
			std::vector<SoftBody*> m_SoftBodies;
			std::vector<physx::PxJoint*> m_Joints;

			CollisionHandler* m_CollisionHandler;

			iCollisionListener* m_CollisionListener;


			PhysicsWorld(const PhysicsWorld&) {}
			PhysicsWorld& operator=(const PhysicsWorld&) {
				return *this;
			}
		};
	}
}