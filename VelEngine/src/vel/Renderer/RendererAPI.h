#pragma once
#include <glm/glm.hpp>
#include "VertexArray.h"

namespace vel
{
	class RendererAPI
	{
	public:

		enum class API
		{
			None = 0,
			OpenGL = 1,
			Vulkan = 2,
			Direct3D = 3,
		};

	public:
		virtual void Init() = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) = 0;
		virtual void DrawInstanced(const Ref<VertexArray>& vertexArray, unsigned int count) = 0;

		virtual void SetViewport(int x, int y, int width, int height) = 0;

		virtual void SetCullFace() = 0;

		virtual void EnableDepth() = 0;

		virtual void DisableDepth() = 0;

		virtual void SetDepthMask(bool) = 0;

		virtual void SetPolygonMode(bool) = 0;

		virtual void SetStencilTest(bool) = 0; 

		virtual void SetColorMask(bool) = 0;

		virtual void SetStencilMask(int mask) = 0;

		virtual void SetStencilFunc() = 0;

		virtual void BindTextureUnit(uint32_t slot, uint32_t renderID) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}
