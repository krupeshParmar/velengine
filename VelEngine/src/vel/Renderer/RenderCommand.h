#pragma once
#include "RendererAPI.h"

namespace vel
{
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
		inline static void SetViewport(int x, int y, int width, int height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetCullFace()
		{
			s_RendererAPI->SetCullFace();
		}

		inline static void EnableDepth()
		{
			s_RendererAPI->EnableDepth();
		}

		inline static void DisableDepth()
		{
			s_RendererAPI->DisableDepth();
		}

		inline static void SetDepthMask(bool enable)
		{
			s_RendererAPI->SetDepthMask(enable);
		}

		inline static void SetPolygonMode(bool isFilled)
		{
			s_RendererAPI->SetPolygonMode(isFilled);
		}

		inline static void SetStencilTest(bool enabled)
		{
			s_RendererAPI->SetStencilTest(enabled);
		}

		inline static void SetColorMask(bool mask)
		{
			s_RendererAPI->SetColorMask(mask);
		}

		inline static void SetStencilMask(int mask)
		{
			s_RendererAPI->SetStencilMask(mask);
		}

		inline static void SetStencilFunc()
		{
			s_RendererAPI->SetStencilFunc();
		}

		inline static void BindTextureUnit(uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindTextureUnit(slot, renderID);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}