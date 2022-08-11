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

	Application* CreateApplication()
	{
		ApplicationSpecification appSpec;
		appSpec.Name = "Quelos Editor";
		appSpec.WorkingDirectory = "../QuelosEditor";

		return new QuelosEditor(appSpec);
	}
}
