#pragma once
#include <string>
#include <vel/Renderer/Material.h>
#include "vel/Renderer/Model.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "vel/Renderer/Texture.h"

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

		glm::vec3 GetRotation() const
		{
			return glm::vec3(
				glm::radians(Rotation.x),
				glm::radians(Rotation.y),
				glm::radians(Rotation.z)
			);
		}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(
						GetRotation()
					)
				)
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	class MeshComponent : public Component
	{
	public:
		virtual ~MeshComponent() {}
		std::string Mesh;
		std::string Path;
		bool UseFBXTextures;
		bool Enabled = true;

		std::string MaterialPath;
		Ref<Material> MaterialIns;
		Ref<Model> ModelIns;
		uint32_t SubmeshIndex = 0;

		MeshComponent()
		{
			MaterialIns = CreateRef<Material>();
		}		
		MeshComponent(std::string& path, bool useFBXTextures)
			:Path(path), UseFBXTextures(useFBXTextures)
		{
			MaterialIns = CreateRef<Material>();
			ModelIns = CreateRef<Model>(Path, useFBXTextures, false);
		}
		MeshComponent(const MeshComponent& other)
			: Mesh(other.Mesh), SubmeshIndex(other.SubmeshIndex), MaterialIns(other.MaterialIns)
		{
		}
	};

	enum class LightTypes
	{
		Point = 0,
		Spot = 1,
		Direct = 2
	};

	class LightComponent : public Component
	{
	public:
		LightComponent()
		{
			Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			Specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			Attenuation = glm::vec4(0.01f, 0.01f, 0.0f, 1.0f);
			Direction = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
			LightParams = glm::vec4(2.f, 0.0f, 0.0f, 1.0f);
		}
		virtual ~LightComponent() {}
		LightTypes LightType;
		glm::vec4 Position;
		glm::vec4 Diffuse;
		glm::vec4 Specular;
		glm::vec4 Ambient;
		glm::vec4 Attenuation;
		glm::vec4 Direction;
		glm::vec4 LightParams;	 // x = lightType, innerAngle, outerAngle, ON or OFF

		uint32_t ID;
		bool Enabled = true;


		inline void SetConstantAttenuation(float newConstAtten)
		{
			this->Attenuation.x = newConstAtten;
			return;
		}

		inline void SetLinearAttenuation(float newLinearAtten)
		{
			this->Attenuation.y = newLinearAtten;
			return;
		}

		inline void SetQuadraticAttenuation(float newQuadraticAtten)
		{
			this->Attenuation.z = newQuadraticAtten;
			return;
		}

		inline void TurnON(void)
		{
			this->LightParams.w = 1;
		}

		inline void TurnOFF(void)
		{
			this->LightParams.w = 0;
		}

		float Constant;
		float Linear;
		float Quadratic;

		float InnerCutOff;
		float OuterCutOff;

	};

	struct Bloom
	{
		float Thresold;
		float Intensity;

	};

	struct DepthOfField
	{
		enum DOFType
		{
			Off,
			Gaussian,
			Bokeh
		};
		DOFType Type;
		float Start;
		float End;
		float Radius;
		const float MAX = 1.5f;
		const float MIN = 0.5f;

		float FocusDistance = 10.0f;
		float FocalLength = 50.0;
		float Aperture = 5.6;
		bool Enabled = true;
	};

	class Volume : public Component
	{
	public:
		virtual ~Volume() {}

		Bloom bloom;
		DepthOfField depthOfField;
		bool Enabled = true;
	};

	class SkyBox : public Component
	{
	public:
		SkyBox() = default;
		SkyBox(std::vector<std::string> texturePaths, std::string modelPath)
		{
			CreateSkyBox(texturePaths, modelPath);
		}

		virtual ~SkyBox() {
			skyboxTexture->RemoveData();
		}

		void CreateSkyBox(std::vector<std::string> texturePaths, std::string modelPath)
		{
			skyboxTexture = TextureCubeMap::Create(texturePaths);

			cubeModel = CreateRef<Model>(modelPath, false, false);
			skyBox = VertexArray::Create();
			Ref<VertexBuffer> vb = VertexBuffer::Create(&skyboxVertices[0], 108);
			vb->SetLayout({

				{ vel::ShaderDataType::Float4, "vPosition"},
				});
			skyBox->AddVertexBuffer(vb);
		}

		Ref<TextureCubeMap> skyboxTexture;
		Ref<Model> cubeModel;
		Ref<VertexArray> skyBox;
		float skyboxVertices[108] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

	};
}