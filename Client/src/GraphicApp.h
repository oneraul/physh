#pragma once

#include "App.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Batch.h"
#include "pj/Pj.h"
#include <enet/enet.h>
#include <map>
#include "Camera.h"

namespace rmkl {

	class GraphicApp : public App
	{
	public:
		static GraphicApp* GetInstance();
		static Stage GetStage() { return *(_instance->m_Stage); }
		static int GetTick() { return _instance->m_Stage->GetTick(); }
		static int GetRtt() { return _instance->m_EnetHost->peers->roundTripTime; }
		static float GetInterpolationAlpha() { return _instance->m_InterpolationAlpha; }
		static float GetFixedTickrate() { return 1.0f / 30; }
		static float GetServerTickrate() { return 1.0f / 20; }
		static const std::vector<Input>& GraphicApp::GetPendingInputs() { return _instance->m_PendingInputs; }

	private:
		static GraphicApp* _instance;

	public:
		virtual ~GraphicApp();

	protected:
		virtual void FixedUpdate() override;
		virtual void Update(float dt) override;
		virtual void Render(float interpolationAlpha) override;
		virtual void OnNetworkConnected(const ENetEvent& e) override;
		virtual void OnNetworkDisconnected(const ENetEvent& e) override;
		virtual void OnNetworkReceived(const ENetEvent& e) override;

	private:
		GraphicApp();
		
		void RenderImGui();
		void SendInput(Input input);
		void SetVsync(bool value);
		void UpdateRtt(int ms);

	private:
		std::vector<Input> m_PendingInputs;
		std::map<int, Pj> m_Pjs;
		int m_ControlledPj;
		int m_SpectatingPj;

		Camera m_Cam;

		int m_WindowWidth;
		int m_WindowHeight;
		bool m_Vsync;
		GLFWwindow* m_Window;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Batch> m_Batch;
		std::unique_ptr<Stage> m_Stage;
		float m_PacketLoss;
		std::map<int, std::chrono::high_resolution_clock::time_point> pings;
		int _rtt;
	};
}
