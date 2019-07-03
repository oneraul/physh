#pragma once

#include "App.h"
#include <enet/enet.h>
#include <memory>
#include <unordered_map>
#include "ServerPj.h"
#include "Client.h"

namespace rmkl {

	class ServerApp : public rmkl::App
	{
	public:

		ServerApp();
		~ServerApp();

	protected:
		virtual void PollEvents() override;
		virtual void FixedUpdate() override;
		virtual void Update(float dt) override;
		virtual void Render(float interpolationAlpha) override;
		virtual void OnNetworkConnected(const ENetEvent& e) override;
		virtual void OnNetworkDisconnected(const ENetEvent& e) override;
		virtual void OnNetworkReceived(const ENetEvent& e) override;

	private:
		void SetPjOwnership(int pjId, int clientId);
		void RemovePjOwnership(int pjId, bool sendIdUpdate);

	private:
		std::unique_ptr<Stage> m_Stage;
		std::unordered_map<unsigned int, Client> Clients;
		std::unordered_map<int, ServerPj> m_Pjs;
		int m_HistoryBufferLength;
		float m_UpdateAccumulator; // necessary to update at a lower framerate than the physics
	};

}