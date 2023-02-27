#pragma once
#include <string>
#include <vel/Renderer/Material.h>

namespace vel
{

	class Component
	{
	public:
		Component() {}
		virtual ~Component() = default;
	};
	class TransformComponent : public Component
	{
	public:
		virtual ~TransformComponent() {}
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	class MeshComponent : public Component
	{
		virtual ~MeshComponent() {}
		std::string Mesh;
		std::string Path;
		std::string MaterialPath;
		Ref<Material> MaterialIns;
		uint32_t SubmeshIndex = 0;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& other)
			: Mesh(other.Mesh), SubmeshIndex(other.SubmeshIndex), MaterialIns(other.MaterialIns)
		{
		}
	};

	enum class LightTypes
	{
		Point,
		Spot,
		Direct
	};

	class LightComponent : public Component
	{
		virtual ~LightComponent() {}
		LightTypes LightType;
		glm::vec3 Diffuse;
		glm::vec3 Specular;
		glm::vec3 Ambient;

		float Constant;
		float Linear;
		float Quadratic;

		float InnerCutOff;
		float OuterCutOff;

	};
}