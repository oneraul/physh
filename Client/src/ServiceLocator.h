#pragma once

#include <vector>
#include "Input.h"
#include <enet/types.h>

namespace rmkl {

	class ServiceLocator 
	{
	public:
		static std::vector<Input> GetInput();
		static void ProvideInput(std::vector<Input>* input);
		static float GetInterpolationAlpha();
		static void ProvideInterpolationAlpha(float* alpha);
		static int GetPhysicsTick();
		static void ProvidePhysicsTick(int* tick);
		static enet_uint32 GetRtt();
		static void ProvideRtt(enet_uint32* rtt);

	private:
		static std::vector<Input>* _input;
		static float* _interpolationAlpha;
		static int* _physicsTick;
		static enet_uint32* _rtt;
	};
}