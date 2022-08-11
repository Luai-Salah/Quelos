#pragma once

#ifdef QS_PLATFORM_WINDOWS

extern Quelos::Application* Quelos::CreateApplication();

int main()
{
	QS_PROFILE_BEGIN_SESSION("Startup", "Profiling\\QS_Profile-Startup.json");
	auto app = Quelos::CreateApplication();
	QS_PROFILE_END_SESSION();

	QS_PROFILE_BEGIN_SESSION("Runtime", "Profiling\\QS_Profile-Runtime.json");
	app->Run();
	QS_PROFILE_END_SESSION();

	QS_PROFILE_BEGIN_SESSION("Shutdown", "Profiling\\QS_Profile-Shutdown.json");
	delete app;
	QS_PROFILE_END_SESSION();

	return 0;
}

#endif // QS_PLATFORM_WINDOWS
