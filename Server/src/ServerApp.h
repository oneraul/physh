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

		void FixedUpdate();
		void Update(double dt);
		void Render(double interpolationAlpha);

	protected:
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
		double m_UpdateAccumulator; // necessary to update at a lower framerate than the physics
	};

}