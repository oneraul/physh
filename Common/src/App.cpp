#include "pch.h"
#include "App.h"

namespace rmkl {

	App::App()
		: m_Running(true) 
		, m_InterpolationAlpha(0)
	{
	}

	App::~App()
	{
		enet_host_destroy(m_EnetHost);
		enet_deinitialize();
	}

	void App::Run()
	{
		std::chrono::nanoseconds lag(0);
		std::chrono::nanoseconds fixedFrameDuration(std::chrono::milliseconds(33));

		auto previousTime = std::chrono::high_resolution_clock::now();

		while (m_Running)
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			auto elapsedTime = currentTime - previousTime;
			std::chrono::duration<float> elapsedTime_in_seconds = currentTime - previousTime;

			previousTime = currentTime;
			lag += elapsedTime;

			PollEvents();

			while (lag >= fixedFrameDuration)
			{
				lag -= fixedFrameDuration;
				FixedUpdate();
			}

			m_InterpolationAlpha = (float)lag.count() / fixedFrameDuration.count();

			Update(elapsedTime_in_seconds.count());
			Render(m_InterpolationAlpha);
		}
	}

	void App::ProcessNetworkEvents()
	{
		ENetEvent event;
		while (enet_host_service(m_EnetHost, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				OnNetworkConnected(event);
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				OnNetworkReceived(event);
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				OnNetworkDisconnected(event);
				break;
			}
		}
	}

}
