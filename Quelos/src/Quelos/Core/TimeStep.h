#pragma once

namespace Quelos
{
	struct TimeStep
	{
	public:
		TimeStep(float time)
			: m_Time(time)
		{
		}

		float GetSeconds() const { return m_Time; }
		float GetMilliSeconds() const { return m_Time * 1000; }

		operator float() { return m_Time; }
	private:
		float m_Time;
	};
}
