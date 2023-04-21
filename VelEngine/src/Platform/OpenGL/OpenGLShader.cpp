#include "velpch.h"

#include "OpenGLShader.h"
#include <glad/glad.h>
#include <vel/Core/Log.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

namespace vel
{
	GLenum glCheckError()
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
			switch (errorCode)
			{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
			}
			VEL_ERROR(error);
		}
		return errorCode;
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		VEL_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& shaderFilePath)
		:m_Path(shaderFilePath)
	{
		VEL_PROFILE_FUNCTION();
		std::string source = ReadFile(shaderFilePath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
		
		// assets/shaders/Texture.glsl (get last slashed name, ignore the extension)
		auto lastSlash = shaderFilePath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = shaderFilePath.rfind(".");

		auto count =
			lastDot == std::string::npos 
			? shaderFilePath.size() - lastSlash 
			: lastDot - lastSlash;
		m_Name = shaderFilePath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
	{
		VEL_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(shaderSources);
	}

	OpenGLShader::~OpenGLShader()
	{
		VEL_PROFILE_FUNCTION();
		glDeleteProgram(m_RendererID);
	}


	std::string OpenGLShader::ReadFile(const std::string& filePath)
	{
		VEL_PROFILE_FUNCTION();
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			VEL_CORE_ERROR("Could not open file '{0}'", filePath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		VEL_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			VEL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			VEL_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = 
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

		}
		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		VEL_PROFILE_FUNCTION();
		GLuint program = glCreateProgram();
		VEL_CORE_ASSERT(shaderSources.size() <= 2, "More shader than expected");
		std::array<GLuint, 2> glShaderIDs;
		int glShaderIdIndex = 0;
		for (auto& key : shaderSources)
		{
			GLenum shaderType = key.first;
			const std::string& source = key.second;

			GLuint shader = glCreateShader(shaderType);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);


				VEL_CORE_ERROR("{0}: {1}", m_Path, infoLog.data());
				VEL_CORE_ASSERT(false, "Shader compile failed!!");

				break;
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIdIndex++] = shader;

		}
		m_RendererID = program;

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
			{
				glDeleteShader(id);
			}
			VEL_CORE_ERROR("{0}: {1}", m_Path, infoLog.data());
			VEL_CORE_ASSERT(false, "Shader linking failed!!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
		}
	}

	void OpenGLShader::Bind() const
	{
		VEL_PROFILE_FUNCTION();
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& uniformName, int value)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformInt(uniformName, value);
	}

	void OpenGLShader::SetBool(const std::string& uniformName, bool value)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformBool(uniformName, value);
	}

	void OpenGLShader::SetFloat(const std::string& uniformName, float value)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformFloat(uniformName, value);
	}

	void OpenGLShader::SetFloat2(const std::string& uniformName, const glm::vec2& values)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformFloat2(uniformName, values);
	}

	void OpenGLShader::SetFloat3(const std::string& uniformName, const glm::vec3& values)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformFloat3(uniformName, values);
	}

	void OpenGLShader::SetFloat4(const std::string& uniformName, const glm::vec4& values)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformFloat4(uniformName, values);
	}

	void OpenGLShader::SetMat3(const std::string& uniformName, const glm::mat3& matrix)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformMat3(uniformName, matrix);
	}

	void OpenGLShader::SetMat4(const std::string& uniformName, const glm::mat4& value)
	{
		VEL_PROFILE_FUNCTION();
		UploadUniformMat4(uniformName, value);
	}

	void OpenGLShader::DeleteProgram()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::UploadUniformInt(const std::string& uniformName, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniform1i(location, value);
		glCheckError();
	}

	void OpenGLShader::UploadUniformBool(const std::string& uniformName, bool value)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniform1i(location, (GLfloat) value);
		glCheckError();
	}

	void OpenGLShader::UploadUniformFloat(const std::string& uniformName, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniform1f(location, value);
		glCheckError();
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& uniformName, const glm::vec2& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniform2f(location, values.x, values.y);
		glCheckError();
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& uniformName, const glm::vec3& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniform3f(location, values.x, values.y, values.z);
		glCheckError();
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& uniformName, const glm::vec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
		glCheckError();
	}

	void OpenGLShader::UploadUniformMat3(const std::string& uniformName, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniformMatrix3fv(location, 1, false, glm::value_ptr(matrix));
		glCheckError();
	}

	void OpenGLShader::UploadUniformMat4(const std::string& uniformName, const glm::mat4& matrix)
	{
		glUseProgram(m_RendererID);
		GLint location = glGetUniformLocation(m_RendererID, uniformName.c_str());
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(matrix));
		glCheckError();
	}
}