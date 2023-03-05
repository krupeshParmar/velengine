#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
#include <string>

namespace vel
{
	class Material
	{
	public:
		bool IsCompiled = false;
		std::string Name = "";
		glm::vec4 Diffuse = glm::vec4(0.75);
		glm::vec4 Specular = glm::vec4(0.75);
		float Ambient = 1.0f;
		float Shininess = 0.5f;

		Ref<Texture2D> DiffuseTexture;
		Ref<Texture2D> NormalTexture;
		Ref<Texture2D> SpecularTexture;

		std::string DiffuseTexturePath = "";
		std::string NormalTexturePath = "";
		std::string SpecularTexturePath = "";
	};
}