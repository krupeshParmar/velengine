#include "velpch.h"
#include "SaveSystem.h"
#include "vel/Core/Log.h"

namespace vel
{
	bool SaveSceneFile(std::string filename, Ref<EntityManager> entityManager)
	{
		pugi::xml_document doc;
		pugi::xml_node saveNode = doc.append_child("scene");
		pugi::xml_node sceneNameNode = saveNode.append_child("name");
		sceneNameNode.append_child(pugi::node_pcdata).set_value(filename.c_str());
		Ref<std::vector<Entity*>> entityList = entityManager->GetAllEntities();

		for (int i = 0; i < entityList->size(); i++)
		{
			Entity* entity = entityList->at(i);
			pugi::xml_node gameobjectNode = saveNode.append_child("entity");
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

	bool SaveMaterial(std::string materialPath, Ref<Material> material)
	{
		pugi::xml_document doc;
		pugi::xml_node materialNode = doc.append_child("material");
		pugi::xml_node materialNameNode = materialNode.append_child("name");
		materialNameNode.append_child(pugi::node_pcdata).set_value(
			material->Name.substr(material->Name.find_last_of('/') + 1, material->Name.size()).c_str());

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

		pugi::xml_node ambientNode = materialNode.append_child("ambient");
		ambientNode.append_child(pugi::node_pcdata).set_value(std::to_string(material->Ambient).c_str());

		pugi::xml_node shininessNode = materialNode.append_child("shininess");
		shininessNode.append_child(pugi::node_pcdata).set_value(std::to_string(material->Shininess).c_str());

		pugi::xml_node diffusetextureNode = materialNode.append_child("diffuse_texture");
		diffusetextureNode.append_child(pugi::node_pcdata).set_value(material->DiffuseTexturePath.c_str());

		pugi::xml_node speculartextureNode = materialNode.append_child("specular_texture");
		speculartextureNode.append_child(pugi::node_pcdata).set_value(material->SpecularTexturePath.c_str());

		pugi::xml_node normaltextureNode = materialNode.append_child("normal_texture");
		normaltextureNode.append_child(pugi::node_pcdata).set_value(material->NormalTexturePath.c_str());

		return doc.save_file(materialPath.c_str());
	}

	Ref<Material> LoadMaterial(std::string materialPath)
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
						material->Diffuse.x =
						std::stof(specularNode.child_value());

					if (specularNodeName == "g")
						material->Diffuse.y =
						std::stof(specularNode.child_value());

					if (specularNodeName == "b")
						material->Diffuse.z =
						std::stof(specularNode.child_value());

					if (specularNodeName == "a")
						material->Diffuse.w =
						std::stof(specularNode.child_value());
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
		}
		return material;
	}

	bool LoadSceneFile(std::string filename, Ref<EntityManager> entityManager)
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
				pugi::xml_object_range<pugi::xml_node_iterator> gameObjectData = sceneNode.children();

				uint32_t id = entityManager->CreateEntity();
				Entity* entity = entityManager->GetEntity(id);
				for (pugi::xml_node_iterator gameobjectsDataIterator = gameObjectData.begin();
					gameobjectsDataIterator != gameObjectData.end(); gameobjectsDataIterator++)
				{
					pugi::xml_node gameObjectNode = *gameobjectsDataIterator;
					std::string nodeName = gameObjectNode.name();
					if (nodeName == "name")
					{
						entity->name = gameObjectNode.child_value();
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
								TransformComponent* transform = entityManager->GetComponentByType<TransformComponent>(entity->GetID());
								pugi::xml_object_range<pugi::xml_node_iterator>
									transformNodeChildren = componentNode.children();
								for (pugi::xml_node_iterator transformNodeIterator = transformNodeChildren.begin();
									transformNodeIterator != transformNodeChildren.end();
									transformNodeIterator++)
								{
									pugi::xml_node transformNode = *transformNodeIterator;
									std::string transformNodeName = transformNode.name();
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
												transform->Rotation.x =
												std::stof(rotationNode.child_value());
											if (rotationNodeName == "y")
												transform->Rotation.y =
												std::stof(rotationNode.child_value());
											if (rotationNodeName == "z")
												transform->Rotation.z =
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
							}

							if (componentNodeName == "light")
							{
								LightComponent* light = new LightComponent();
								entityManager->AddComponent(entity->GetID(), light);
								pugi::xml_object_range<pugi::xml_node_iterator>
									lightNodeChildren = componentNode.children();
								//continue;
								for (pugi::xml_node_iterator lightNodeIterator = lightNodeChildren.begin();
									lightNodeIterator != lightNodeChildren.end();
									lightNodeIterator++)
								{
									pugi::xml_node lightNode = *lightNodeIterator;
									std::string lightNodeName = lightNode.name();
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
									if (lightNodeName == "atten")
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
									if (lightNodeName == "param1")
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
							}

							if (componentNodeName == "meshobject")
							{
								MeshComponent* meshObject = new MeshComponent();
								for (pugi::xml_node_iterator meshNodeIterator = componentNode.children().begin();
									meshNodeIterator != componentNode.children().end();
									meshNodeIterator++)
								{
									pugi::xml_node meshNode = *meshNodeIterator;
									std::string meshNodeName = meshNode.name();
									if (meshNodeName == "path")
									{
										meshObject->Path = meshNode.child_value();
									}
									if (meshNodeName == "material_path")
									{
										meshObject->MaterialPath = meshNode.child_value();
										if (meshObject->MaterialPath.empty())
											meshObject->MaterialIns = CreateRef<Material>();
										else meshObject->MaterialIns = LoadMaterial(meshObject->MaterialPath);
										if (!meshObject->MaterialIns->DiffuseTexturePath.empty())
										{
											Ref<Texture2D> diffTex = Texture2D::Create(meshObject->MaterialIns->DiffuseTexturePath);
											if (diffTex != nullptr)
											{
												meshObject->MaterialIns->DiffuseTexture = diffTex;
											}
										}
										if (meshObject->MaterialIns && !meshObject->MaterialIns->NormalTexturePath.empty())
										{
											Ref<Texture2D> normTex = Texture2D::Create(meshObject->MaterialIns->NormalTexturePath);
											if (normTex != nullptr)
											{
												meshObject->MaterialIns->NormalTexture = normTex;
											}
										}
										if (meshObject->MaterialIns && !meshObject->MaterialIns->SpecularTexturePath.empty())
										{
											Ref<Texture2D> specTex = Texture2D::Create(meshObject->MaterialIns->SpecularTexturePath);
											if (specTex != nullptr)
											{
												meshObject->MaterialIns->SpecularTexture = specTex;
											}
										}
									}
									if (meshNodeName == "use_fbx_textures")
									{
										meshObject->UseFBXTextures = meshNode.child_value() == "1";
									}

								}
								if (!meshObject->Path.empty())
								{
									meshObject->ModelIns = CreateRef<Model>(meshObject->Path, meshObject->UseFBXTextures);
								}
								entityManager->AddComponent(entity->GetID(), meshObject);
							}
						}
					}
				}

			}

		}
		return true;
	}
}