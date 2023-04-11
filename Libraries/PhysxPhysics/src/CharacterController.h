#pragma once
#include <iCharacterController.h>
#include <PxRigidDynamic.h>
#include <glm/glm.hpp>

namespace physics
{
	namespace physxim
	{
		class CharacterController : public iCharacterController
		{
		public:
			CharacterController();
			~CharacterController();

			virtual void AddForce(glm::vec3 force) override;
			virtual void SetMass(float mass) override;
			virtual void SetMaxLinearVelocity(float vel) override;
			virtual void SetPosition(glm::vec3 pos) override;
			virtual glm::vec3 GetPosition() override {
				return {
					m_Controller->getGlobalPose().p.x,
					m_Controller->getGlobalPose().p.y,
					m_Controller->getGlobalPose().p.z
				};
			}

			void SetController(physx::PxRigidDynamic* cont);
		private:
			physx::PxRigidDynamic* m_Controller;
		};
	}
}