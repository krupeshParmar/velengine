#pragma once

#include "iRigidBody.h"
#include <glm/glm.hpp>

namespace physics
{
	class iCharacterController
	{
	public:
		virtual ~iCharacterController() {}
		virtual void AddForce(glm::vec3 force) = 0;
		virtual void SetMass(float mass) = 0;
		virtual void SetMaxLinearVelocity(float velocity) = 0;
		virtual void SetPosition(glm::vec3 pos) = 0;
		virtual glm::vec3 GetPosition() = 0;

	protected:
		iCharacterController(){}

	private:
		iCharacterController(const iCharacterController&) {}
		iCharacterController& operator=(const iCharacterController&) {
			return *this;
		}
	};
}