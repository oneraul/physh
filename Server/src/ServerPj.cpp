#include "ServerPj.h"

namespace rmkl {

	int ServerPj::s_CreatedEntitiesCount = 0;

	ServerPj::ServerPj(float x, float y)
		: PjCommon(++s_CreatedEntitiesCount, Rigidbody(x, y, 1)),
		m_ControlledByClient(false), m_ClientId(0)
	{
	}

	ServerPj::~ServerPj()
	{
	}
}