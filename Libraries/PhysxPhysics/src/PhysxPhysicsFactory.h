#pragma once

#include "PhysicsWorld.h"
#include <iPhysicsFactory.h>

namespace physics
{
	namespace physxim
	{
		class PhysicsFactory : public iPhysicsFactory
		{
		public:
			PhysicsFactory();
			virtual ~PhysicsFactory();
			virtual iPhysicsWorld* CreateWorld() override;
			virtual iRigidBody* CreateRigidBody(const RigidBodyDesc& desc, iShape* shape) override;
			virtual iSoftBody* CreateSoftBody(const SoftBodyDesc& desc) override;

		private:
			PhysicsWorld* physxPhysicsWorld;
		};
	};
}