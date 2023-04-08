#pragma once

#include "Window.h"
#include "Quelos\Event\ApplicationEvents.h"

#include "Quelos\Core\LayerStack.h"
#include "Quelos\ImGui\ImGuiLayer.h"

#include "Quelos/Core/TimeStep.h"

namespace Quelos
{
	struct ApplicationCommandLineArgs
	{
		int Count;
		char** Arguments;

		std::string operator[](int index)
		{
			return Arguments[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Quelos App";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpec);
		~Application();

		void Run();
		void OnEvent(Event& e);

		inline static Application& Get() { return *s_Instance; }
		inline static void RenderImGui(bool value) { s_Instance->m_RenderImGui = value; }
		inline static Window& GetWindow() { return *s_Instance->m_Window; }
		inline static void Close() { s_Instance->OnClose(); }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		void SubmitToMainThread(const std::function<void()>& function);

		ApplicationSpecification GetSpecification() const { return m_Specification; }
	private:
		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResizedEvent(WindowResizedEvent& e);
		void ExecuteMainThreadQueue();
		void OnClose();
	private:
		bool m_IsRunning = true;
		bool m_Minimized = false;
		bool m_RenderImGui = true;

		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;

		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication(const ApplicationCommandLineArgs& applicationSpec);
}
