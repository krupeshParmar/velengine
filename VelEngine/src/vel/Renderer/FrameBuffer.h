#pragma once

#include "vel/Core/Core.h"
#include <glm/glm.hpp>

namespace vel
{
	struct FrameBufferSpecification
	{
		uint32_t Width, Height;

		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void BindColorTexture() = 0;

		virtual uint32_t GetColorAttachmenRendererID() const = 0;
		virtual uint32_t GetDepthAttachmenRendererID() const = 0;
		virtual uint32_t GetVerteWorldPositionAttachmenRendererID() const = 0;

		//virtual const SetSpecification(FrameBufferSpecification& specs) = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;
		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};
}