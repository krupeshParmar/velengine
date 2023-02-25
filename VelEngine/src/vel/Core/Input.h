#pragma once

#include <vel/Core/Core.h>
#include "vel/Core/KeyCodes.h"

namespace vel
{
	class VEL_API Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseButton button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
