#pragma once

#include "Input.h"
#include <enet/enet.h>

namespace rmkl {

	class NetMessage 
	{
	public:

		enum Type : int
		{
			Input = 1,
			SetControlledPjId,
			SetSpectatingPjId,
			SyncTickNumber,
			SpawnPj,
			RemovePj,
			StateUpdate,
			Wall,
			ForceEmitter
		};

		template<typename T>
		static void pack(char* destination, size_t& stride, T& data)
		{
			std::memcpy(destination + stride, &data, sizeof(T));
			stride += sizeof(T);
		}

		template<typename T>
		static T unpack(const enet_uint8* data, size_t& stride)
		{
			T value = *(T*)(data + stride);
			stride += sizeof(T);
			return value;
		}

		template<typename T>
		static T unpackOne(const enet_uint8* data)
		{
			return *(T*)(data + sizeof(Type));
		}


		template<typename T>
		static void SendTo(Type type, const T& payload, ENetPeer* peer)
		{
			size_t size = sizeof(Type) + sizeof(T);
			char* packet = (char*)malloc(size);
			size_t stride = 0;
			pack(packet, stride, type);
			pack(packet, stride, payload);

			ENetPacket* p = enet_packet_create(packet, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
			enet_peer_send(peer, 0, p);

			free(packet);
		}

		template<typename T>
		static void SendToAll(Type type, const T& payload, ENetHost* host)
		{
			size_t size = sizeof(Type) + sizeof(T);
			char* packet = (char*)malloc(size);
			size_t stride = 0;
			pack(packet, stride, type);
			pack(packet, stride, payload);

			ENetPacket* p = enet_packet_create(packet, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
			enet_host_broadcast(host, 0, p);

			free(packet);
		}
	};
}
