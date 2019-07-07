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
#include "Utils.h"
#include <random>

namespace rmkl {

	GraphicApp* GraphicApp::_instance = nullptr;

	GraphicApp* GraphicApp::GetInstance()
	{
		if (_instance == nullptr)
			_instance = new GraphicApp();
		return _instance;
	}


	GraphicApp::GraphicApp()
		: m_WindowWidth(800)
		, m_WindowHeight(600)
		, m_Vsync(0)
		, m_Stage(std::make_unique<Stage>())
		, m_ControlledPj(-1)
		, m_SpectatingPj(-1)
		, m_PacketLoss(0.2f)
		, m_Cam(800, 600, 6)
		, _rtt(0)
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

		ASSERT(glfwInit(), "Failed to initialize GLFW");
		glfwSetErrorCallback([](int error, const char* description) {
			std::cout << "GLFW Error (" << error << "): " << description << std::endl;
		});

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

		m_Pjs.try_emplace( 42, Pj{  42, 2, 1.5f });
		m_Pjs.try_emplace( 69, Pj{  69, 2, 1.5f });
		m_Pjs.try_emplace(117, Pj{ 117, 2, 1.5f });
		m_Pjs.at(42).SetMode(PjModes::DUMMY);
		m_Pjs.at(69).SetMode(PjModes::PREDICTED);
		m_ControlledPj = 42;

		int i = pings.size();
		pings.try_emplace(i, std::chrono::high_resolution_clock::now());
		NetMessage::SendToAll(NetMessage::Type::Ping, i, m_EnetHost);
	}

	GraphicApp::~GraphicApp()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(m_Window);

		glfwTerminate();
	}

	void GraphicApp::SetVsync(bool value)
	{
		if (m_Vsync != value)
		{
			m_Vsync = value;
			glfwSwapInterval(m_Vsync ? 1 : 0);
		}
	}

	void GraphicApp::FixedUpdate()
	{
		glfwPollEvents();

		m_Stage->FixedUpdate();

		//if (m_ControlledPj == -1) return;
		//Pj& pj = m_Pjs.at(m_ControlledPj);
		//	
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

		//Input input {
		//	rawInput.x,
		//	rawInput.y,
		//	glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS,
		//	m_Stage->GetTick()
		//};

		//SendInput(input);

		////Do client - side prediction.
		//pj.FixedUpdate(input, *m_Stage);

		////Save this input for later reconciliation.
		//m_PendingInputs.emplace_back(input);
		//pj.History.emplace(pj.SerializeState(input.Tick));

		//if (m_Stage->GetTick() > 500)
		//	pj.History.erase(pj.History.begin(), pj.History.lower_bound(m_Stage->GetTick() - 500));


		int tick = m_Stage->GetTick();
		Input dummyInput = Input{ rawInput.x, rawInput.y, 0, tick };
		Input localInput = Input{ rawInput.x, rawInput.y, 0, tick };
		if ((tick % 30) < 30 * m_PacketLoss && m_PendingInputs.size() > 0)
		{
			auto _pastInput = *m_PendingInputs.rbegin();
			localInput.X = _pastInput.X;
			localInput.Y = _pastInput.Y;
		}
		m_PendingInputs.emplace_back(localInput);

		Pj& dummyPj = m_Pjs.at(42);
		dummyPj.FixedUpdate(dummyInput, *m_Stage);
		dummyPj.History.erase(dummyPj.History.begin(), dummyPj.History.lower_bound(tick - 30));
		dummyPj.History.emplace(dummyPj.SerializeState(tick));

		SendInput(dummyInput);

		Pj& localPj = m_Pjs.at(69);
		localPj.FixedUpdate(localInput, *m_Stage);
		localPj.History.erase(localPj.History.begin(), localPj.History.lower_bound(tick - 30));
		localPj.History.emplace(localPj.SerializeState(tick));

		// simulate receive update from server
		static int doit = 0;
		if (++doit % 3 > 0)
		{
			auto state = dummyPj.History.find(tick-7);
			if (state != dummyPj.History.end())
			{
				m_PendingInputs.erase(std::remove_if(m_PendingInputs.begin(), m_PendingInputs.end(),
					[=](const Input& input) { return input.Tick <= state->Tick; }),
					m_PendingInputs.end());

				localPj.ProcessStateUpdate(*state, *m_Stage);
				m_Pjs.at(117).ProcessStateUpdate(*state, *m_Stage);
			}
		}
	}

	void GraphicApp::SendInput(Input input)
	{

		NetMessage::Type type = NetMessage::Type::Input;
		int pingNumber = pings.size();
		int count = static_cast<int>(m_PendingInputs.size());

		size_t size = sizeof(NetMessage::Type) + sizeof(int) * 2 + (sizeof(Input) * count);
		char* packet = (char*)malloc(size);
		size_t stride = 0;
		NetMessage::pack(packet, stride, type);
		NetMessage::pack(packet, stride, pingNumber);
		NetMessage::pack(packet, stride, count);
		for (int i = 0; i < count; i++) 
		{
			Input& input = m_PendingInputs.at(i);
			NetMessage::pack<float>(packet, stride, input.X);
			NetMessage::pack<float>(packet, stride, input.Y);
			NetMessage::pack<int>(packet, stride, input.Space);
			NetMessage::pack<int>(packet, stride, input.Tick);
		}

		ENetPacket* p = enet_packet_create(packet, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
		enet_host_broadcast(m_EnetHost, 0, p);
		free(packet);

		pings.try_emplace(pingNumber, std::chrono::high_resolution_clock::now());
	}

	void GraphicApp::OnNetworkConnected(const ENetEvent& e) {}

	void GraphicApp::OnNetworkDisconnected(const ENetEvent& e)
	{
		e.peer->data = nullptr;
	}

	void GraphicApp::UpdateRtt(int ms)
	{
		if (_rtt == 0)
			_rtt = ms;
		else
			_rtt = _rtt * 0.9f + ms * 0.1f;
	}

	void GraphicApp::OnNetworkReceived(const ENetEvent& e)
	{
		enet_uint8* data = e.packet->data;
		NetMessage::Type type = *(NetMessage::Type*)(data);
		if (type == NetMessage::Type::Ping)
		{
			auto t1 = std::chrono::high_resolution_clock::now();
			int i = NetMessage::unpackOne<int>(data);
			auto rtt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - pings.at(i));
			UpdateRtt(rtt.count());
		}




		//enet_uint8* data = e.packet->data;
		//NetMessage::Type type = *(NetMessage::Type*)(data);
		//switch (type)
		//{
		//case NetMessage::Type::StateUpdate:
		//{
		//	size_t stride = sizeof(NetMessage::Type);
		//	int count = NetMessage::unpack<int>(data, stride);
		//	for (int i = 0; i < count; i++)
		//	{
		//		PjState state{
		//			NetMessage::unpack<int>(data, stride),
		//			NetMessage::unpack<int>(data, stride),
		//			NetMessage::unpack<float>(data, stride),
		//			NetMessage::unpack<float>(data, stride),
		//			NetMessage::unpack<float>(data, stride),
		//			NetMessage::unpack<float>(data, stride),
		//			NetMessage::unpack<float>(data, stride),
		//			NetMessage::unpack<float>(data, stride)
		//		};

		//		Pj& pj = m_Pjs.at(state.Id);
		//		if (pj.GetMode() == PjModes::PREDICTED)
		//		{
		//			m_PendingInputs.erase(std::remove_if(m_PendingInputs.begin(), m_PendingInputs.end(),
		//				[=](const Input& input) { return input.Tick <= state.Tick; }),
		//				m_PendingInputs.end());

		//			/*m_PendingInputs.erase(m_PendingInputs.begin(), 
		//				std::lower_bound(m_PendingInputs.begin(), m_PendingInputs.end(), 
		//					[=](const Input& input) { return input.Tick <= state.Tick; }));*/
		//		}

		//		pj.ProcessStateUpdate(state, *m_Stage);
		//	}
		//	break;
		//}
		//case NetMessage::Type::SpawnPj:
		//{
		//	PjSpawnState spawn = NetMessage::unpackOne<PjSpawnState>(data);
		//	m_Pjs.insert_or_assign(spawn.Id, Pj(spawn.Id, spawn.posX, spawn.posY));
		//	std::cout << "spawn pj " << spawn.Id << std::endl;
		//	break;
		//}
		//case NetMessage::Type::RemovePj:
		//{
		//	int id = NetMessage::unpackOne<int>(data);
		//	m_Pjs.erase(id);
		//	std::cout << "remove pj " << id << std::endl;
		//	break;
		//}
		//case NetMessage::Type::SetControlledPjId:
		//{
		//	int id = NetMessage::unpackOne<int>(data);

		//	if (m_ControlledPj != -1)
		//		m_Pjs.at(m_ControlledPj).SetMode(PjModes::INTERPOLATED);

		//	m_ControlledPj = id;
		//	if (m_ControlledPj != -1)
		//		m_Pjs.at(m_ControlledPj).SetMode(PjModes::PREDICTED);

		//	std::cout << "set controlled pj " << id << std::endl;
		//	break;
		//}
		//case NetMessage::Type::SetSpectatingPjId:
		//{
		//	int id = NetMessage::unpackOne<int>(data);
		//	m_SpectatingPj = id;
		//	std::cout << "set spectating pj " << id << std::endl;
		//	break;
		//}
		//case NetMessage::Type::SyncTickNumber:
		//{
		//	int tick = NetMessage::unpackOne<int>(data);
		//	float tickrate = 1.0f / 10; //FIXED_UPDATE_FPS;
		//	float rtt = 0.15f; // TODO ------------------------------------------------------------
		//	float serverInputBuffer = tickrate;
		//	float forward = rtt + serverInputBuffer;
		//	m_Stage->SetTick(tick + (int)(forward / tickrate) + 1);
		//	std::cout << "sync ticknumber " << std::endl;
		//	break;
		//}
		//case NetMessage::Type::Wall:
		//{
		//	break;
		//}
		//case NetMessage::Type::ForceEmitter:
		//{
		//	break;
		//}
		//}
	}


	void GraphicApp::Update(float dt)
	{
		for (auto&[_, pj] : m_Pjs)
			pj.Update(dt);
	}

	void GraphicApp::Render(float interpolationAlpha)
	{
		glClearColor(0.1f, 0.2f, 0.3f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// clear
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (m_ControlledPj != -1)
			m_Cam.SetPosition({ m_Pjs.at(m_ControlledPj).GetDrawPos() - glm::vec2(4, 3), 0 });

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", m_Cam.GetViewProjectionMatrix());

		m_Batch->Begin();
		glm::vec4 wallColour = { 0, 0.5f, 1.0f, 0.5f };
		for (Aabb wall : m_Stage->Walls)
			m_Batch->DrawAabb(wall, wallColour);

		for (ForceEmitter* emitter : m_Stage->ForceEmitters)
			m_Batch->DrawAabb(emitter->GetDebugAabb(m_Stage->GetTick()), emitter->GetDebugColour(m_Stage->GetTick()));

		for (auto&[id, pj] : m_Pjs)
			pj.Draw(*m_Batch.get());

		m_Batch->End();


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		RenderImGui();

		ImGui::GetIO().DisplaySize = ImVec2((float)m_WindowWidth, (float)m_WindowHeight);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_Window);

		if (glfwWindowShouldClose(m_Window))
			m_Running = false;
	}

	void GraphicApp::RenderImGui() 
	{
		//static int targetFps = GetTargetFps();

		ImGui::Begin("settings");

		/*const char* items[] = { "vsync", "custom", "off" };
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
			SetTargetFps(targetFps);*/

		static bool vsync = false;
		if(ImGui::Checkbox("VSync", &vsync))
			SetVsync(vsync);

		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Separator();

		ImGui::Text("Physics tick %i", m_Stage->GetTick());
		ImGui::Text("enet %ims RTT", GetRtt());
		ImGui::Text("own  %ims RTT", _rtt);
		ImGui::Text("%i pending inputs", m_PendingInputs.size());
		ImGui::SliderFloat("Packet loss", &m_PacketLoss, 0.0f, 1.0f);

		ImGui::End();
	}
}
