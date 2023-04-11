#include "pch.h"
#include "PhysxPhysicsFactory.h"
#include "PhysicsWorld.h"
#include <iostream>

namespace physics
{
	namespace physxim
	{
		PhysicsFactory::PhysicsFactory()
		{
		}
		PhysicsFactory::~PhysicsFactory()
		{
		}
		iPhysicsWorld* PhysicsFactory::CreateWorld()
		{
			physxPhysicsWorld = new PhysicsWorld();
			return physxPhysicsWorld;
		}
		iRigidBody* PhysicsFactory::CreateRigidBody(const RigidBodyDesc& desc, iShape* shape)
		{
			return physxPhysicsWorld->AddActor(desc, shape);
		}
		iSoftBody* PhysicsFactory::CreateSoftBody(const SoftBodyDesc& desc)
		{
			return  new SoftBody(desc);
		}
	}
}