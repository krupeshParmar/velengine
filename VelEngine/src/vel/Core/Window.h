#pragma once

#include "velpch.h"

#include "vel/Core/Core.h"
#include "vel/Events/Event.h"

namespace vel
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		
		WindowProps(const std::string& title = "Vel Engine",
					unsigned int width = 2560,
					unsigned int height = 1440)
			: Title(title), Width(width), Height(height)
		{}
	};

	class VEL_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}