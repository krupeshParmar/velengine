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
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::BindColorTexture()
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		GLfloat black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Creating color buffer
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexStorage2D(GL_TEXTURE_2D,
			1, GL_RGB8, m_Specification.Width, m_Specification.Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER /*GL_CLAMP_TO_EDGE*/);

		

		glCreateTextures(GL_TEXTURE_2D, 1, &m_VertexWorldPosition);
		glBindTexture(GL_TEXTURE_2D, m_VertexWorldPosition);

		//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8,			// 8 bits per colour
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F,		// 24 bits per colour
			m_Specification.Width,				// g_FBO_SizeInPixes
			m_Specification.Height);			// g_FBO_SizeInPixes

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
			GL_DEPTH_STENCIL_ATTACHMENT,
			m_DepthAttachment, 0);

		static const GLenum draw_bufers[] =
		{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,		// vertex world position
			GL_DEPTH_STENCIL_ATTACHMENT
		};
		glDrawBuffers(2, draw_bufers);

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