#pragma once
#include <iPhysicsWorld.h>
#include <vector>
#include "RigidBody.h"
#include "SoftBody.h"
#include "CollisionHandler.h"

namespace physics
{
	namespace vel
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

			virtual void AddConstraint(iConstraint* constraint) override
			{

			}
			virtual void CreateBlastFrameWork()
			{}
			virtual void CreateBlastAsset()
			{}
			virtual void DamageAsset()
			{}


			virtual void RegisterCollisionListener(iCollisionListener* listener) override
			{
				m_CollisionListener = listener;
			}

		private:
			Vector3 m_Gravity;

			std::vector<iCollisionBody*> m_Bodies;
			std::vector<RigidBody*> m_RigidBodies;
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