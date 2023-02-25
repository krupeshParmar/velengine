#pragma once
#include <glm/glm.hpp>
#include "Texture.h"

namespace vel
{
	class Material
	{
	public:

		glm::vec3 Diffuse;
		glm::vec3 Normal;
		glm::vec3 Specular;
		glm::vec3 ambient;
		float shininess;

		Ref<Texture2D> DiffuseTexture;
		Ref<Texture2D> NormalTexture;
		Ref<Texture2D> SpecularTexture;

		std::string DiffuseTexturePath;
		std::string NormalTexturePath;
		std::string SpecularTexturePath;
	};
}