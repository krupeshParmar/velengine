#include "velpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace vel
{
	OpenGLRendererAPI::OpenGLRendererAPI()
	{
	}
	void OpenGLRendererAPI::Init()
	{
		VEL_PROFILE_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		//glClearColor(color.r, color.g, color.b, color.a);
		glClearColor(pow(color.r, 2.20f), pow(color.g, 2.20f), pow(color.b, 2.20f), color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		//glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	GLenum glCheckError_()
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

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		
		/*GLenum error = glCheckError_();

		if (error != GL_NO_ERROR)
		{

			VEL_ASSERT(false,"OpenGL error!!");
		}*/
	}

	void OpenGLRendererAPI::SetViewport(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetCullFace()
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::EnableDepth()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}

	void OpenGLRendererAPI::DisableDepth()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetDepthMask(bool enable)
	{
		if(enable)
			glDepthMask(GL_TRUE);
		else glDepthMask(GL_FALSE);
	}

	void OpenGLRendererAPI::SetPolygonMode(bool isFilled)
	{
		if(isFilled)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLRendererAPI::SetStencilTest(bool enable)
	{
		if (enable)
			glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::SetColorMask(bool mask)
	{
		if(mask)
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		else glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}

	void OpenGLRendererAPI::SetStencilMask(int mask)
	{
		glStencilMask(mask); 
	}

	void OpenGLRendererAPI::SetStencilFunc()
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	void OpenGLRendererAPI::BindTextureUnit(uint32_t slot, uint32_t renderID)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, renderID);
	}

}