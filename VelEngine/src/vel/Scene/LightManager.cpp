#include <velpch.h>

#include "LightManager.h"


namespace vel
{
	std::vector<LightComponent*> LightManager::s_Lights = std::vector<LightComponent*>();

	unsigned int LightManager::LIGHT_INDEX = 0;

	LightManager::LightManager()
	{
		Clear();
	}

	void LightManager::Clear()
	{
		s_Lights.clear();
		LightManager::LIGHT_INDEX = 0;
	}

	void LightManager::CopyLightInformationToShader(Ref<Shader> shader)
	{
		std::string posStr = "position";
		std::string difStr = "diffuse";
		std::string speStr = "specular";
		std::string attStr = "atten";
		std::string dirStr = "direction";
		std::string pa1Str = "param1";
		std::string intStr = "intensity";
		for (std::vector<LightComponent*>::iterator itLight = s_Lights.begin();
			itLight != s_Lights.end();
			itLight++)
		{
			LightComponent* light = *itLight;
			std::string lightsVariableName = "theLights[" + std::to_string(light->ID) + "].";
			if (!light->Enabled)
			{
				shader->SetFloat4(lightsVariableName + pa1Str, glm::vec4(0.f));
				continue;
			}

			shader->SetFloat4(lightsVariableName + posStr, light->Position);
			shader->SetFloat4(lightsVariableName + difStr, light->Diffuse);
			shader->SetFloat4(lightsVariableName + speStr, light->Specular);
			shader->SetFloat4(lightsVariableName + attStr, light->Attenuation);
			shader->SetFloat4(lightsVariableName + dirStr, light->Direction);
			shader->SetFloat4(lightsVariableName + pa1Str, light->LightParams);
			shader->SetFloat(lightsVariableName + intStr, light->Intensity);
		}
	}
	void LightManager::AddNewLightInfo(LightComponent* light)
	{
		light->ID = LIGHT_INDEX;
		s_Lights.push_back(light);
		LIGHT_INDEX++;
	}
}