#include <Common.h>
#include "ServerApp.h"

rmkl::App* rmkl::CreateApplication()
{
	return new ServerApp();
}
