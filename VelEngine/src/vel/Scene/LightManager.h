#pragma once
#include <vector>
#include "Component.h"
#include "vel/Renderer/Shader.h"

namespace vel
{

	class LightManager
	{
	public:
		LightManager();

		static const unsigned int NUMBER_OF_LIGHTS_IN_USE = 20;
		static unsigned int LIGHT_INDEX;

		static void CopyLightInformationToShader(Ref<Shader> shader);
		static void AddNewLightInfo(LightComponent* light);

	private:
		static std::vector<LightComponent*> s_Lights;
	};
}