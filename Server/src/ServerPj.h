#pragma once

#include "PjCommon.h"

namespace rmkl {

	class ServerPj : public PjCommon
	{
	public:
		ServerPj() = delete;
		ServerPj(float x, float y);
		~ServerPj();
	
	public:
		bool m_ControlledByClient;
		unsigned int m_ClientId;

	private:
		static int s_CreatedEntitiesCount;

	};

}
