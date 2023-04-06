#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace vel
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetInt(const std::string& uniformName, int value) = 0;
		virtual void SetBool(const std::string& uniformName, bool value) = 0;
					 
		virtual void SetFloat(const std::string& uniformName, float value) = 0;
		virtual void SetFloat2(const std::string& uniformName, const glm::vec2& values) = 0;
		virtual void SetFloat3(const std::string& uniformName, const glm::vec3& values) = 0;
		virtual void SetFloat4(const std::string& uniformName, const glm::vec4& values) = 0;

		virtual void SetMat3(const std::string& uniformName, const glm::mat3& matrix) = 0;
		virtual void SetMat4(const std::string& uniformName, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;
		virtual void DeleteProgram() = 0;
		static Ref<Shader> Create(const std::string& shaderFilePath);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Reload(const std::string& filepath, const std::string& name);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}