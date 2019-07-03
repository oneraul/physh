#pragma once

#include "pch.h"

namespace rmkl {

namespace utils {

	inline int secondsToTicks(float seconds) { return (int)(FIXED_UPDATE_FPS * seconds); }
	inline float ticksToSeconds(int ticks) { return ticks * (1.0f / FIXED_UPDATE_FPS); }

	template<typename T>
	inline T Lerp(T a, T b, float alpha) { return b * alpha + a * (1.0f - alpha); }

	template <class T>
	inline int Sign(T value) { return (value > 0) ? 1 : ((value < 0) ? -1 : 0); }
}
}