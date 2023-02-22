#pragma once

#include "vel/Renderer/Shader.h"
#include <glm/glm.hpp>
#include <string>

// TODO: REMOVE!!
typedef unsigned int GLenum;

namespace vel
{
	class OpenGLShader: public Shader
	{
	public:
		OpenGLShader(const std::string& shaderFilePath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetInt(const std::string& uniformName, int value) override;
		virtual void SetBool(const std::string& uniformName, bool value) override;

		virtual void SetFloat(const std::string& uniformName, float value) override;
		virtual void SetFloat2(const std::string& uniformName, const glm::vec2& values) override;
		virtual void SetFloat3(const std::string& uniformName, const glm::vec3& values) override;
		virtual void SetFloat4(const std::string& uniformName, const glm::vec4& values) override;

		virtual void SetMat3(const std::string& uniformName, const glm::mat3& matrix) override;
		virtual void SetMat4(const std::string& uniformName, const glm::mat4& value) override;


		virtual const std::string& GetName() const override { return m_Name; };

		void UploadUniformInt(const std::string& uniformName, int value);
		void UploadUniformBool(const std::string& uniformName, bool value);

		void UploadUniformFloat(const std::string& uniformName, float value);
		void UploadUniformFloat2(const std::string& uniformName, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& uniformName, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& uniformName, const glm::vec4& values);

		void UploadUniformMat3(const std::string& uniformName, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& uniformName, const glm::mat4& matrix);
		
		inline const uint32_t GetID() { return m_RendererID; }

	private:
		std::string ReadFile(const std::string& filePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}