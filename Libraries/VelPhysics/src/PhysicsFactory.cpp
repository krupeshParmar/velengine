#include "PhysicsFactory.h"
#include "PhysicsWorld.h"

namespace physics
{
	namespace vel
	{
		PhysicsFactory::PhysicsFactory()
		{
			// Creating Vel Physics
			// TODO: Log this somehow
		}
		PhysicsFactory::~PhysicsFactory()
		{
		}
		iPhysicsWorld* PhysicsFactory::CreateWorld()
		{
			PhysicsWorld* velPhysicsWorld = new PhysicsWorld();
			return velPhysicsWorld;
		}
		iRigidBody* PhysicsFactory::CreateRigidBody(const RigidBodyDesc& desc, iShape* shape)
		{
			return new RigidBody(desc, shape);
		}
		iSoftBody* PhysicsFactory::CreateSoftBody(const SoftBodyDesc& desc)
		{
			return  new SoftBody(desc);
		}
	}
}