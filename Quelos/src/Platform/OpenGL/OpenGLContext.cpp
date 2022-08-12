#include "QSPCH.h"
#include "OpenGLContext.h"

#include "GLFW\glfw3.h"
#include "glad\glad.h"

namespace Quelos
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		QS_CORE_ASSERT(windowHandle, "Window Handle is null!");
	}

	void OpenGLContext::Init()
	{
		QS_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		QS_CORE_ASSERT(status, "Failed to initialize Glad!");

		const char* format = R"(OpenGL Info:
	Vendor: {}
	Renderer: {}
	Version: {})";

		//QS_CORE_LOG(format, glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		QS_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}
}
