#pragma once

#include "vel/Renderer/FrameBuffer.h"

namespace vel
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual const uint32_t GetID() const override {
			return m_RendererID;
		}

		virtual void BindColorTexture() override;
		virtual void BindWorldPositionTexture() override;
		virtual void BindNormalTexture() override;
		virtual void BindSpecularTexture() override;

		virtual void CopyDepthData(Ref<FrameBuffer> buffer) override;


		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachmenRendererID() const override { return m_ColorAttachment; }
		virtual uint32_t GetSpecularAttachmenRendererID() const override { return m_SpecularAttachment; }
		virtual uint32_t GetNormalAttachmenRendererID() const override { return m_NormalAttachment; }
		virtual uint32_t GetDepthAttachmenRendererID() const override { return m_DepthAttachment; }
		virtual uint32_t GetVerteWorldPositionAttachmenRendererID() const override { return m_VertexWorldPosition; }

		void Invalidate();

		//virtual const SetSpecification(FrameBufferSpecification& specs) = 0;
		virtual const FrameBufferSpecification& GetSpecification() const override;

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_VertexWorldPosition = 0;
		uint32_t m_NormalAttachment = 0;
		uint32_t m_SpecularAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		FrameBufferSpecification m_Specification;

	};
}