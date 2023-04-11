#pragma once
#include <iCharacterController.h>
#include <PxRigidDynamic.h>

namespace physics
{
	namespace physxim
	{
		class CharacterController : iCharacterController
		{
		public:
			CharacterController();
			~CharacterController();

			virtual void AddForce() override;
			virtual void SetMass() override;
			virtual void SetMaxLinearVelocity() override;
			virtual void SetPosition() override;
		private:
			physx::PxRigidDynamic* controller;
		};
	}
}