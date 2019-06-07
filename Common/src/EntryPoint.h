#pragma once

#include "App.h"

extern rmkl::App* rmkl::CreateApplication();

int main(int argc, char** argv)
{
	auto app = rmkl::CreateApplication();
	app->Run();
	delete app;
	return 0;
}
