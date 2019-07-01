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

	template<typename T>
	void RemoveOldSnapshots(std::vector<T>& buffer, int tick)
	{
		buffer.erase(std::remove_if(buffer.begin(), buffer.end(), 
			[=](const T& snapshot) -> bool { return snapshot.Tick <= tick; }), 
			buffer.end());
	}

	template<typename T>
	void RemoveOldSnapshots(std::map<int, T>& buffer, int tick)
	{
		std::map<int, T>::iterator it = buffer.begin();
		while (it != buffer.end())
		{
			if (it->first <= tick)
				it = buffer.erase(it);
			else
				it++;
		}
	}

}
}