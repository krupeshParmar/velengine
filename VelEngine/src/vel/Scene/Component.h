#pragma once
#include <string>
#include <vel/Renderer/Material.h>
#include "vel/Renderer/Model.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "vel/Renderer/Texture.h"
#include "vel/Core/GUID.h"
#include "vel/Math/Math.h"

namespace vel
{
	enum EntityType
	{
		NotAsset,
		IsAsset,
	};
	struct IDComponent
	{
		GUID ID = 0;
		GUID AssetID;
		EntityType Type = EntityType::NotAsset;
	};

	struct AssetComponent
	{
		AssetComponent(std::string fileLocation, Ref<Model> modelInstance)
			:FileLocation(fileLocation), ModelInstance(modelInstance)
		{

		}
		std::string FileLocation;
		Ref<Model> ModelInstance;
	};

	struct Asset
	{
		Asset() {};
		Asset(std::string name, GUID id, GUID assetID, std::string materialLocation) : Name(name), ID(id), AssetID(assetID), MaterialLocation(materialLocation){}
		std::string Name;
		GUID ID = 0;
		GUID AssetID = 0;
		std::string MaterialLocation;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct RelationshipComponent
	{
		GUID ParentHandle = 0;
		std::vector<GUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(GUID parent)
			: ParentHandle(parent) {}
	};

	struct TransformComponent
	{
	public:
		bool enabled = true;
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	private:
		// Should not be accessed directly
		// Only use functions
		glm::quat Rotation = { 1.f, 0.0f, 0.0f, 0.0f };

	public:
		glm::vec3 RotationEuler = { 0.0f, 0.0f, 0.0f };
		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) 
			:Translation(other.Translation), Rotation(other.Rotation), RotationEuler(other.RotationEuler), Scale(other.Scale)
		{}
		TransformComponent(const glm::vec3& translation)
			: Translation(translation){}

		glm::vec3 GetRotationInRad() const
		{
			return glm::vec3(
				glm::radians(RotationEuler.x),
				glm::radians(RotationEuler.y),
				glm::radians(RotationEuler.z)
			);
		}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(
					GetRotationInRad()
					)
				)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		void SetTransform(const glm::mat4& transform)
		{
			Math::DecomposeTransform(transform, Translation, Rotation, Scale);
			RotationEuler = glm::eulerAngles(Rotation);
		}

		glm::vec3 GetRotationEuler() const
		{
			return RotationEuler;
		}

		void SetRotationEuler(const glm::vec3& euler)
		{
			RotationEuler = glm::vec3(euler);
			Rotation = glm::quat(RotationEuler);
		}

		glm::quat GetRotation() const
		{
			return Rotation;
		}

		void SetRotation(const glm::quat& quat)
		{
			Rotation = quat;
			RotationEuler = glm::eulerAngles(Rotation);
		}
	};
	struct MeshData
	{
		std::vector<Vertices> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Ref<Texture2D>> m_Textures;

		bool m_Loaded = false;

		Ref<VertexArray> m_VertexArray;
	};

	struct MeshComponent
	{
		Ref<GUID> ID;
		std::string Mesh;
		std::string Path;
		bool UseFBXTextures;
		bool Enabled = true;


		std::string MaterialPath;
		Ref<Material> MaterialIns;
		Ref<Shader> shader;
		Ref<Model> ModelIns;
		uint32_t SubmeshIndex = 0;
		Ref<MeshData> MeshDrawData;

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
			: Mesh(other.Mesh),Path(other.Path), SubmeshIndex(other.SubmeshIndex), MaterialIns(other.MaterialIns), ModelIns(other.ModelIns), UseFBXTextures(other.UseFBXTextures), MaterialPath(other.MaterialPath)
		{
		}
	};

	enum class LightTypes
	{
		Point = 0,
		Spot = 1,
		Direct = 2
	};

	struct LightComponent
	{
		LightComponent(const LightComponent& other)
			: Position(other.Position),
			Diffuse(other.Diffuse),
			Specular(other.Specular),
			Ambient(other.Ambient),
			Attenuation(other.Attenuation),
			Direction(other.Direction),
			LightParams(other.LightParams),
			Intensity(other.Intensity),
			InnerCutOff(other.InnerCutOff),
			OuterCutOff(other.OuterCutOff)
		{

		}
		LightComponent()
		{
			Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			Specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			Attenuation = glm::vec4(0.01f, 0.01f, 0.0f, 1.0f);
			Direction = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
			LightParams = glm::vec4(2.f, 0.0f, 0.0f, 1.0f);
		}
		LightTypes LightType;
		glm::vec4 Position;
		glm::vec4 Diffuse;
		glm::vec4 Specular;
		glm::vec4 Ambient;
		glm::vec4 Attenuation;
		glm::vec4 Direction;
		glm::vec4 LightParams;	 // x = lightType, innerAngle, outerAngle, ON or OFF

		float Intensity = 1.0f;

		uint32_t ID;
		bool Enabled = true;


		void SetConstantAttenuation(float newConstAtten)
		{
			this->Attenuation.x = newConstAtten;
			return;
		}

		void SetLinearAttenuation(float newLinearAtten)
		{
			this->Attenuation.y = newLinearAtten;
			return;
		}

		void SetQuadraticAttenuation(float newQuadraticAtten)
		{
			this->Attenuation.z = newQuadraticAtten;
			return;
		}

		void TurnON(void)
		{
			this->LightParams.w = 1;
		}

		void TurnOFF(void)
		{
			this->LightParams.w = 0;
		}

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

	struct Volume
	{
		Bloom bloom;
		DepthOfField depthOfField;
		bool Enabled = true;
	};

	struct SkyBox
	{
		SkyBox() = default;
		SkyBox(std::vector<std::string> texturePaths, std::string modelPath)
		{
			CreateSkyBox(texturePaths, modelPath);
		}

		~SkyBox() {
			skyboxTexture->RemoveData();
		}

		void CreateSkyBox(std::vector<std::string> texturePaths, std::string modelPath)
		{
			skyboxTexture = TextureCubeMap::Create(texturePaths);

			cubeModel = CreateRef<Model>(modelPath, false, false, nullptr);
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