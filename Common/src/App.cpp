#include "pch.h"
#include "App.h"
#include <GLFW/glfw3.h>

namespace rmkl {

	App::App()
		: m_Running(true) 
		, m_FixedFrameDuration(std::chrono::milliseconds(33))
		, m_InterpolationAlpha(0)
	{
		ASSERT(glfwInit(), "Failed to initialize GLFW");
		glfwSetErrorCallback([](int error, const char* description) {
			std::cout << "GLFW Error (" << error << "): " << description << std::endl;
		});
	}

	App::~App()
	{
		enet_host_destroy(m_EnetHost);
		enet_deinitialize();
		glfwTerminate();
	}

	void App::Run()
	{
		std::chrono::nanoseconds lag(0);
		std::chrono::nanoseconds m_fixedFrameDuration(std::chrono::milliseconds(33));

		auto previousTime = std::chrono::high_resolution_clock::now();

		while (m_Running)
		{
			auto currentTime = std::chrono::high_resolution_clock::now();
			auto elapsedTime = currentTime - previousTime;
			previousTime = currentTime;
			lag += elapsedTime;

			glfwPollEvents();
			ProcessNetworkEvents();

			while (lag >= m_FixedFrameDuration)
			{
				lag -= m_FixedFrameDuration;
				FixedUpdate();
			}

			Update((float)elapsedTime.count());

			m_InterpolationAlpha = (float)lag.count() / m_FixedFrameDuration.count();
			
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
