#pragma once

#include "vel/Core/Layer.h"
#include <vel/Events/ApplicationEvent.h>
#include <vel/Events/MouseEvent.h>
#include <vel/Events/KeyEvent.h>

namespace vel
{
	class VEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void SetBlockEvents(bool block) { m_BlockEvents = block; }

	private:
		bool m_BlockEvents = true;
		float m_Time = 0.f;
	};
}

