#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <stdio.h>
#include <stdint.h>
#include <cstddef>
#include <thread>
#include <chrono>
#include <vector>
#include <map>

#define ASSERT(x, ...) { if(!(x)) { std::cout << "Assertion Failed: " << __VA_ARGS__ << std::endl; __debugbreak(); } }

constexpr auto FIXED_UPDATE_FPS = 30;
constexpr auto SERVER_TICKRATE = 20;
