#pragma once

#include <enet/enet.h>
#include <vector>
#include "Input.h"

namespace rmkl {

	class Client 
	{
	public:
		Client(ENetPeer* peer);

	public:
		ENetPeer* EnetPeer;
		int Id;
		std::vector<Input> InputBuffer;
		int ControlledPj;
	};

}
