#pragma once

namespace Quelos
{
	class Input
	{
	public:
		static void Init();

		static bool GetKey(KeyCode keycode);
		static bool GetKeyDown(KeyCode keycode);
		static bool GetKeyUp(KeyCode keycode);

		static bool IsMouseButtonPressed(MouseButtonCode button);
		static bool IsMouseButtonReleased(MouseButtonCode button);

		static glm::vec2 GetMousePosition();
	};
}
