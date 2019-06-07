#pragma once

#include <enet/enet.h>

namespace rmkl {

	class App
	{
	public:
		App();
		virtual ~App();
		void Run();
	protected:
		virtual void FixedUpdate() = 0;
		virtual void Update(double dt) = 0;
		virtual void Render(double interpolationAlpha) = 0;

		inline int GetTargetFps() const { return m_TargetFps; }
		inline bool GetLimitFps() const { return m_LimitFps; }
		inline void SetLimitFps(bool value) { m_LimitFps = value; }
		inline void SetTargetFps(int value) { m_TargetFps = value; }

	protected:
		bool m_Running;
		ENetHost* m_EnetHost;
		static float m_InterpolationAlpha;
		
	private:
		inline double GetTargetFrameDuration() const { return 1.0 / m_TargetFps; }
		inline double GetFixedFrameDuration() const { return 1.0 / m_FixedUpdateFps; }
	private:
		bool m_LimitFps;
		double m_PreviousTime;
		double m_Lag;
		int m_TargetFps;
		int m_FixedUpdateFps;
	};

	App* CreateApplication();
}