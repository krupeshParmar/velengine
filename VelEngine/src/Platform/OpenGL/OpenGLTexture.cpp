#include "velpch.h"
#include "OpenGLTexture.h"
#include <glad/glad.h>
#include "stb_image.h"

namespace vel
{
	//******************* 2D ************************
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		:m_Width(width), m_Height(height)
	{
		VEL_PROFILE_FUNCTION();
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		CreateData(path);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		VEL_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		VEL_PROFILE_FUNCTION();
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		VEL_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::CreateData(const std::string& path)
	{
		m_Path = path;
		VEL_PROFILE_FUNCTION();
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			VEL_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		if (!data)
		{
			m_Loaded = false;
		}
		//VEL_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_SRGB8_ALPHA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3) {
			internalFormat = GL_SRGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		VEL_CORE_ASSERT(internalFormat & dataFormat, "[OpenGLTexture]: Format not supported!!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		m_Loaded = true;
		stbi_image_free(data);
	}

	void OpenGLTexture2D::RemoveData()
	{
		m_Loaded = false;
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		VEL_PROFILE_FUNCTION();
		glBindTextureUnit(slot, m_RendererID);
	}

	//******************* CUBE MAP ************************

	OpenGLTextureCubeMap::OpenGLTextureCubeMap(const std::vector<std::string> paths)
	{
		CreateData(paths);
	}
	OpenGLTextureCubeMap::~OpenGLTextureCubeMap()
	{
	}
	void OpenGLTextureCubeMap::CreateData(const std::vector<std::string> paths)
	{
		m_Paths = paths;
		VEL_PROFILE_FUNCTION();
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			VEL_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			for (int i = 0; i < m_Paths.size(); i++)
			{
				data = stbi_load(m_Paths[i].c_str(), &width, &height, &channels, 0);

				if (!data)
				{
					m_Loaded = false;
					VEL_CORE_ERROR("Couldn't load cubemap data for " + m_Paths[i]);
					glDeleteTextures(1, &m_RendererID);
					return;
				}
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
		}
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		m_Loaded = true;
	}
	void OpenGLTextureCubeMap::RemoveData()
	{
		m_Loaded = false;
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCubeMap::DrawSkyBox(glm::mat4 viewProjection, Ref<Shader> shader, Ref<VertexArray> vertexArray)
	{
		if (!m_Loaded)
			return;
		if (!vertexArray)
			return;
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		glCullFace(GL_FRONT);
		shader->Bind();
		shader->SetMat4("viewprojection", viewProjection);

		shader->SetFloat3("tilingfactor", tilingfactor);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		glCullFace(GL_BACK);
	}

	void OpenGLTextureCubeMap::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}
}