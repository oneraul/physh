#include <Common.h>

#include "GraphicApp.h"

rmkl::App* rmkl::CreateApplication()
{
	return new GraphicApp();
}
