#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace physics
{
	class CharacterControllerDesc
	{
	public:
		CharacterControllerDesc()
			:radius(2.f), height(2.f), position(glm::vec3(0.f)),rotation(glm::quat())
		{

		}
		CharacterControllerDesc(float rad, float hei, glm::vec3 pos, glm::quat rot)
			:radius(rad), height(hei), position(pos), rotation(rot)
		{

		}
		float radius;
		float height;
		glm::vec3 position;
		glm::quat rotation;

	};
}