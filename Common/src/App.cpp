#include "pch.h"
#include "App.h"
#include <GLFW/glfw3.h>

namespace rmkl {

	float App::m_InterpolationAlpha = 0.0f;

	App::App()
		: m_Running(true), m_LimitFps(true), m_PreviousTime(glfwGetTime()), m_Lag(0), m_TargetFps(100), m_FixedUpdateFps(FIXED_UPDATE_FPS)
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
		while (m_Running)
		{
			double currentTime = glfwGetTime();
			double elapsedTime = currentTime - m_PreviousTime;
			if (m_LimitFps && elapsedTime < GetTargetFrameDuration())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
				continue;
			}
			m_PreviousTime = currentTime;
			m_Lag += elapsedTime;

			glfwPollEvents();

			while (m_Lag >= GetFixedFrameDuration())
			{
				m_Lag -= GetFixedFrameDuration();
				FixedUpdate();
			}

			Update(elapsedTime);

			double m_InterpolationAlpha = m_Lag / GetFixedFrameDuration();
			
			Render(m_InterpolationAlpha);
		}
	}

}
