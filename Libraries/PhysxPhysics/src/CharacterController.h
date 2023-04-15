#pragma once
#include <iCharacterController.h>
#include <PxRigidDynamic.h>
#include <glm/glm.hpp>
#include <characterkinematic/PxController.h>

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
			virtual void Move(glm::vec3 displacement, float dt) override;
			virtual void SetMass(float mass) override;
			virtual void SetMaxLinearVelocity(float vel) override;
			virtual void Reset(CharacterControllerDesc desc) override;
			virtual void SetPosition(glm::vec3 pos) override;
			virtual glm::vec3 GetPosition() override {
				return {
					m_Controller->getFootPosition().x,
					m_Controller->getFootPosition().y,
					m_Controller->getFootPosition().z
				};
			}

			void SetController(physx::PxController* cont);
		private:
			//physx::PxRigidDynamic* m_Controller;
			physx::PxController* m_Controller;
			physx::PxControllerFilters filters;
		};
	}
}