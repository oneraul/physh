#include "ServiceLocator.h"

namespace rmkl {

	std::vector<Input>* ServiceLocator::_input = nullptr;
	float* ServiceLocator::_interpolationAlpha = nullptr;
	int* ServiceLocator::_physicsTick = nullptr;
	enet_uint32* ServiceLocator::_rtt = nullptr;

	std::vector<Input> ServiceLocator::GetInput() { return *_input; }
	void ServiceLocator::ProvideInput(std::vector<Input>* input) { _input = input; }

	float ServiceLocator::GetInterpolationAlpha() { return 0; }// *_interpolationAlpha; } // TODO
	void ServiceLocator::ProvideInterpolationAlpha(float* alpha) { _interpolationAlpha = alpha; }

	int ServiceLocator::GetPhysicsTick() { return 0; }//*_physicsTick; } // TODO
	void ServiceLocator::ProvidePhysicsTick(int* tick) { _physicsTick = tick; }

	enet_uint32 ServiceLocator::GetRtt() { return *_rtt; }
	void ServiceLocator::ProvideRtt(enet_uint32* rtt) { _rtt = rtt; }
}