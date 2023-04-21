#pragma once
#include <vel/Core/Timestep.h>
#include "Component.h"
#include "vel/Renderer/Shader.h"
#include <vector>
#include "vel/Renderer/VertexArray.h"

namespace vel
{
	class ParticleSystem
	{
	public:
		ParticleSystem();

		void OnUpdate(Timestep ts);
		void OnRender(glm::vec4 eyeLocation);

		void Emit(const ParticlesComponent& particleData);

		inline void SetSphereVA(Ref<VertexArray> sphereVA)
		{
			m_SphereVA = sphereVA;
		}

	private:
		struct Particle
		{
			glm::mat4 parentTransform = glm::mat4(1.f);
			glm::vec3 Position;
			glm::vec3 Velocity;
			glm::vec4 ColorBegin, ColorEnd;

			float Rotation = 0.f;
			float SizeBegin, SizeEnd;

			float LifeTime = 1.f;
			float LifeRemaining = 0.f;

			bool Active = false;
		};
		std::vector<Particle> m_ParticlePool;
		ShaderLibrary m_ShaderLibrary;
		Ref<Shader> m_Shader;
		uint32_t m_PoolIndex = 100;
		
		Ref<VertexArray> m_SphereVA;
	};
}