#include "QSPCH.h"
#include "WindowsWindow.h"

#include "Quelos\Event\ApplicationEvents.h"
#include "Quelos\Event\KeyEvent.h"
#include "Quelos\Event\MouseEvents.h"
#include "Quelos/Renderer/Texture.h"

#include "Platform\OpenGL\OpenGLContext.h"

#include <stb_image.h>

namespace Quelos
{
	static int s_GLFWWindowCount = 0;

	Window* Window::Create(const WindowProps& props) { return new WindowsWindow(props); }

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		QS_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow() { Shutdown(); }

	void GLFWErrorCallback(int err, const char* desc)
	{
		QS_CORE_ERROR("GLFW error[{0}]: {1}", err, desc);
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		QS_PROFILE_FUNCTION();

		m_Data.Title = props.Ttile;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		QS_CORE_INFO("Createing Window :\n\tTitle: {0}\n\tSize: ({1}, {2})",
			m_Data.Title.c_str(),
			(int)m_Data.Width,
			(int)m_Data.Height
		);

		if (s_GLFWWindowCount == 0)
		{
			QS_PROFILE_SCOPE("glfwInit")
			int seccess = glfwInit();
			QS_CORE_ASSERT(seccess, "Couldn't Initialize GLFW")

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			QS_PROFILE_SCOPE("glfwCreateWindow")
			m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), NULL, NULL);
		}
		
		m_Context = new OpenGLContext(m_Window);

		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizedEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent e;
			data.EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancodes, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, false);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallback(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, true);
					data.EventCallback(e);
					break;
				}
				default:
					break;
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
					break;
				}
				default:
					break;
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xOffset, (float)yOffset);
			data.EventCallback(e);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x, (float)y);
			data.EventCallback(e);
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent e(keycode);
			data.EventCallback(e);
		});

		s_GLFWWindowCount++;
	}

	void WindowsWindow::OnUpdate()
	{
		QS_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		QS_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
		m_Data.Title = title;
	}

	void WindowsWindow::SetIcon(const std::string& texture)
	{
		GLFWimage images[1];
		stbi_set_flip_vertically_on_load(0);
		images[0].pixels = stbi_load(texture.c_str(), &images[0].width, &images[0].height, 0, 4);
		glfwSetWindowIcon(m_Window, 1, images);
		stbi_image_free(images[0].pixels);
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
	void WindowsWindow::Shutdown()
	{
		QS_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
			glfwTerminate();
	}
}
