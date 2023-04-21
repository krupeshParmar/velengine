#include "velpch.h"
#include "ParticleSystem.h"
#include "vel/Renderer/Renderer.h"
#include "vel/Math/Random.h"
#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include "vel/Renderer/Texture.h"

namespace vel
{
	ParticleSystem::ParticleSystem()
	{
		m_ParticlePool.resize(100);
		m_Shader = m_ShaderLibrary.Load("assets/shaders/ParticleShader.glsl");
		m_Shader->Bind();
		m_Shader->SetInt("u_TextureDiffuse", 0);
		m_Shader->SetInt("u_TextureNormal", 1);
		m_Shader->SetInt("u_TextureSpecular", 2);
		m_Shader->SetInt("u_TextureEmissive", 3);
		m_Shader->SetInt("skyBox", 4);
	}
	void ParticleSystem::OnUpdate(Timestep ts)
	{
		for (Particle& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.Active = false;
				continue;
			}

			particle.LifeRemaining -= ts;
			particle.Position += particle.Velocity * (float)ts;
			particle.Rotation += 0.01f * ts;
		}
	}
	void ParticleSystem::OnRender(glm::vec4 eyeLocation)
	{
		unsigned int count = 0;
		for (Particle& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			float life = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life); 
			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

			// Render
			glm::mat4 transform = glm::translate(particle.parentTransform,
				{ particle.Position.x, particle.Position.y, particle.Position.z })
				* glm::rotate(glm::mat4(1.0f), particle.Rotation, { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { size, size, size });

			m_Shader->Bind();
			m_Shader->SetFloat4("eyeLocation", eyeLocation);
			m_Shader->SetFloat("u_texsize", 1.f);
			m_Shader->SetFloat4("RGBA", color);
			m_Shader->SetFloat4("SPEC", color);
			m_Shader->SetFloat4("EMIS", color);
			m_Shader->SetFloat("SHIN", 0.f);
			m_Shader->SetFloat("EmissiveIntensity", 2.f);
			m_Shader->SetBool("useTextureNormal", false);
			m_Shader->SetBool("useTextureSpecular", false);
			m_Shader->SetBool("useTextureDiffuse", false);
			m_Shader->SetBool("useTextureEmissive", false);
			m_Shader->SetBool("u_UseBone", false);
			std::string transformString = "u_Transform[" + std::to_string(count) + "]";
			std::string inverseTransformString = "u_InverseTransform[" + std::to_string(count) + "]";
			m_Shader->SetMat4(transformString, transform);
			m_Shader->SetMat4(inverseTransformString, glm::inverse(glm::transpose(transform)));
			count++;
		}
		if(count > 0)
			Renderer::SubmitInstanced(count, m_Shader, m_SphereVA);
	}
	void ParticleSystem::Emit(const ParticlesComponent& particleData)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.Active = true;
		particle.Position = particleData.Position;
		particle.Rotation = Math::Random::Float() * 2.0f * glm::pi<float>();

		// Velocity
		particle.Velocity = particleData.Velocity;
		particle.Velocity.x += particleData.VelocityVariation.x * (Math::Random::Float() - 0.5f);
		particle.Velocity.y += particleData.VelocityVariation.y * (Math::Random::Float() - 0.5f);
		particle.Velocity.z += particleData.VelocityVariation.z * (Math::Random::Float() - 0.5f);

		// Color
		particle.ColorBegin = particleData.ColorBegin;
		particle.ColorEnd = particleData.ColorEnd;

		particle.LifeTime = particleData.LifeTime;
		particle.LifeRemaining = particleData.LifeTime;
		particle.SizeBegin = particleData.SizeBegin + particleData.SizeVariation * (Math::Random::Float() - 0.5f);
		particle.SizeEnd = particleData.SizeEnd;
		particle.parentTransform = particleData.parentTransform;

		m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
	}
}