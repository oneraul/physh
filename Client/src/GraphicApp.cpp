#include "pch.h"
#include <imgui.h>
#include <glad/glad.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "GraphicApp.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ForceEmitter.h"
#include "NetMessage.h"
#include "ServiceLocator.h"
#include "Utils.h"

namespace rmkl {

	GraphicApp::GraphicApp()
		: m_WindowWidth(800), m_WindowHeight(600), m_Vsync(0), m_Stage(std::make_unique<Stage>()),
		m_ControlledPj(-1), m_SpectatingPj(-1), m_PacketLoss(0.2f), m_Cam()
	{
		// Init Enet

		ASSERT(enet_initialize() == 0, "An error occurred while initializing ENet.");

		m_EnetHost = enet_host_create(NULL, 1, 2, 0, 0);
		ASSERT(m_EnetHost, "An error occurred while trying to create an ENet client.")

		std::cout << "CLIENT started" << std::endl;

		ENetAddress address;
		ENetEvent event;
		ENetPeer *peer;
		enet_address_set_host(&address, "localhost");
		address.port = 1234;
		peer = enet_host_connect(m_EnetHost, &address, 2, 0);
		ASSERT(peer, "No available peers for initiating an ENet connection.");

		if (enet_host_service(m_EnetHost, &event, 5000) > 0
			&& event.type == ENET_EVENT_TYPE_CONNECT)
		{
			std::cout << "Connection succeeded." << std::endl;
		}
		else
		{
			enet_peer_reset(peer);
			std::cout << "Connection failed." << std::endl;
		}

		// Init GLFW window

		m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "rmkl", nullptr, nullptr);
		ASSERT(m_Window, "Failed to create window.");

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ASSERT(status, "Failed to initialize Glad.");

		glfwSwapInterval(0);

		// Init imgui

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 410");

		// Init gl

		m_Batch.reset(new Batch());

		// init shader

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec2 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec4 v_Color;

			uniform mat4 u_MVP;

			void main()
			{
				v_Color = a_Color;

				gl_Position = u_MVP * vec4(a_Position, 0.0, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		float h = 6.0f;
		float w = (h / GetWindowHeight()) * GetWindowWidth();
		m_Proj = glm::ortho(0.0f, w, 0.0f, h);

		m_Cam[0] = 0;
		m_Cam[1] = 0;

		ServiceLocator::ProvideInput(&m_PendingInputs);
		ServiceLocator::ProvideInterpolationAlpha(&m_InterpolationAlpha);
		ServiceLocator::ProvidePhysicsTick(&m_Stage->PhysicsTick);
		ServiceLocator::ProvideRtt(&m_EnetHost->peers->roundTripTime);
	}

	GraphicApp::~GraphicApp()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_Window);
	}

	void GraphicApp::SetVsync(bool value)
	{
		m_Vsync = value;
		glfwSwapInterval(m_Vsync ? 1 : 0);
	}

	void GraphicApp::FixedUpdate()
	{
		m_Stage->PhysicsTick++;

		ForceEmitter::UpdateEmitters(*m_Stage);

		if (m_ControlledPj == -1) return;
		Pj& pj = m_Pjs.at(m_ControlledPj);
			
		glm::vec2 rawInput = glm::vec2();
		if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
			rawInput.y += 1.0f;
		if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
			rawInput.x -= 1.0f;
		if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
			rawInput.y -= 1.0f;
		if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
			rawInput.x += 1.0f;

		if (rawInput.x != 0 || rawInput.y != 0)
			rawInput = glm::normalize(rawInput);

		Input input;
		input.X = rawInput.x;
		input.Y = rawInput.y;
		input.Space = glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS;
		input.Tick = m_Stage->PhysicsTick;

		SendInput(input);

		//Do client - side prediction.
		pj.FixedUpdate(input, *m_Stage);

		//Save this input for later reconciliation.
		PjState result = pj.SerializeState(input.Tick); // input.result = 

		m_PendingInputs.emplace_back(input);

		pj.m_History.try_emplace(input.Tick, result);
			
		if (m_Stage->PhysicsTick > 500)
			pj.m_History.erase(m_Stage->PhysicsTick - 500);
	}

	void GraphicApp::SendInput(Input input)
	{
		NetMessage::Type type = NetMessage::Type::Input;
		int count = static_cast<int>(m_PendingInputs.size());

		size_t size = sizeof(NetMessage::Type) + sizeof(int) + (sizeof(Input) * count);
		char* packet = (char*)malloc(size);
		size_t stride = 0;
		NetMessage::pack(packet, stride, type);
		NetMessage::pack(packet, stride, count);
		for (int i = 0; i < count; i++) 
		{
			Input& input = m_PendingInputs.at(i);
			NetMessage::pack<float>(packet, stride, input.X);
			NetMessage::pack<float>(packet, stride, input.Y);
			NetMessage::pack<int>(packet, stride, input.Space);
			NetMessage::pack<int>(packet, stride, input.Tick);
		}

		ENetPacket* p = enet_packet_create(packet, size, ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(m_EnetHost, 0, p);
		free(packet);
	}

	void GraphicApp::OnNetworkConnected(const ENetEvent& e) {}

	void GraphicApp::OnNetworkDisconnected(const ENetEvent& e)
	{
		e.peer->data = nullptr;
	}

	void GraphicApp::OnNetworkReceived(const ENetEvent& e)
	{
		enet_uint8* data = e.packet->data;
		NetMessage::Type type = *(NetMessage::Type*)(data);
		switch (type)
		{
		case NetMessage::Type::StateUpdate:
		{
			size_t stride = sizeof(NetMessage::Type);
			int count = NetMessage::unpack<int>(data, stride);
			for (int i = 0; i < count; i++)
			{
				PjState state{
					NetMessage::unpack<int>(data, stride),
					NetMessage::unpack<int>(data, stride),
					NetMessage::unpack<float>(data, stride),
					NetMessage::unpack<float>(data, stride),
					NetMessage::unpack<float>(data, stride),
					NetMessage::unpack<float>(data, stride),
					NetMessage::unpack<float>(data, stride),
					NetMessage::unpack<float>(data, stride)
				};

				Pj& pj = m_Pjs.at(state.Id);
				if (pj.GetMode() == PjModes::PREDICTED)
					utils::RemoveOldSnapshots(m_PendingInputs, state.Tick);

				pj.UpdateState(state, *m_Stage);
			}
			break;
		}
		case NetMessage::Type::SpawnPj:
		{
			PjSpawnState spawn = NetMessage::unpackOne<PjSpawnState>(data);
			m_Pjs.insert_or_assign(spawn.Id, Pj(spawn.Id, spawn.posX, spawn.posY));
			std::cout << "spawn pj " << spawn.Id << std::endl;
			break;
		}
		case NetMessage::Type::RemovePj:
		{
			int id = NetMessage::unpackOne<int>(data);
			m_Pjs.erase(id);
			std::cout << "remove pj " << id << std::endl;
			break;
		}
		case NetMessage::Type::SetControlledPjId:
		{
			int id = NetMessage::unpackOne<int>(data);

			if (m_ControlledPj != -1)
				m_Pjs.at(m_ControlledPj).SetMode(PjModes::INTERPOLATED);

			m_ControlledPj = id;
			if (m_ControlledPj != -1)
				m_Pjs.at(m_ControlledPj).SetMode(PjModes::PREDICTED);

			std::cout << "set controlled pj " << id << std::endl;
			break;
		}
		case NetMessage::Type::SetSpectatingPjId:
		{
			int id = NetMessage::unpackOne<int>(data);
			m_SpectatingPj = id;
			std::cout << "set spectating pj " << id << std::endl;
			break;
		}
		case NetMessage::Type::SyncTickNumber:
		{
			int tick = NetMessage::unpackOne<int>(data);
			float tickrate = 1.0f / FIXED_UPDATE_FPS;
			float rtt = 0.15f;
			float serverInputBuffer = tickrate;
			float forward = rtt + serverInputBuffer;
			m_Stage->PhysicsTick = tick + (int)(forward / tickrate) + 1;
			std::cout << "sync ticknumber " << std::endl;
			break;
		}
		case NetMessage::Type::Wall:
		{
			break;
		}
		case NetMessage::Type::ForceEmitter:
		{
			break;
		}
		}
	}


	void GraphicApp::Update(double dt) 
	{
	}

	void GraphicApp::Render(double interpolationAlpha)
	{
		glClearColor(0.1f, 0.2f, 0.3f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// clear
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_Shader->Bind();

		m_View = glm::translate(glm::mat4x4(1.0f), glm::vec3(m_Cam[0], m_Cam[1], m_Cam[2]));
		m_Model = glm::translate(glm::mat4x4(1.0f), glm::vec3(0));
		glm::mat4x4 mvp = m_Proj * m_View * m_Model;
		m_Shader->SetUniformMat4f("u_MVP", mvp);


		m_Batch->Begin();
		glm::vec4 wallColour = { 0, 0.5f, 1.0f, 0.5f };
		for (Aabb wall : m_Stage->Walls)
			m_Batch->DrawAabb(wall, wallColour);

		for (ForceEmitter* emitter : m_Stage->ForceEmitters)
			m_Batch->DrawAabb(emitter->GetDebugAabb(m_Stage->PhysicsTick), emitter->GetDebugColour(m_Stage->PhysicsTick));

		for (auto&[id, pj] : m_Pjs)
			pj.Draw(*m_Batch.get(), (float)interpolationAlpha);

		m_Batch->End();


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		RenderImGui();

		ImGui::GetIO().DisplaySize = ImVec2((float)GetWindowWidth(), (float)GetWindowHeight());
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_Window);

		if (glfwWindowShouldClose(m_Window))
			m_Running = false;
	}

	void GraphicApp::RenderImGui() 
	{
		static int targetFps = GetTargetFps();

		ImGui::Begin("settings");

		const char* items[] = { "vsync", "custom", "off" };
		static const char* item_current = items[1];
		if (ImGui::BeginCombo("Limit FPS", item_current, 0))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (item_current == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current = items[n];

					SetLimitFps(n == 1);
					SetVsync(n == 0);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (GetLimitFps())
			ImGui::SliderInt("Frame rate cap", &targetFps, 30, 300);
		if (targetFps != GetTargetFps())
			SetTargetFps(targetFps);

		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Separator();

		ImGui::Text("Physics tick %i", m_Stage->PhysicsTick);
		ImGui::Text("%ims RTT", m_EnetHost->peers->roundTripTime);
		ImGui::Text("%i pending inputs", m_PendingInputs.size());
		ImGui::SliderFloat("Packet loss", &m_PacketLoss, 0.0f, 1.0f);

		ImGui::SliderFloat("ViewX", &m_Cam[0], -7.0f, 14.0f);
		ImGui::SliderFloat("ViewY", &m_Cam[1], -7.0f, 14.0f);

		ImGui::End();
	}
}
