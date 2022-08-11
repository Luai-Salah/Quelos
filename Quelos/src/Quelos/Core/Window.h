#pragma once

#include <string>
#include <functional>

#include "Quelos/Core/Core.h"

struct GLFWwindow;

namespace Quelos
{
	class Event;
	class Texture2D;

	struct WindowProps
	{
		std::string Ttile;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title, uint32_t width = 1280, uint32_t height = 720)
			: Ttile(title), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual GLFWwindow* GetNativeWindow() = 0;

		virtual void SetEventCallbackFunc(const EventCallbackFunc& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetIcon(const std::string& texture) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProps& windowProps);
	};
}
