#pragma once

#include <iPhysicsFactory.h>


namespace physics
{
	namespace vel
	{
		class PhysicsFactory : public iPhysicsFactory
		{
		public:
			PhysicsFactory();
			virtual ~PhysicsFactory();
			virtual iPhysicsWorld* CreateWorld() override;
			virtual iRigidBody* CreateRigidBody(const RigidBodyDesc& desc, iShape* shape) override;
			virtual iSoftBody* CreateSoftBody(const SoftBodyDesc& desc) override;
		};
	};
}