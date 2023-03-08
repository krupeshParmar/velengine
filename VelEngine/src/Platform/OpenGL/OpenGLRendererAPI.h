#pragma once
#include "vel/Renderer/RendererAPI.h"

namespace vel
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		OpenGLRendererAPI();
		virtual void Init() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
		virtual void SetViewport(int x, int y, int width, int height) override;

		virtual void SetCullFace() override;

		virtual void EnableDepth() override;

		virtual void DisableDepth()  override;

		virtual void SetDepthMask(bool) override;

		virtual void SetPolygonMode(bool) override;

		virtual void SetStencilTest(bool) override;

		virtual void SetColorMask(bool) override;

		virtual void SetStencilMask(int mask) override;

		virtual void SetStencilFunc() override;

		virtual void BindTextureUnit(uint32_t slot, uint32_t renderID) override;


	};
}