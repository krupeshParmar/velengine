#include "velpch.h"

#include "OpenGLFrameBuffer.h"
#include <glad/glad.h>

namespace vel
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& spec)
		:m_Specification(spec)
	{
		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_VertexWorldPosition);
		glDeleteTextures(1, &m_NormalAttachment);
		glDeleteTextures(1, &m_SpecularAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_FRAMEBUFFER_SRGB);
	}

	void OpenGLFrameBuffer::BindColorTexture(uint32_t slot)
	{
		glBindTextureUnit(slot, m_ColorAttachment);
	}

	void OpenGLFrameBuffer::BindColorTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
	}

	void OpenGLFrameBuffer::BindWorldPositionTexture()
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_VertexWorldPosition);
	}

	void OpenGLFrameBuffer::BindNormalTexture()
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_NormalAttachment);
	}

	void OpenGLFrameBuffer::BindSpecularTexture()
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_SpecularAttachment);
	}

	void OpenGLFrameBuffer::BindBloomTexture()
	{
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_BloomAttachment);
	}

	void OpenGLFrameBuffer::EnableGammaCorrection()
	{
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void OpenGLFrameBuffer::CopyDepthData(Ref<FrameBuffer> buffer)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, buffer->GetID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
		glBlitFramebuffer(0, 0, buffer->GetSpecification().Width, buffer->GetSpecification().Height, 0, 0, m_Specification.Width, m_Specification.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_VertexWorldPosition);
			glDeleteTextures(1, &m_NormalAttachment);
			glDeleteTextures(1, &m_SpecularAttachment);
			glDeleteTextures(1, &m_BloomAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}
		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Creating color buffer
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexStorage2D(GL_TEXTURE_2D,
			1, GL_RGBA32F,
			m_Specification.Width, 
			m_Specification.Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);

		

		glCreateTextures(GL_TEXTURE_2D, 1, &m_VertexWorldPosition);
		glBindTexture(GL_TEXTURE_2D, m_VertexWorldPosition);

		//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8,			// 8 bits per colour
		glTexStorage2D(GL_TEXTURE_2D, 
			1, GL_RGBA32F,		// 24 bits per colour
			m_Specification.Width,				// g_FBO_SizeInPixes
			m_Specification.Height);			// g_FBO_SizeInPixes

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);


		glCreateTextures(GL_TEXTURE_2D, 1, &m_NormalAttachment);
		glBindTexture(GL_TEXTURE_2D, m_NormalAttachment);
		glTexStorage2D(GL_TEXTURE_2D,
			1, GL_RGBA32F,
			m_Specification.Width, 
			m_Specification.Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);


		glCreateTextures(GL_TEXTURE_2D, 1, &m_SpecularAttachment);
		glBindTexture(GL_TEXTURE_2D, m_SpecularAttachment);
		glTexStorage2D(GL_TEXTURE_2D,
			1, GL_RGBA32F, m_Specification.Width, m_Specification.Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);


		glCreateTextures(GL_TEXTURE_2D, 1, &m_BloomAttachment);
		glBindTexture(GL_TEXTURE_2D, m_BloomAttachment);
		glTexStorage2D(GL_TEXTURE_2D,
			1, GL_RGBA32F, m_Specification.Width, m_Specification.Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		

		// Create the depth buffer (texture)
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8,	//GL_DEPTH32F_STENCIL8,
			m_Specification.Width,
			m_Specification.Height);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0,			// Colour goes to #0
			m_ColorAttachment, 0);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT1,			// Vertex world position goes here (to #1)
			m_VertexWorldPosition, 0);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT2,			//Normal  (to #2)
			m_NormalAttachment, 0);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT3,			// Specular (to #3)
			m_SpecularAttachment, 0);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT4,			// Bloom (to #4)
			m_BloomAttachment, 0);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			m_DepthAttachment, 0);

		static const GLenum draw_bufers[] =
		{
			GL_COLOR_ATTACHMENT0,		// colors
			GL_COLOR_ATTACHMENT1,		// vertex world position
			GL_COLOR_ATTACHMENT2,		// normals
			GL_COLOR_ATTACHMENT3,		// specular
			GL_COLOR_ATTACHMENT4,		// bloom
			GL_DEPTH_STENCIL_ATTACHMENT
		};
		glDrawBuffers(5, draw_bufers);

		bool bFrameBufferIsGoodToGo = true;
		std::string error = "";
		switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
		{
		case GL_FRAMEBUFFER_COMPLETE:
			bFrameBufferIsGoodToGo = true;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			bFrameBufferIsGoodToGo = false;
			error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			break;
			//	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		case GL_FRAMEBUFFER_UNSUPPORTED:
		default:
			bFrameBufferIsGoodToGo = false;
			error = "Error";
			break;
		}

		// Point back to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		VEL_CORE_ASSERT(bFrameBufferIsGoodToGo, error);
	}

	const FrameBufferSpecification& OpenGLFrameBuffer::GetSpecification() const
	{
		return m_Specification;
	}
}