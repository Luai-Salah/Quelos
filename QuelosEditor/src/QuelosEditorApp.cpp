#include <Quelos.h>
#include <Quelos\Core\EntryPoint.h>

#include "EditorLayer.h"

namespace Quelos
{
	class QuelosEditor : public Application
	{
	public:
		QuelosEditor(const ApplicationSpecification& specifications)
			: Application(specifications)
		{
			PushLayer(new EditorLayer());
		}

		~QuelosEditor()
		{
		}

	private:

	};

	Application* CreateApplication(const ApplicationCommandLineArgs& commandLineArgs)
	{
		ApplicationSpecification appSpec;
		appSpec.Name = "Quelos Editor";
		appSpec.WorkingDirectory = "../QuelosEditor";
		appSpec.CommandLineArgs = commandLineArgs;

		Project::New()->SaveActive("Sandbox.qproj");

		return new QuelosEditor(appSpec);
	}
}
