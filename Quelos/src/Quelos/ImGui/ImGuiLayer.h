#pragma once

#include "Quelos\Core\Layer.h"

namespace Quelos
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void BlockEvents(bool value) { m_BlockEvents = value; }

		void Begin();
		void End();

		void SetDarkThemeColor();
		
		uint32_t GetActiveWidgetID() const;
	private:
		bool m_BlockEvents = false;
		float m_Time = 0;
	};
}
