#pragma once
#include <glm/glm.hpp>
#include <vel/Core/Timestep.h>
#include <vector>
#include "VertexArray.h"
#include "vel/Core/Core.h"
#include "Shader.h"
namespace vel
{
	struct ParticleProps
	{
		glm::vec3 Position;
		glm::vec3 Velocity, VelocityVariation;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd, SizeVariation;
		float LifeTime = 1.0f;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem();

		void OnUpdate(Timestep ts);
		void OnRender();

		void Emit(const ParticleProps& particleProps);
	private:
		struct Particle
		{
			glm::vec3 Position;
			glm::vec3 Velocity;
			glm::vec4 ColorBegin, ColorEnd;
			float Rotation = 0.0f;
			float SizeBegin, SizeEnd;

			float LifeTime = 1.0f;
			float LifeRemaining = 0.0f;

			bool Active = false;
		};
		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex = 999;

		Ref<VertexArray> m_QuadVA = 0;
		Ref<Shader> m_ParticleShader;
	};
}