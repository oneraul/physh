#pragma once

#include <chrono>
#include <enet/enet.h>

namespace rmkl {

	class App
	{
	public:
		App();
		virtual ~App();
		void Run();

	protected:
		virtual void PollEvents() = 0;
		virtual void FixedUpdate() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render(float interpolationAlpha) = 0;
		virtual void OnNetworkConnected(const ENetEvent& e) = 0;
		virtual void OnNetworkDisconnected(const ENetEvent& e) = 0;
		virtual void OnNetworkReceived(const ENetEvent& e) = 0;
		void ProcessNetworkEvents();

	protected:
		bool m_Running;
		ENetHost* m_EnetHost;
		float m_InterpolationAlpha;

	};

	App* CreateApplication();
}
