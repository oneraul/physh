#pragma once

#include <vector>
#include "Input.h"

namespace rmkl {

	class ServiceLocator 
	{
	public:
		static std::vector<Input>* GetInput();
		static void ProvideInput(std::vector<Input>* input);
		static float* GetInterpolationAlpha();
		static void ProvideInterpolationAlpha(float* alpha);
		static int* GetPhysicsTick();
		static void ProvidePhysicsTick(int* tick);

	private:
		static std::vector<Input>* _input;
		static float* _interpolationAlpha;
		static int* _physicsTick;
	};
}