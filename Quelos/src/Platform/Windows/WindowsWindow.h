#pragma once

#include "Quelos\Core\Window.h"

#include <GLFW\glfw3.h>
#include "Quelos\Renderer\GraphicsContext.h"

namespace Quelos
{
	class WindowsWindow
		: public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline GLFWwindow* GetNativeWindow() override { return m_Window; }

		void OnUpdate() override;

		inline void SetEventCallbackFunc(const EventCallbackFunc& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		void SetTitle(const std::string& title) override;
		void SetIcon(const std::string& texture) override;
		bool IsVSync() const override;

	private:
		virtual void Init(const WindowProps& porps);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;

			EventCallbackFunc EventCallback;	
			bool VSync;
		};

		WindowData m_Data;
	};
}
