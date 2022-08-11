#include "QSPCH.h"
#include "Time.h"

#include <GLFW\glfw3.h>

namespace Quelos
{
	float Time::s_DeltaTime = 0.0f;
	float Time::s_LastFrameTime = 0.0f;

	void Time::OnUpdate()
	{
		QS_PROFILE_FUNCTION();

		float time = (float)glfwGetTime();
		s_DeltaTime = time - s_LastFrameTime;
		s_LastFrameTime = time;
	}
}
