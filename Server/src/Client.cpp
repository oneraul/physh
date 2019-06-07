#include "pch.h"
#include "Client.h"

namespace rmkl {

	Client::Client(ENetPeer* peer)
		: EnetPeer(peer), Id(peer->connectID), ControlledPj(-1)
	{

	}
}