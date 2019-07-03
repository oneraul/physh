#pragma once

#include "App.h"
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Batch.h"
#include "pj/Pj.h"
#include <enet/enet.h>

namespace rmkl {

	class GraphicApp : public App
	{
	public:
		GraphicApp();
		virtual ~GraphicApp();

		inline int GetWindowWidth() const { return m_WindowWidth; }
		inline int GetWindowHeight() const { return m_WindowHeight; }
		inline bool GetVsync() const { return m_Vsync; }
		void SetVsync(bool value);

	protected:
		virtual void FixedUpdate() override;
		virtual void Update(float dt) override;
		virtual void Render(float interpolationAlpha) override;
		virtual void OnNetworkConnected(const ENetEvent& e) override;
		virtual void OnNetworkDisconnected(const ENetEvent& e) override;
		virtual void OnNetworkReceived(const ENetEvent& e) override;

	private:
		void RenderImGui();
		void SendInput(Input input);

	private:
		std::vector<Input> m_PendingInputs;
		std::map<int, Pj> m_Pjs;
		int m_ControlledPj;
		int m_SpectatingPj;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::mat4 m_Model;
		float m_Cam[2];

		int m_WindowWidth;
		int m_WindowHeight;
		bool m_Vsync;
		GLFWwindow* m_Window;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Batch> m_Batch;
		std::unique_ptr<Stage> m_Stage;
		float m_PacketLoss;
	};
}