#include "pch.h"
#include "ServerApp.h"
#include "NetMessage.h"
#include "Utils.h"

namespace rmkl {

	ServerApp::ServerApp()
		: m_Stage(std::make_unique<Stage>()), m_HistoryBufferLength(SERVER_TICKRATE /* 1s */),
		m_UpdateAccumulator(0)
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

	void ServerApp::OnNetworkConnected(const ENetEvent& e) 
	{
		std::cout << "Someone connected" << std::endl;
		auto&[pair, succeed] = Clients.insert_or_assign(e.peer->connectID, Client(e.peer));
		Client& client = pair->second;

		e.peer->data = malloc(sizeof(int));
		memcpy(e.peer->data, &e.peer->connectID, sizeof(unsigned int));

		NetMessage::SendTo(Tp::syncTickNumber, m_Stage->PhysicsTick, client.EnetPeer);

		// update the new client with the info about the other players
		for (auto&[id, pj] : m_Pjs)
		{
			NetMessage::SendTo(MSpawn{ Tp::spawnPj,
				id, pj.m_Body.m_Pos.x, pj.m_Body.m_Pos.y,
				pj.Spritesheet, pj.Palette }, client.EnetPeer);
		}

		// instantiate new pj
		float x = 6.0f, y = 2.0f;
		ServerPj pj(x, y);
		m_Pjs.insert({ pj.m_Id, pj });
		NetMessage::SendToAll(MSpawn{ Tp::spawnPj,
			pj.m_Id, pj.m_Body.m_Pos.x, pj.m_Body.m_Pos.y,
			pj.Spritesheet, pj.Palette }, m_EnetHost);

		SetPjOwnership(pj.m_Id, client.Id);
		NetMessage::SendTo(Tp::setSpectatingPjId, pj.m_Id, client.EnetPeer);

		// TODO walls and emitters
	}

	void ServerApp::OnNetworkDisconnected(const ENetEvent& e)
	{
		unsigned int clientId = *(unsigned int*)e.peer->data;
		if (int pjId = Clients.at(clientId).ControlledPj; pjId != 0)
		{
			m_Pjs.erase(pjId);
			NetMessage::SendToAll(Tp::removePj, pjId, m_EnetHost);
		}
		Clients.erase(clientId);

		e.peer->data = nullptr;
		std::cout << "client " << clientId << " disconnected" << std::endl;
	}

	void ServerApp::OnNetworkReceived(const ENetEvent& e)
	{
		Tp type = *(Tp*)(e.packet->data);
		if (type == Tp::input)
		{
			Input input = NetMessage::DeserializeInput(e.packet->data);
			Clients.at(e.peer->connectID).InputBuffer.emplace_back(input);
		}
	}

	void ServerApp::Update(double dt) 
	{
		m_UpdateAccumulator += dt;
		if (m_UpdateAccumulator >= (1.0f / SERVER_TICKRATE))
		{
			m_UpdateAccumulator -= (1.0f / SERVER_TICKRATE);

			/*local packet = { pjs = {} }
			for _, pj in pairs(pjs) do
				table.insert(packet.pjs, pj:serializeState(physicsTickNumber))
			end

			for _, client in ipairs(server:getClients()) do
				local clientId = client:getConnectId()
				packet.acks = acks[clientId]
				client : send("stateUpdate", packet)
				acks[clientId] = {}
				packet.acks = nil
			end*/
			for (auto&[pjId, pj] : m_Pjs)
			{
				NetMessage::SendToAll(pj.SerializeState(m_Stage->PhysicsTick), m_EnetHost);
			}
		}
	}

	void ServerApp::FixedUpdate()
	{
		m_Stage->PhysicsTick++;
		
		ForceEmitter::UpdateEmitters(*m_Stage);

		const int tick = m_Stage->PhysicsTick;
		for (auto&[id, pj] : m_Pjs)
		{
			if (pj.m_ControlledByClient)
			{
				Client& client = Clients.at(pj.m_ClientId);
				auto it = std::find_if(client.InputBuffer.begin(), client.InputBuffer.end(),
					[=](const Input& input) -> bool { return input.Tick == tick; });

				if (it != client.InputBuffer.end())
				{
					pj.FixedUpdate(*it, *m_Stage);
					pj.m_History.try_emplace(tick, pj.SerializeState(tick));
					client.Acks.emplace_back(tick);
				}
			}

			const int HistoryBufferLength = 60; // 2s, in ticks
			int historyStartsAtTick = m_Stage->PhysicsTick - HistoryBufferLength;
			if (historyStartsAtTick > 0)
				utils::RemoveOldSnapshots(pj.m_History, historyStartsAtTick);
		}
	}

	void ServerApp::Render(double interpolationAlpha) {}

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

		NetMessage::SendTo(Tp::setControlledPjId, pjId, client.EnetPeer);
	}

	void ServerApp::RemovePjOwnership(int pjId, bool sendIdUpdate)
	{
		ServerPj* pj = &m_Pjs.at(pjId);

		if (pj->m_ControlledByClient)
		{
			Client& client = Clients.at(pj->m_ClientId);

			client.ControlledPj = -1;
			if (sendIdUpdate)
				NetMessage::SendTo(Tp::setControlledPjId, -1, client.EnetPeer);

			pj->m_ControlledByClient = false;
			pj->m_ClientId = 0;
		}
	}
}