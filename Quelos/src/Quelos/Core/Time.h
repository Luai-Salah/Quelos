#pragma once

namespace Quelos
{
	class Time
	{
	public:
		static void OnUpdate();

		inline static float DeltaTime() { return s_DeltaTime; }
		inline static float DeltaTimeMs() { return s_DeltaTime * 1000; }
		inline static float GetTime() { return s_LastFrameTime; }
	private:
		static float s_LastFrameTime;
		static float s_DeltaTime;
	};

	class Timer
	{
	public:
		Timer()
			: m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{
			if (!m_Stopped)
				Stop();
		}

		float Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			m_Stopped = true;

			return (float)(end - start);
		}

		float ElapsedMillis()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			return (end - start) / 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
	};
}
