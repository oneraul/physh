#include "pch.h"
#include "ServerApp.h"
#include "NetMessage.h"
#include "Utils.h"

namespace rmkl {

	ServerApp::ServerApp()
		: m_Stage(std::make_unique<Stage>())
		, m_HistoryBufferLength(SERVER_TICKRATE /* 1s */)
		, m_UpdateAccumulator(0)
	{
		ASSERT(enet_initialize() == 0, "An error occurred while initializing ENet.");
		
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = 1234;

		m_EnetHost = enet_host_create(&address, 32, 2, 0, 0);
		ASSERT(m_EnetHost, "An error occurred while trying to create an ENet server host.")

		std::cout << "SERVER started" << std::endl;
	}

	ServerApp::~ServerApp() {}

	void ServerApp::PollEvents()
	{
		ProcessNetworkEvents();
	}

	void ServerApp::OnNetworkConnected(const ENetEvent& e) 
	{
		std::cout << "Someone connected" << std::endl;
		auto&[pair, succeed] = Clients.insert_or_assign(e.peer->connectID, Client(e.peer));
		Client& client = pair->second;

		e.peer->data = malloc(sizeof(int));
		memcpy(e.peer->data, &e.peer->connectID, sizeof(unsigned int));

		NetMessage::SendTo(NetMessage::Type::SyncTickNumber, m_Stage->GetTick(), client.EnetPeer);

		// update the new client with the info about the other players
		for (auto&[id, pj] : m_Pjs)
		{
			NetMessage::SendTo(NetMessage::Type::SpawnPj, 
				PjSpawnState { id, pj.GetPos().x, pj.GetPos().y, pj.Spritesheet, pj.Palette },
				client.EnetPeer);
		}

		// instantiate new pj
		float x = 6.0f, y = 2.0f;
		ServerPj pj(x, y);
		m_Pjs.insert({ pj.GetId(), pj });
		NetMessage::SendToAll(NetMessage::Type::SpawnPj,
			PjSpawnState{ pj.GetId(), pj.GetPos().x, pj.GetPos().y, pj.Spritesheet, pj.Palette },
			m_EnetHost);

		SetPjOwnership(pj.GetId(), client.Id);
		NetMessage::SendTo(NetMessage::Type::SetSpectatingPjId, pj.GetId(), client.EnetPeer);

		// TODO walls and emitters
	}

	void ServerApp::OnNetworkDisconnected(const ENetEvent& e)
	{
		unsigned int clientId = *(unsigned int*)e.peer->data;
		if (int pjId = Clients.at(clientId).ControlledPj; pjId != 0)
		{
			m_Pjs.erase(pjId);
			NetMessage::SendToAll(NetMessage::Type::RemovePj, pjId, m_EnetHost);
		}
		Clients.erase(clientId);

		e.peer->data = nullptr;
		std::cout << "client " << clientId << " disconnected" << std::endl;
	}

	void ServerApp::OnNetworkReceived(const ENetEvent& e)
	{
		NetMessage::Type type = *(NetMessage::Type*)(e.packet->data);
		if (type == NetMessage::Type::Input)
		{
			auto& inputBuffer = Clients.at(e.peer->connectID).InputBuffer;

			size_t stride = sizeof(NetMessage::Type);
			int count = NetMessage::unpack<int>(e.packet->data, stride);
			for (int i = 0; i < count; i++)
			{
				inputBuffer.emplace_back(Input{
					NetMessage::unpack<float>(e.packet->data, stride),
					NetMessage::unpack<float>(e.packet->data, stride),
					NetMessage::unpack<int>(e.packet->data, stride),
					NetMessage::unpack<int>(e.packet->data, stride)
				});
			}
			std::sort(inputBuffer.begin(), inputBuffer.end());
			inputBuffer.erase(std::unique(inputBuffer.begin(), inputBuffer.end()), inputBuffer.end());
		}
	}

	void ServerApp::Update(float dt) 
	{
		m_UpdateAccumulator += dt;
		if (m_UpdateAccumulator >= (1.0f / SERVER_TICKRATE))
		{
			m_UpdateAccumulator -= (1.0f / SERVER_TICKRATE);


			std::vector<PjState> _states;
			_states.reserve(m_Pjs.size());
			for (auto&[pjId, pj] : m_Pjs)
				_states.emplace_back(pj.SerializeState(m_Stage->GetTick()));
			int statesCount = static_cast<int>(_states.size());

			NetMessage::Type messageType = NetMessage::Type::StateUpdate;
			for (auto&[clientId, client] : Clients)
			{
				size_t size = sizeof(NetMessage::Type) + sizeof(int) + sizeof(PjState) * statesCount;
				char* packet = (char*)malloc(size);
				size_t stride = 0;
				NetMessage::pack(packet, stride, messageType);
				NetMessage::pack(packet, stride, statesCount);
				for (int i = 0; i < statesCount; i++)
				{
					PjState& state = _states.at(i);
					NetMessage::pack(packet, stride, state.Tick);
					NetMessage::pack(packet, stride, state.Id);
					NetMessage::pack(packet, stride, state.posX);
					NetMessage::pack(packet, stride, state.posY);
					NetMessage::pack(packet, stride, state.inputVX);
					NetMessage::pack(packet, stride, state.inputVY);
					NetMessage::pack(packet, stride, state.nonInputVX);
					NetMessage::pack(packet, stride, state.nonInputVY);
				}

				ENetPacket* p = enet_packet_create(packet, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
				enet_peer_send(client.EnetPeer, 0, p);
				free(packet);
			}

		}
	}

	void ServerApp::FixedUpdate()
	{
		m_Stage->FixedUpdate();

		const int tick = m_Stage->GetTick();
		for (auto&[id, pj] : m_Pjs)
		{
			if (pj.m_ControlledByClient)
			{
				Client& client = Clients.at(pj.m_ClientId);
				auto input = std::find_if(client.InputBuffer.begin(), client.InputBuffer.end(),
					[=](const Input& input) -> bool { return input.Tick == tick; });

				if (input != client.InputBuffer.end())
				{
					pj.FixedUpdate(*input, *m_Stage);
					pj.History.emplace(pj.SerializeState(tick));
				}
			}

			const int HistoryBufferLength = 60; // 2s, in ticks
			int historyStartsAtTick = m_Stage->GetTick() - HistoryBufferLength;
			if (historyStartsAtTick > 0)
				pj.History.erase(pj.History.begin(), pj.History.lower_bound(historyStartsAtTick));
		}
	}

	void ServerApp::Render(float interpolationAlpha) {}

	void ServerApp::SetPjOwnership(int pjId, int clientId)
	{
		Client& client = Clients.at(clientId);
		if (client.ControlledPj != -1)
			RemovePjOwnership(client.ControlledPj, false);


		ServerPj& pj = m_Pjs.at(pjId);
		if (pj.m_ControlledByClient)
			RemovePjOwnership(pjId, true);

		pj.m_ControlledByClient = true;
		pj.m_ClientId = clientId;
		client.ControlledPj = pjId;

		NetMessage::SendTo(NetMessage::Type::SetControlledPjId, pjId, client.EnetPeer);
	}

	void ServerApp::RemovePjOwnership(int pjId, bool sendIdUpdate)
	{
		ServerPj* pj = &m_Pjs.at(pjId);

		if (pj->m_ControlledByClient)
		{
			Client& client = Clients.at(pj->m_ClientId);

			client.ControlledPj = -1;
			if (sendIdUpdate)
				NetMessage::SendTo(NetMessage::Type::SetControlledPjId, -1, client.EnetPeer);

			pj->m_ControlledByClient = false;
			pj->m_ClientId = 0;
		}
	}
}
