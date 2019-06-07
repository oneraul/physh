#pragma once

#include "Input.h"
#include <enet/enet.h>

namespace rmkl {

	enum Tp : int
	{
		input = 1,			// x, y, space, tick

		setControlledPjId,	// id
		setSpectatingPjId,	// id
		syncTickNumber,		// ticknumber
		spawnPj,			// id, x, y, spritesheet, palette
		removePj,			// id
		stateUpdate,		// id, posX, posY, inputVx, InputVy, nonInputVx, nonInputVy, tick
		wall,
		forceEmitter,
	};

	struct MInt { Tp type; int payload; };
	struct MSpawn { Tp type; int id; float x; float y; int spritesheed; int palette; };
	struct MInput { Tp type; Input input; };
	struct MState { Tp type; PjState state; };

	class NetMessage 
	{
	public:
		static inline MState SerializeState(PjState state) { return MState{ Tp::stateUpdate, PjState(state.Tick, state.Id, state.posX, state.posY, state.inputVX, state.inputVY, state.nonInputVX, state.nonInputVY) }; }
		static inline MInput SerializeInput(Input input) { return MInput{ Tp::input, input.X, input.Y, input.Space, input.Tick }; }
		static inline MInt SerializeInt(Tp type, int value) { return MInt{ type, value }; }

		static int DeserializeInt(enet_uint8* data) 
		{
			int stride = sizeof(Tp);
			int value = ExtractInt(data, stride);
			return value;
		}

		static Input DeserializeInput(enet_uint8* data)
		{
			int stride = sizeof(Tp);
			float x = ExtractFloat(data, stride);
			float y = ExtractFloat(data, stride);
			bool space = ExtractBool(data, stride);
			int tick = ExtractInt(data, stride);
			return Input{ x, y, space, tick };
		}

		static MSpawn DeserializeSpawn(enet_uint8* data)
		{
			int stride = sizeof(Tp);
			int id = ExtractInt(data, stride);
			float x = ExtractFloat(data, stride);
			float y = ExtractFloat(data, stride);
			int spritesheet = ExtractInt(data, stride);
			int palette = ExtractInt(data, stride);
			return MSpawn{Tp::spawnPj, id, x, y, spritesheet, palette};
		}

		static PjState DeserializeState(enet_uint8* data)
		{
			int stride = sizeof(Tp);
			int tick = ExtractInt(data, stride);
			int id = ExtractInt(data, stride);
			float x = ExtractFloat(data, stride);
			float y = ExtractFloat(data, stride);
			float inputVx = ExtractFloat(data, stride);
			float inputVy = ExtractFloat(data, stride);
			float nonInputVx = ExtractFloat(data, stride);
			float nonInputVy = ExtractFloat(data, stride);
			return PjState{ tick, id, x, y, inputVx, inputVy, nonInputVx, nonInputVy };
		}

		static int ExtractInt(const enet_uint8* data, int& stride)
		{
			int value = *(int*)(data + stride);
			stride += sizeof(int);
			return value;
		}

		static float ExtractFloat(const enet_uint8* data, int& stride)
		{
			float value = *(float*)(data + stride);
			stride += sizeof(float);
			return value;
		}

		static bool ExtractBool(const enet_uint8* data, int& stride)
		{
			bool value = *(bool*)(data + stride);
			stride += sizeof(int); // it gives wrong results when using sizeof(bool)
			return value;
		}


		static void SendToAll(Tp tp, int value, ENetHost* host)
		{
			MInt m = NetMessage::SerializeInt(tp, value);
			ENetPacket* packet = enet_packet_create(&m, sizeof(MInt), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);
		}

		static void SendTo(Tp tp, int value, ENetPeer* peer)
		{
			MInt m = NetMessage::SerializeInt(tp, value);
			ENetPacket* packet = enet_packet_create(&m, sizeof(MInt), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
		}

		static void Send(Input& input, ENetHost* host)
		{
			MInput mInput = NetMessage::SerializeInput(input);
			ENetPacket* packet = enet_packet_create(&mInput, sizeof(MInput), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);
		}

		static void SendToAll(MSpawn& mSpawn, ENetHost* host)
		{
			ENetPacket* packet = enet_packet_create(&mSpawn, sizeof(MSpawn), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);
		}

		static void SendTo(MSpawn& mSpawn, ENetPeer* peer)
		{
			ENetPacket* packet = enet_packet_create(&mSpawn, sizeof(MSpawn), ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
		}

		static void SendToAll(PjState& state, ENetHost* host)
		{
			MState mState = NetMessage::SerializeState(state);
			ENetPacket* packet = enet_packet_create(&mState, sizeof(MState), ENET_PACKET_FLAG_RELIABLE);
			enet_host_broadcast(host, 0, packet);
		}
	};
}