#include "velpch.h"
#include "SaveSystem.h"
#include "vel/Core/Log.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "MeshRenderer.h"
#include "MaterialSystem.h"
#include "Entity.h"
#include "LightManager.h"

namespace vel
{
	struct SceneDataLoadInfo
	{
		std::string filename;
		Scene* scene;
		bool async;
	};

	/*
	bool SaveSceneFile(std::string filename, Ref<EntityManager> entityManager)
	{
		pugi::xml_document doc;
		pugi::xml_node saveNode = doc.append_child("scene");
		pugi::xml_node sceneNameNode = saveNode.append_child("name");
		sceneNameNode.append_child(pugi::node_pcdata).set_value(filename.c_str());
		std::vector<Ref<Entity>> entityList = entityManager->GetAllEntities();

		for (int i = 0; i < entityList.size(); i++)
		{
			Ref<Entity> entity = entityList.at(i);
			pugi::xml_node gameobjectNode = saveNode.append_child("entity");
			gameobjectNode.append_attribute("id").set_value(entity->GetID());
			pugi::xml_node nameNode = gameobjectNode.append_child("name");
			nameNode.append_child(pugi::node_pcdata).set_value(entity->name.c_str());
			pugi::xml_node enabledNode = gameobjectNode.append_child("enabled");

			if (entity->enabled)
				enabledNode.append_child(pugi::node_pcdata).set_value("1");
			else enabledNode.append_child(pugi::node_pcdata).set_value("0");

			pugi::xml_node componentNode = gameobjectNode.append_child("components");
			pugi::xml_node transformNode = componentNode.append_child("transform");
			
			TransformComponent* transform = entityManager->GetComponentByType<TransformComponent>(entity->GetID());
			if (transform) 
			{
				pugi::xml_node positionNode = transformNode.append_child("position");
				{
					pugi::xml_node xNode = positionNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Translation.x).c_str());
					pugi::xml_node yNode = positionNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Translation.y).c_str());
					pugi::xml_node zNode = positionNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Translation.z).c_str());
				}

				pugi::xml_node rotationNode = transformNode.append_child("rotation");
				{
					pugi::xml_node xNode = rotationNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Rotation.x).c_str());
					pugi::xml_node yNode = rotationNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Rotation.y).c_str());
					pugi::xml_node zNode = rotationNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Rotation.z).c_str());
				}

				pugi::xml_node scaleNode = transformNode.append_child("scale");
				{
					pugi::xml_node xNode = scaleNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Scale.x).c_str());
					pugi::xml_node yNode = scaleNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Scale.y).c_str());
					pugi::xml_node zNode = scaleNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform->Scale.z).c_str());
				}
			}

			LightComponent* lightComponent = entityManager->GetComponentByType<LightComponent>(entity->GetID());
			if (lightComponent)
			{
				pugi::xml_node lightNode = componentNode.append_child("light");

				pugi::xml_node lightEnabledNode = lightNode.append_child("enabled");

				if (lightComponent->Enabled)
					lightEnabledNode.append_child(pugi::node_pcdata).set_value("1");
				else
					lightEnabledNode.append_child(pugi::node_pcdata).set_value("0");

				pugi::xml_node lightIntensityNode = lightNode.append_child("intensity");
				lightIntensityNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Intensity).c_str());
				pugi::xml_node diffuseNode = lightNode.append_child("diffuse");
				{
					pugi::xml_node rNode = diffuseNode.append_child("r");
					rNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Diffuse.r).c_str());

					pugi::xml_node gNode = diffuseNode.append_child("g");
					gNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Diffuse.g).c_str());

					pugi::xml_node bNode = diffuseNode.append_child("b");
					bNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Diffuse.b).c_str());

					pugi::xml_node wNode = diffuseNode.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Diffuse.w).c_str());
				}
				pugi::xml_node specularNode = lightNode.append_child("specular");
				{
					pugi::xml_node rNode = specularNode.append_child("r");
					rNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Specular.r).c_str());

					pugi::xml_node gNode = specularNode.append_child("g");
					gNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Specular.g).c_str());

					pugi::xml_node bNode = specularNode.append_child("b");
					bNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Specular.b).c_str());

					pugi::xml_node wNode = specularNode.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Specular.w).c_str());
				}

				pugi::xml_node attenNode = lightNode.append_child("attenuation");
				{
					pugi::xml_node xNode = attenNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Attenuation.x).c_str());

					pugi::xml_node yNode = attenNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Attenuation.y).c_str());

					pugi::xml_node zNode = attenNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Attenuation.z).c_str());

					pugi::xml_node wNode = attenNode.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->Attenuation.w).c_str());
				}

				pugi::xml_node param1Node = lightNode.append_child("lightparams");
				{
					pugi::xml_node xNode = param1Node.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->LightParams.x).c_str());

					pugi::xml_node yNode = param1Node.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->LightParams.y).c_str());

					pugi::xml_node zNode = param1Node.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->LightParams.z).c_str());

					pugi::xml_node wNode = param1Node.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent->LightParams.w).c_str());
				}
			}

			MeshComponent* meshComponent = entityManager->GetComponentByType<MeshComponent>(entity->GetID());
			if (meshComponent)
			{
				pugi::xml_node meshObjectNode = componentNode.append_child("meshobject");
				pugi::xml_node meshObjectEnabledNode = meshObjectNode.append_child("enabled");

				if (meshComponent->Enabled)
					meshObjectEnabledNode.append_child(pugi::node_pcdata).set_value("1");
				else
					meshObjectEnabledNode.append_child(pugi::node_pcdata).set_value("0");

				pugi::xml_node meshObjectPathNode = meshObjectNode.append_child("path");
				meshObjectPathNode.append_child(pugi::node_pcdata).set_value(meshComponent->Path.c_str());

				pugi::xml_node meshObjectMaterialPathNode = meshObjectNode.append_child("material_path");
				meshObjectMaterialPathNode.append_child(pugi::node_pcdata).set_value(meshComponent->MaterialPath.c_str());

				pugi::xml_node use_fbx_texturesNode = meshObjectNode.append_child("use_fbx_textures");
				if(meshComponent->UseFBXTextures)
					use_fbx_texturesNode.append_child(pugi::node_pcdata).set_value("1");
				else
					use_fbx_texturesNode.append_child(pugi::node_pcdata).set_value("0");
				SaveMaterial(meshComponent->MaterialPath, meshComponent->MaterialIns);

			}
		}

		return doc.save_file((filename).c_str());
	}
	*/
	bool SaveMaterial(std::string materialPath, Ref<Material> material)
	{
		pugi::xml_document doc;
		pugi::xml_node materialNode = doc.append_child("material");
		pugi::xml_node materialNameNode = materialNode.append_child("name");
		materialNameNode.append_child(pugi::node_pcdata).set_value(
			material->Name.substr(material->Name.find_last_of('/') + 1, material->Name.size()).c_str());

		pugi::xml_node isTransparentNode = materialNode.append_child("transparent");

		if (material->IsTransparent)
		{
			isTransparentNode.append_child(pugi::node_pcdata).set_value("1");
		}
		else isTransparentNode.append_child(pugi::node_pcdata).set_value("0");

		pugi::xml_node materialTextureSizeNode = materialNode.append_child("texture_size");
		materialTextureSizeNode.append_child(pugi::node_pcdata).set_value(std::to_string(material->TextureSize).c_str());

		pugi::xml_node diffuseNode = materialNode.append_child("diffuse");
		{
			pugi::xml_node xNode = diffuseNode.append_child("r");
			xNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Diffuse.x).c_str());

			pugi::xml_node yNode = diffuseNode.append_child("g");
			yNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Diffuse.y).c_str());

			pugi::xml_node zNode = diffuseNode.append_child("b");
			zNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Diffuse.z).c_str());

			pugi::xml_node aNode = diffuseNode.append_child("a");
			aNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Diffuse.w).c_str());
		}

		pugi::xml_node specularNode = materialNode.append_child("specular");
		{
			pugi::xml_node xNode = specularNode.append_child("r");
			xNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Specular.x).c_str());

			pugi::xml_node yNode = specularNode.append_child("g");
			yNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Specular.y).c_str());

			pugi::xml_node zNode = specularNode.append_child("b");
			zNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Specular.z).c_str());

			pugi::xml_node aNode = specularNode.append_child("a");
			aNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Specular.w).c_str());
		}
		pugi::xml_node emissiveNode = materialNode.append_child("emissive");
		{
			pugi::xml_node xNode = emissiveNode.append_child("r");
			xNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Emissive.x).c_str());

			pugi::xml_node yNode = emissiveNode.append_child("g");
			yNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Emissive.y).c_str());

			pugi::xml_node zNode = emissiveNode.append_child("b");
			zNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Emissive.z).c_str());

			pugi::xml_node aNode = emissiveNode.append_child("a");
			aNode.append_child(pugi::node_pcdata).set_value(
				std::to_string(material->Emissive.w).c_str());
		}

		pugi::xml_node ambientNode = materialNode.append_child("ambient");
		ambientNode.append_child(pugi::node_pcdata).set_value(std::to_string(material->Ambient).c_str());

		pugi::xml_node shininessNode = materialNode.append_child("shininess");
		shininessNode.append_child(pugi::node_pcdata).set_value(std::to_string(material->Shininess).c_str());
		
		pugi::xml_node emissiveIntesityNode = materialNode.append_child("emissive_intensity");
		emissiveIntesityNode.append_child(pugi::node_pcdata).set_value(std::to_string(material->EmissiveIntensity).c_str());

		pugi::xml_node diffusetextureNode = materialNode.append_child("diffuse_texture");
		diffusetextureNode.append_child(pugi::node_pcdata).set_value(material->DiffuseTexturePath.c_str());

		pugi::xml_node speculartextureNode = materialNode.append_child("specular_texture");
		speculartextureNode.append_child(pugi::node_pcdata).set_value(material->SpecularTexturePath.c_str());

		pugi::xml_node normaltextureNode = materialNode.append_child("normal_texture");
		normaltextureNode.append_child(pugi::node_pcdata).set_value(material->NormalTexturePath.c_str());

		pugi::xml_node emissivetextureNode = materialNode.append_child("emissive_texture");
		emissivetextureNode.append_child(pugi::node_pcdata).set_value(material->EmissiveTexturePath.c_str());

		return doc.save_file(materialPath.c_str());
	}

	Ref<Material> GetMaterialFromSaveFile(std::string materialPath)
	{
		pugi::xml_document matDoc;
		if (!matDoc.load_file(materialPath.c_str()))
		{
			VEL_CORE_ERROR("Couldn't open " + materialPath);
			return nullptr;
		}
		pugi::xml_object_range<pugi::xml_node_iterator> matData = matDoc.child("material").children();
		if (matData.empty())
		{
			VEL_CORE_ERROR("Material has no data!");
			return nullptr;
		}
		pugi::xml_node_iterator matNodeIterator = matData.begin();
		Ref<Material> material = CreateRef<Material>();
		for (; matNodeIterator != matData.end(); matNodeIterator++)
		{
			pugi::xml_node matNode = *matNodeIterator;
			std::string matNodeName = matNode.name();
			if (matNodeName == "name")
			{
				material->Name = matNode.child_value();
			}
			if (matNodeName == "transparent")
			{
				if (std::string(matNode.child_value()) == "1")
					material->IsTransparent = true;
				else material->IsTransparent = false;
			}

			if (matNodeName == "texture_size")
			{
				material->TextureSize = std::stof(matNode.child_value());
			}

			if(matNodeName == "diffuse")
			{
				pugi::xml_object_range<pugi::xml_node_iterator>
					diffuseNodeChildren = matNode.children();
				for (pugi::xml_node_iterator diffuseNodeIterator = diffuseNodeChildren.begin();
					diffuseNodeIterator != diffuseNodeChildren.end();
					diffuseNodeIterator++)
				{
					pugi::xml_node diffuseNode = *diffuseNodeIterator;
					std::string diffuseNodeName = diffuseNode.name();
					if (diffuseNodeName == "r")
						material->Diffuse.x =
						std::stof(diffuseNode.child_value());

					if (diffuseNodeName == "g")
						material->Diffuse.y =
						std::stof(diffuseNode.child_value());

					if (diffuseNodeName == "b")
						material->Diffuse.z =
						std::stof(diffuseNode.child_value());

					if (diffuseNodeName == "a")
						material->Diffuse.w =
						std::stof(diffuseNode.child_value());
				}
			}
			if(matNodeName == "specular")
			{
				pugi::xml_object_range<pugi::xml_node_iterator>
					specularNodeChildren = matNode.children();
				for (pugi::xml_node_iterator specularNodeIterator = specularNodeChildren.begin();
					specularNodeIterator != specularNodeChildren.end();
					specularNodeIterator++)
				{
					pugi::xml_node specularNode = *specularNodeIterator;
					std::string specularNodeName = specularNode.name();
					if (specularNodeName == "r")
						material->Specular.x =
						std::stof(specularNode.child_value());

					if (specularNodeName == "g")
						material->Specular.y =
						std::stof(specularNode.child_value());

					if (specularNodeName == "b")
						material->Specular.z =
						std::stof(specularNode.child_value());

					if (specularNodeName == "a")
						material->Specular.w =
						std::stof(specularNode.child_value());
				}
			}
			if(matNodeName == "emissive")
			{
				pugi::xml_object_range<pugi::xml_node_iterator>
					emissiveChildren = matNode.children();
				for (pugi::xml_node_iterator emissiveIterator = emissiveChildren.begin();
					emissiveIterator != emissiveChildren.end();
					emissiveIterator++)
				{
					pugi::xml_node emissiveNode = *emissiveIterator;
					std::string emissiveNodeName = emissiveNode.name();
					if (emissiveNodeName == "r")
						material->Emissive.x =
						std::stof(emissiveNode.child_value());

					if (emissiveNodeName == "g")
						material->Emissive.y =
						std::stof(emissiveNode.child_value());

					if (emissiveNodeName == "b")
						material->Emissive.z =
						std::stof(emissiveNode.child_value());

					if (emissiveNodeName == "a")
						material->Emissive.w =
						std::stof(emissiveNode.child_value());
				}
			}
			if (matNodeName == "ambient")
			{
				material->Ambient = std::stof(matNode.child_value());
			}
			if (matNodeName == "shininess")
			{
				material->Shininess = std::stof(matNode.child_value());
			}
			if (matNodeName == "emissive_intensity")
			{
				material->EmissiveIntensity = std::stof(matNode.child_value());
			}
			if (matNodeName == "diffuse_texture")
			{
				material->DiffuseTexturePath = matNode.child_value();
			}
			if (matNodeName == "specular_texture")
			{
				material->SpecularTexturePath = matNode.child_value();
			}
			if (matNodeName == "normal_texture")
			{
				material->NormalTexturePath = matNode.child_value();
			}
			if (matNodeName == "emissive_texture")
			{
				material->EmissiveTexturePath = matNode.child_value();
			}
		}
		return material;
	}

	bool SaveSceneFile(std::string filename, Scene* scene)
	{
		pugi::xml_document doc;
		pugi::xml_node saveNode = doc.append_child("scene");
		pugi::xml_node sceneNameNode = saveNode.append_child("name");
		sceneNameNode.append_child(pugi::node_pcdata).set_value(filename.c_str());
		auto view = scene->GetAllEntitiesWith<IDComponent>();

		for (auto entityHandle : view)
		{
			Entity entity = Entity(entityHandle, scene);
			if (entity.GetComponent<IDComponent>().Type == IsAsset)
			{
				if (entity.HasComponent<MeshComponent>())
				{
					MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();
					SaveMaterial(meshComponent.Path, meshComponent.MaterialIns);
				}
				continue;
			}
			pugi::xml_node gameobjectNode = saveNode.append_child("entity");
			gameobjectNode.append_attribute("id").set_value(entity.GetGUID());
			pugi::xml_node nameNode = gameobjectNode.append_child("name");
			nameNode.append_child(pugi::node_pcdata).set_value(entity.Name().c_str());

			pugi::xml_node componentNode = gameobjectNode.append_child("components");
			pugi::xml_node transformNode = componentNode.append_child("transform");

			TransformComponent& transform = entity.Transform();
			//if (transform)
			{
				pugi::xml_node transformEnabledNode = transformNode.append_child("enabled");

				if (transform.enabled)
					transformEnabledNode.append_child(pugi::node_pcdata).set_value("1");
				else
					transformEnabledNode.append_child(pugi::node_pcdata).set_value("0");
				pugi::xml_node positionNode = transformNode.append_child("position");
				{
					pugi::xml_node xNode = positionNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.Translation.x).c_str());
					pugi::xml_node yNode = positionNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.Translation.y).c_str());
					pugi::xml_node zNode = positionNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.Translation.z).c_str());
				}

				pugi::xml_node rotationNode = transformNode.append_child("rotation");
				{
					pugi::xml_node xNode = rotationNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.RotationEuler.x).c_str());
					pugi::xml_node yNode = rotationNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.RotationEuler.y).c_str());
					pugi::xml_node zNode = rotationNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.RotationEuler.z).c_str());
				}

				pugi::xml_node scaleNode = transformNode.append_child("scale");
				{
					pugi::xml_node xNode = scaleNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.Scale.x).c_str());
					pugi::xml_node yNode = scaleNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.Scale.y).c_str());
					pugi::xml_node zNode = scaleNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(
						std::to_string(transform.Scale.z).c_str());
				}
			}

			if (entity.HasComponent<LightComponent>())
			{
				LightComponent lightComponent = entity.GetComponent<LightComponent>();
				pugi::xml_node lightNode = componentNode.append_child("light");

				pugi::xml_node lightEnabledNode = lightNode.append_child("enabled");

				if (lightComponent.Enabled)
					lightEnabledNode.append_child(pugi::node_pcdata).set_value("1");
				else
					lightEnabledNode.append_child(pugi::node_pcdata).set_value("0");

				pugi::xml_node lightIntensityNode = lightNode.append_child("intensity");
				lightIntensityNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Intensity).c_str());
				pugi::xml_node diffuseNode = lightNode.append_child("diffuse");
				{
					pugi::xml_node rNode = diffuseNode.append_child("r");
					rNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Diffuse.r).c_str());

					pugi::xml_node gNode = diffuseNode.append_child("g");
					gNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Diffuse.g).c_str());

					pugi::xml_node bNode = diffuseNode.append_child("b");
					bNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Diffuse.b).c_str());

					pugi::xml_node wNode = diffuseNode.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Diffuse.w).c_str());
				}
				pugi::xml_node specularNode = lightNode.append_child("specular");
				{
					pugi::xml_node rNode = specularNode.append_child("r");
					rNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Specular.r).c_str());

					pugi::xml_node gNode = specularNode.append_child("g");
					gNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Specular.g).c_str());

					pugi::xml_node bNode = specularNode.append_child("b");
					bNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Specular.b).c_str());

					pugi::xml_node wNode = specularNode.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Specular.w).c_str());
				}

				pugi::xml_node attenNode = lightNode.append_child("attenuation");
				{
					pugi::xml_node xNode = attenNode.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Attenuation.x).c_str());

					pugi::xml_node yNode = attenNode.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Attenuation.y).c_str());

					pugi::xml_node zNode = attenNode.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Attenuation.z).c_str());

					pugi::xml_node wNode = attenNode.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.Attenuation.w).c_str());
				}

				pugi::xml_node param1Node = lightNode.append_child("lightparams");
				{
					pugi::xml_node xNode = param1Node.append_child("x");
					xNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.LightParams.x).c_str());

					pugi::xml_node yNode = param1Node.append_child("y");
					yNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.LightParams.y).c_str());

					pugi::xml_node zNode = param1Node.append_child("z");
					zNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.LightParams.z).c_str());

					pugi::xml_node wNode = param1Node.append_child("w");
					wNode.append_child(pugi::node_pcdata).set_value(std::to_string(lightComponent.LightParams.w).c_str());
				}
			}

			if (entity.HasComponent<AssetComponent>())
			{
				AssetComponent assetComponent = entity.GetComponent<AssetComponent>();
				pugi::xml_node assetComponentNode = componentNode.append_child("asset_component");

				pugi::xml_node assetComponentPathNode = assetComponentNode.append_child("file_location");
				assetComponentPathNode.append_child(pugi::node_pcdata).set_value(assetComponent.FileLocation.c_str());
				
				/*Ref<Model> model = assetComponent.ModelInstance;
				if (model)
				{
					int break_me = 0;
				}*/

				for (Asset& asset : assetComponent.AssetHandle)
				{
					Entity assetEntity = scene->GetEntityWithGUID(asset.ID);

					pugi::xml_node assetNode = assetComponentNode.append_child("asset");
					pugi::xml_attribute idAttr = assetNode.append_attribute("id");
					idAttr.set_value(asset.AssetID);

					pugi::xml_node assetSComponentNode = assetNode.append_child("components");
					pugi::xml_node assetTransformNode = assetSComponentNode.append_child("transform");

					TransformComponent& assettransform = assetEntity.Transform();
					
					{
						pugi::xml_node transformEnabledNode = assetTransformNode.append_child("enabled");

						if (assettransform.enabled)
							transformEnabledNode.append_child(pugi::node_pcdata).set_value("1");
						else
							transformEnabledNode.append_child(pugi::node_pcdata).set_value("0");
						pugi::xml_node positionNode = assetTransformNode.append_child("position");
						{
							pugi::xml_node xNode = positionNode.append_child("x");
							xNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.Translation.x).c_str());
							pugi::xml_node yNode = positionNode.append_child("y");
							yNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.Translation.y).c_str());
							pugi::xml_node zNode = positionNode.append_child("z");
							zNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.Translation.z).c_str());
						}

						pugi::xml_node rotationNode = assetTransformNode.append_child("rotation");
						{
							pugi::xml_node xNode = rotationNode.append_child("x");
							xNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.RotationEuler.x).c_str());
							pugi::xml_node yNode = rotationNode.append_child("y");
							yNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.RotationEuler.y).c_str());
							pugi::xml_node zNode = rotationNode.append_child("z");
							zNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.RotationEuler.z).c_str());
						}

						pugi::xml_node scaleNode = assetTransformNode.append_child("scale");
						{
							pugi::xml_node xNode = scaleNode.append_child("x");
							xNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.Scale.x).c_str());
							pugi::xml_node yNode = scaleNode.append_child("y");
							yNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.Scale.y).c_str());
							pugi::xml_node zNode = scaleNode.append_child("z");
							zNode.append_child(pugi::node_pcdata).set_value(
								std::to_string(assettransform.Scale.z).c_str());
						}
					}

					if (assetEntity.HasComponent<MeshComponent>())
					{
						std::string materialPath = assetEntity.GetComponent<MeshComponent>().MaterialPath;

						if (!materialPath.empty())
						{
							pugi::xml_node assetMaterial = assetNode.append_child("material_path");
							assetMaterial.append_child(pugi::node_pcdata).set_value(materialPath.c_str());
						}
					}

					if (assetEntity.HasComponent<AnimatorComponent>())
					{
						AnimatorComponent& animator = assetEntity.GetComponent<AnimatorComponent>();

						if (!animator.List_Animations.empty())
						{
							for (Animation* animation : animator.List_Animations)
							{
								pugi::xml_node assetAnimation = assetNode.append_child("animation");
								assetAnimation.append_child(pugi::node_pcdata).set_value(animation->name.c_str());
							}
						}
					}

					for (GUID id : assetEntity.Children())
					{
						Entity childEntity = scene->GetEntityWithGUID(id);
						if (childEntity.HasComponent<IDComponent>())
						{
							IDComponent idComponent = childEntity.GetComponent<IDComponent>();
							if (idComponent.Type == NotAsset)
							{
								pugi::xml_node assetChild = assetNode.append_child("child");
								assetChild.append_child(pugi::node_pcdata).set_value(std::to_string(idComponent.ID).c_str());
							}
						}
					}
				}
			}

			if (entity.HasComponent<MeshComponent>())
			{
				MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();
				pugi::xml_node meshObjectNode = componentNode.append_child("meshobject");
				pugi::xml_node meshObjectEnabledNode = meshObjectNode.append_child("enabled");

				if (meshComponent.Enabled)
					meshObjectEnabledNode.append_child(pugi::node_pcdata).set_value("1");
				else
					meshObjectEnabledNode.append_child(pugi::node_pcdata).set_value("0");

				pugi::xml_node meshObjectPathNode = meshObjectNode.append_child("path");
				meshObjectPathNode.append_child(pugi::node_pcdata).set_value(meshComponent.Path.c_str());

				pugi::xml_node meshObjectMaterialPathNode = meshObjectNode.append_child("material_path");
				meshObjectMaterialPathNode.append_child(pugi::node_pcdata).set_value(meshComponent.MaterialPath.c_str());

				pugi::xml_node use_fbx_texturesNode = meshObjectNode.append_child("use_fbx_textures");
				if (meshComponent.UseFBXTextures)
					use_fbx_texturesNode.append_child(pugi::node_pcdata).set_value("1");
				else
					use_fbx_texturesNode.append_child(pugi::node_pcdata).set_value("0");
				SaveMaterial(meshComponent.MaterialPath, meshComponent.MaterialIns);

			}
		}

		return doc.save_file((filename).c_str());
	}
		
	bool LoadFileForReal(std::string filename, Scene* scene, bool async)
	{
		std::string materialPath = "";
		Ref<Material> material;
		pugi::xml_document sceneDoc;
		if (!sceneDoc.load_file(filename.c_str()))
		{
			VEL_CORE_ERROR("Couldn't open " + filename);
			return false;
		}

		pugi::xml_object_range<pugi::xml_node_iterator> sceneData = sceneDoc.child("scene").children();
		if (sceneData.empty())
		{
			VEL_CORE_ERROR("Scene has no data!");
			return false;
		}

		pugi::xml_node_iterator sceneNodeIterator = sceneData.begin();

		for (; sceneNodeIterator != sceneData.end(); sceneNodeIterator++)
		{
			pugi::xml_node sceneNode = *sceneNodeIterator;
			std::string sceneNodeName = sceneNode.name();

			if (sceneNodeName == "entity")
			{
				std::string idText = sceneNode.attribute("id").value();
				uint64_t id;
				/*if (!idText.empty())
				{
					id = (uint64_t) std::stoi(idText);
					scene->CreateEntityWithID(id);
				}
				else
					*/id = scene->CreateEntity().GetGUID();

				Entity* entity = &scene->GetEntityMap()[id];
				entity->SetScene(scene);
				pugi::xml_object_range<pugi::xml_node_iterator> gameObjectData = sceneNode.children();

				for (pugi::xml_node_iterator gameobjectsDataIterator = gameObjectData.begin();
					gameobjectsDataIterator != gameObjectData.end(); gameobjectsDataIterator++)
				{
					pugi::xml_node gameObjectNode = *gameobjectsDataIterator;
					std::string nodeName = gameObjectNode.name();
					if (nodeName == "name")
					{
						entity->Name() = gameObjectNode.child_value();
					}
					if (nodeName == "enabled")
					{
						std::string isEnabled = gameObjectNode.child_value();
						if (isEnabled == "1")
							entity->enabled = true;
						else entity->enabled = false;
					}
					if (nodeName == "components")
					{
						pugi::xml_object_range<pugi::xml_node_iterator>
							componentsNodesChildren = gameObjectNode.children();
						for (pugi::xml_node_iterator componentsNodeIterator = componentsNodesChildren.begin();
							componentsNodeIterator != componentsNodesChildren.end();
							componentsNodeIterator++)
						{
							pugi::xml_node componentNode = *componentsNodeIterator;
							std::string componentNodeName = componentNode.name();
							if (componentNodeName == "transform")
							{
								TransformComponent* transform = new TransformComponent();
								glm::vec3 rotation = glm::vec3(0.f);
								pugi::xml_object_range<pugi::xml_node_iterator>
									transformNodeChildren = componentNode.children();
								for (pugi::xml_node_iterator transformNodeIterator = transformNodeChildren.begin();
									transformNodeIterator != transformNodeChildren.end();
									transformNodeIterator++)
								{
									pugi::xml_node transformNode = *transformNodeIterator;
									std::string transformNodeName = transformNode.name();
									if (transformNodeName == "enabled")
									{
										std::string isEnabled = transformNode.child_value();
										if (isEnabled == "1")
											transform->enabled = true;
										else transform->enabled = false;
									}
									if (transformNodeName == "position")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											positionNodeChildren = transformNode.children();
										for (pugi::xml_node_iterator positionNodeIterator = positionNodeChildren.begin();
											positionNodeIterator != positionNodeChildren.end();
											positionNodeIterator++)
										{
											pugi::xml_node positionNode = *positionNodeIterator;
											std::string positionNodeName = positionNode.name();
											if (positionNodeName == "x")
												transform->Translation.x =
												std::stof(positionNode.child_value());
											if (positionNodeName == "y")
												transform->Translation.y =
												std::stof(positionNode.child_value());
											if (positionNodeName == "z")
												transform->Translation.z =
												std::stof(positionNode.child_value());
										}
									}

									if (transformNodeName == "rotation")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											rotationNodeChildren = transformNode.children();
										for (pugi::xml_node_iterator rotationNodeIterator =
											rotationNodeChildren.begin();
											rotationNodeIterator != rotationNodeChildren.end();
											rotationNodeIterator++)
										{
											pugi::xml_node rotationNode = *rotationNodeIterator;
											std::string rotationNodeName = rotationNode.name();

											if (rotationNodeName == "x")
												rotation.x =
												std::stof(rotationNode.child_value());
											if (rotationNodeName == "y")
												rotation.y =
												std::stof(rotationNode.child_value());
											if (rotationNodeName == "z")
												rotation.z =
												std::stof(rotationNode.child_value());
										}
									}
									if (transformNodeName == "scale")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											scaleNodeChildren = transformNode.children();
										for (pugi::xml_node_iterator scaleNodeIterator = scaleNodeChildren.begin();
											scaleNodeIterator != scaleNodeChildren.end();
											scaleNodeIterator++)
										{
											pugi::xml_node scaleNode = *scaleNodeIterator;
											std::string scaleNodeName = scaleNode.name();
											if (scaleNodeName == "x")
												transform->Scale.x =
												std::stof(scaleNode.child_value());
											if (scaleNodeName == "y")
												transform->Scale.y =
												std::stof(scaleNode.child_value());
											if (scaleNodeName == "z")
												transform->Scale.z =
												std::stof(scaleNode.child_value());
										}
									}
								}
								
								transform->SetRotationEuler(rotation);
								entity->Transform() = *transform;
								delete transform;
							}

							if (componentNodeName == "light")
							{
								LightComponent* light = new LightComponent();
								pugi::xml_object_range<pugi::xml_node_iterator>
									lightNodeChildren = componentNode.children();
								for (pugi::xml_node_iterator lightNodeIterator = lightNodeChildren.begin();
									lightNodeIterator != lightNodeChildren.end();
									lightNodeIterator++)
								{
									pugi::xml_node lightNode = *lightNodeIterator;
									std::string lightNodeName = lightNode.name();
									if (lightNodeName == "enabled")
									{
										std::string isEnabled = lightNode.child_value();
										if (isEnabled == "1")
											light->Enabled = true;
										else light->Enabled = false;
									}
									if (lightNodeName == "intensity")
									{
										light->Intensity =
											std::stof(lightNode.child_value());
									}
									if (lightNodeName == "diffuse")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											diffuseNodeChildren = lightNode.children();
										for (pugi::xml_node_iterator diffuseNodeIterator = diffuseNodeChildren.begin();
											diffuseNodeIterator != diffuseNodeChildren.end();
											diffuseNodeIterator++)
										{
											pugi::xml_node diffuseNode = *diffuseNodeIterator;
											std::string diffuseNodeName = diffuseNode.name();
											if (diffuseNodeName == "r")
												light->Diffuse.x =
												std::stof(diffuseNode.child_value());

											if (diffuseNodeName == "g")
												light->Diffuse.y =
												std::stof(diffuseNode.child_value());

											if (diffuseNodeName == "b")
												light->Diffuse.z =
												std::stof(diffuseNode.child_value());

											if (diffuseNodeName == "w")
												light->Diffuse.w =
												std::stof(diffuseNode.child_value());
										}
									}
									if (lightNodeName == "specular")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											sepcularNodeChildren = lightNode.children();
										for (pugi::xml_node_iterator sepcularNodeIterator = sepcularNodeChildren.begin();
											sepcularNodeIterator != sepcularNodeChildren.end();
											sepcularNodeIterator++)
										{
											pugi::xml_node sepcularNode = *sepcularNodeIterator;
											std::string sepcularNodeName = sepcularNode.name();
											if (sepcularNodeName == "r")
												light->Specular.x =
												std::stof(sepcularNode.child_value());

											if (sepcularNodeName == "g")
												light->Specular.y =
												std::stof(sepcularNode.child_value());

											if (sepcularNodeName == "b")
												light->Specular.z =
												std::stof(sepcularNode.child_value());

											if (sepcularNodeName == "w")
												light->Specular.w =
												std::stof(sepcularNode.child_value());
										}
									}
									if (lightNodeName == "attenuation")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											attenNodeChildren = lightNode.children();
										for (pugi::xml_node_iterator attenNodeIterator = attenNodeChildren.begin();
											attenNodeIterator != attenNodeChildren.end();
											attenNodeIterator++)
										{
											pugi::xml_node attenNode = *attenNodeIterator;
											std::string attenNodeName = attenNode.name();
											if (attenNodeName == "x")
												light->Attenuation.x =
												std::stof(attenNode.child_value());

											if (attenNodeName == "y")
												light->Attenuation.y =
												std::stof(attenNode.child_value());

											if (attenNodeName == "z")
												light->Attenuation.z =
												std::stof(attenNode.child_value());

											if (attenNodeName == "w")
												light->Attenuation.w =
												std::stof(attenNode.child_value());
										}
									}
									if (lightNodeName == "lightparams")
									{
										pugi::xml_object_range<pugi::xml_node_iterator>
											param1NodeChildren = lightNode.children();
										for (pugi::xml_node_iterator param1NodeIterator = param1NodeChildren.begin();
											param1NodeIterator != param1NodeChildren.end();
											param1NodeIterator++)
										{
											pugi::xml_node param1Node = *param1NodeIterator;
											std::string param1NodeName = param1Node.name();
											if (param1NodeName == "x")
												light->LightParams.x =
												std::stof(param1Node.child_value());

											if (param1NodeName == "y")
												light->LightParams.y =
												std::stof(param1Node.child_value());

											if (param1NodeName == "z")
												light->LightParams.z =
												std::stof(param1Node.child_value());

											if (param1NodeName == "w")
												light->LightParams.w =
												std::stof(param1Node.child_value());
										}
									}
								}

								entity->AddComponent<LightComponent>(*light);
								LightManager::AddNewLightInfo(&entity->GetComponent<LightComponent>());
							}

							if (componentNodeName == "asset_component")
							{
								pugi::xml_object_range<pugi::xml_node_iterator>
									assetComponentNodesChildren = componentNode.children();
								std::map<GUID, Asset> assetsData;
								std::string assetFileLocation;
								for (pugi::xml_node_iterator assetComponentIterator = componentNode.children().begin();
									assetComponentIterator != componentNode.children().end();
									assetComponentIterator++)
								{
									pugi::xml_node assetNode = *assetComponentIterator;
									std::string assetNodeName = assetNode.name();
									if (assetNodeName == "file_location")
									{
										assetFileLocation = assetNode.child_value();
										assetsData = LoadAssetFile(assetFileLocation);
									}
									if (assetNodeName == "asset")
									{
										GUID id = std::stoull(assetNode.attribute("id").value());
										for (pugi::xml_node_iterator assetNodeIterator = assetNode.children().begin();
											assetNodeIterator != assetNode.children().end();
											assetNodeIterator++)
										{
											pugi::xml_node assetDataNode = *assetNodeIterator;
											std::string assetDataNodeName = assetDataNode.name();

											if (assetDataNodeName == "components")
											{
												for (pugi::xml_node_iterator assetCompoentIterator = assetDataNode.children().begin();
													assetCompoentIterator != assetDataNode.children().end();
													assetCompoentIterator++)
												{
													pugi::xml_node assetCompoentNode = *assetCompoentIterator;
													std::string assetCompoentNodeName = assetCompoentNode.name();
													if(assetCompoentNodeName == "transform")
													{ 
														TransformComponent* transform = new TransformComponent();
														glm::vec3 rotation = glm::vec3(0.f);
														pugi::xml_object_range<pugi::xml_node_iterator>
															transformNodeChildren = assetCompoentNode.children();
														for (pugi::xml_node_iterator transformNodeIterator = transformNodeChildren.begin();
															transformNodeIterator != transformNodeChildren.end();
															transformNodeIterator++)
														{
															pugi::xml_node transformNode = *transformNodeIterator;
															std::string transformNodeName = transformNode.name();
															if (transformNodeName == "enabled")
															{
																std::string isEnabled = transformNode.child_value();
																if (isEnabled == "1")
																	transform->enabled = true;
																else transform->enabled = false;
															}
															if (transformNodeName == "position")
															{
																pugi::xml_object_range<pugi::xml_node_iterator>
																	positionNodeChildren = transformNode.children();
																for (pugi::xml_node_iterator positionNodeIterator = positionNodeChildren.begin();
																	positionNodeIterator != positionNodeChildren.end();
																	positionNodeIterator++)
																{
																	pugi::xml_node positionNode = *positionNodeIterator;
																	std::string positionNodeName = positionNode.name();
																	if (positionNodeName == "x")
																		transform->Translation.x =
																		std::stof(positionNode.child_value());
																	if (positionNodeName == "y")
																		transform->Translation.y =
																		std::stof(positionNode.child_value());
																	if (positionNodeName == "z")
																		transform->Translation.z =
																		std::stof(positionNode.child_value());
																}
															}

															if (transformNodeName == "rotation")
															{
																pugi::xml_object_range<pugi::xml_node_iterator>
																	rotationNodeChildren = transformNode.children();
																for (pugi::xml_node_iterator rotationNodeIterator =
																	rotationNodeChildren.begin();
																	rotationNodeIterator != rotationNodeChildren.end();
																	rotationNodeIterator++)
																{
																	pugi::xml_node rotationNode = *rotationNodeIterator;
																	std::string rotationNodeName = rotationNode.name();

																	if (rotationNodeName == "x")
																		rotation.x =
																		std::stof(rotationNode.child_value());
																	if (rotationNodeName == "y")
																		rotation.y =
																		std::stof(rotationNode.child_value());
																	if (rotationNodeName == "z")
																		rotation.z =
																		std::stof(rotationNode.child_value());
																}
															}
															if (transformNodeName == "scale")
															{
																pugi::xml_object_range<pugi::xml_node_iterator>
																	scaleNodeChildren = transformNode.children();
																for (pugi::xml_node_iterator scaleNodeIterator = scaleNodeChildren.begin();
																	scaleNodeIterator != scaleNodeChildren.end();
																	scaleNodeIterator++)
																{
																	pugi::xml_node scaleNode = *scaleNodeIterator;
																	std::string scaleNodeName = scaleNode.name();
																	if (scaleNodeName == "x")
																		transform->Scale.x =
																		std::stof(scaleNode.child_value());
																	if (scaleNodeName == "y")
																		transform->Scale.y =
																		std::stof(scaleNode.child_value());
																	if (scaleNodeName == "z")
																		transform->Scale.z =
																		std::stof(scaleNode.child_value());
																}
															}
														}

														transform->SetRotationEuler(rotation);
														std::map<GUID, Asset>::iterator assMapIT = assetsData.find(GUID(id));
														if (assMapIT != assetsData.end())
															assMapIT->second.transform = *transform;
														delete transform;
													}
												}
											}

											if (assetDataNodeName == "animation")
											{
												std::map<GUID, Asset>::iterator assMapIT = assetsData.find(GUID(id));
												if (assMapIT != assetsData.end())
												{
													assMapIT->second.AnimationsList.push_back(assetDataNode.child_value());
												}
											}
											if (assetDataNodeName == "material_path")
											{
												std::map<GUID, Asset>::iterator assMapIT = assetsData.find(GUID(id));
												if (assMapIT != assetsData.end())
												{
													assMapIT->second.MaterialLocation = assetDataNode.child_value();
													MaterialSystem::LoadMaterial(assMapIT->second.MaterialLocation, GetMaterialFromSaveFile(assMapIT->second.MaterialLocation));
												}
											}
										}
									}
								}

								ModelLoadData data;
								data.gameScene = scene;
								data.useTextures = false;
								data.loadAsync = async;
								data.source = assetFileLocation.substr(0, assetFileLocation.size() - 7);
								data.parentEntity = entity;
								data.assetData = assetsData;
								MeshRenderer::LoadMesh(data);
							}

							if (componentNodeName == "meshobject")
							{
								//continue;
								MeshComponent* meshObject = new MeshComponent();
								for (pugi::xml_node_iterator meshNodeIterator = componentNode.children().begin();
									meshNodeIterator != componentNode.children().end();
									meshNodeIterator++)
								{
									pugi::xml_node meshNode = *meshNodeIterator;
									std::string meshNodeName = meshNode.name();
									if (meshNodeName == "enabled")
									{
										std::string isEnabled = meshNode.child_value();
										if (isEnabled == "1")
											meshObject->Enabled = true;
										else meshObject->Enabled = false;
									}
									if (meshNodeName == "path")
									{
										meshObject->Path = meshNode.child_value();
									}
									if (meshNodeName == "material_path")
									{
										meshObject->MaterialPath = meshNode.child_value();
										if (meshObject->MaterialPath.empty())
											meshObject->MaterialIns = CreateRef<Material>();
										else
										{
											Ref<Material> mat = MaterialSystem::GetMaterial(meshObject->MaterialPath);

											if (mat)
												meshObject->MaterialIns = mat;
											else 
												meshObject->MaterialIns = MaterialSystem::LoadMaterial(meshObject->MaterialPath, GetMaterialFromSaveFile(meshObject->MaterialPath));
										}

									}
									if (meshNodeName == "use_fbx_textures")
									{
										meshObject->UseFBXTextures = meshNode.child_value() == "1";
									}
								}
								if (!meshObject->Path.empty())
								{ 
									ModelLoadData data;
									data.gameScene = scene;
									data.useTextures = meshObject->UseFBXTextures;
									data.loadAsync = async;
									data.source = meshObject->Path;
									data.material = meshObject->MaterialIns;
									data.materialPath = meshObject->MaterialPath;
									data.parentEntity = entity;
									std::map<GUID, Asset> assetsData = LoadAssetFile(meshObject->Path+".vasset");
									//data.assetData = assetsData;
									Entity* assetEntity = MeshRenderer::LoadMesh(data);
								}
								//entity->AddComponent<MeshComponent>(*meshObject);
							}
						}
					}
				}

			}

		}
		return true;
	}

	DWORD WINAPI LoadSceneFileAsync(LPVOID pParameters)
	{
		SceneDataLoadInfo* pFileParams = (SceneDataLoadInfo*)pParameters;

		LoadFileForReal(pFileParams->filename, pFileParams->scene, pFileParams->async);
		return 0;
	}
	
	bool LoadSceneFile(std::string filename, Scene* scene, bool async)
	{
		if (async)
		{
			SceneDataLoadInfo* fileInfo = new SceneDataLoadInfo();
			fileInfo->filename = filename;
			fileInfo->scene = scene;
			fileInfo->async = async;

			LPDWORD lpThreadId = 0;

			HANDLE hSceneLoader =
				CreateThread(
					NULL,
					0,
					LoadSceneFileAsync,
					(void*)fileInfo,
					0,
					lpThreadId
				);
			return true;
		}

		return LoadFileForReal(filename, scene, async);
	}
	bool SaveAssetFile(GUID assetID, std::string filename, std::vector<Asset> assets)
	{
		pugi::xml_document doc;
		pugi::xml_node assetListNode = doc.append_child("assets_list");
		pugi::xml_attribute idNode = assetListNode.append_attribute("id");
		idNode.set_value(assetID);
		for (int i = 0; i < assets.size(); i++)
		{
			pugi::xml_node assetNode = assetListNode.append_child("asset");
			pugi::xml_node assetNameNode = assetNode.append_child("name");
			assetNameNode.append_child(pugi::node_pcdata).set_value(assets[i].Name.c_str());

			pugi::xml_node assetIDNode = assetNode.append_child("id");
			assetIDNode.append_child(pugi::node_pcdata).set_value(std::to_string(assets[i].AssetID).c_str());
		}

		return doc.save_file(filename.c_str());
	}
	std::map<GUID,Asset> LoadAssetFile(std::string filename)
	{
		std::map<GUID, Asset> assets;
		pugi::xml_document assetDoc;
		if (!assetDoc.load_file(filename.c_str()))
		{
			VEL_CORE_ERROR("Couldn't open " + filename);
			return assets;
		}
		pugi::xml_object_range<pugi::xml_node_iterator> assetsList = assetDoc.child("assets_list").children();
		if (assetsList.empty())
		{
			VEL_CORE_ERROR("Material has no data!");
			return assets;
		}
		pugi::xml_node_iterator assetIterator = assetsList.begin();
		for (; assetIterator != assetsList.end(); assetIterator++)
		{
			pugi::xml_node assetNode = *assetIterator;
			std::string assetNodeName = assetNode.name();

			if (assetNodeName == "asset")
			{
				Asset asset;
				for (pugi::xml_node_iterator assetNodeIterator = assetNode.begin();
					assetNodeIterator != assetNode.end();
					assetNodeIterator++)
				{
					pugi::xml_node assetDataNode = *assetNodeIterator;
					std::string assetDataNodeName = assetDataNode.name();

					if (assetDataNodeName == "name")
					{
						asset.Name = assetDataNode.child_value();
					}
					if (assetDataNodeName == "id")
					{
						asset.ID = std::stoull(assetDataNode.child_value());
					}
				}
				assets.emplace(asset.ID, asset);
			}
		}
		return assets;
	}
}