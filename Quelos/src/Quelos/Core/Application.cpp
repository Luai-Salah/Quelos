#include "QSPCH.h"
#include "Application.h"

#include "Quelos\Event\KeyEvent.h"
#include "Quelos\Core\Log.h"
#include <Quelos\Event\ApplicationEvents.h>
#include "Quelos\ImGui\ImGuiLayer.h"

#include "Input.h"
#include "Quelos\Core\KeyCodes.h"

#include "Quelos\Renderer\Buffer.h"
#include "Quelos\Renderer\Renderer.h"
#include "Quelos\Renderer\RenderCommand.h"

#include "Quelos/Scripting/ScriptEngine.h"

#include "Quelos/Core/AssetsManager.h"

#include "Quelos\Core\Time.h"

namespace Quelos
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& appSpec)
	{
		QS_PROFILE_FUNCTION();

		QS_CORE_ASSERT((!s_Instance), "Application already exists!")
		s_Instance = this;
		
		m_Specification = appSpec;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		Log::Init();

		m_Window = std::unique_ptr<Window>(Window::Create({ m_Specification.Name, 1440, 900 }));
		m_Window->SetEventCallbackFunc(BIND_EVNT_FUNC(Application::OnEvent));

		Input::Init();
		Renderer::Init();
		ScriptEngine::Init();
		AssetsManager::Init();

		//Scripting::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		QS_PROFILE_FUNCTION();

		ScriptEngine::Shutdown();
		Renderer::Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		QS_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVNT_FUNC(Application::OnWindowCloseEvent));
		dispatcher.Dispatch<WindowResizedEvent>(BIND_EVNT_FUNC(Application::OnWindowResizedEvent));

		for (auto& layer : m_LayerStack)
		{
			if (e.Handled)
				break;
			layer->OnEvent(e);
		}
	}

	void Application::Run()
	{
		QS_PROFILE_FUNCTION();

		while (m_IsRunning)
		{
			QS_PROFILE_SCOPE("RunLoop");

			Time::OnUpdate();
			TimeStep timestep = Time::DeltaTime();
			
			if (!m_Minimized)
			{
				{
					//Update Layers
					QS_PROFILE_SCOPE("LayerStack OnUpdate");
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				if (m_RenderImGui)
				{
					//Render ImGui Things
					m_ImGuiLayer->Begin();
					{
						QS_PROFILE_SCOPE("LayerStack OnImGuiRender");
						for (Layer* layer : m_LayerStack)
							layer->OnImGuiRender();
					}
					m_ImGuiLayer->End();
				}
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		OnClose();
		return true;
	}

	bool Application::OnWindowResizedEvent(WindowResizedEvent& e)
	{
		QS_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResized(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::OnClose()
	{
		QS_CORE_ERROR("Application Closed");
		m_IsRunning = false;
	}

	void Application::PushLayer(Layer* layer)
	{
		QS_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		QS_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		QS_PROFILE_FUNCTION();

		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::PopOverlay(Layer* overlay)
	{
		QS_PROFILE_FUNCTION();

		m_LayerStack.PopOverlay(overlay);
		overlay->OnDetach();
	}
}
