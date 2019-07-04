#pragma once

namespace rmkl {

	struct Input
	{
		float X, Y;
		int Space;
		int Tick;

		bool operator < (const Input &other) const { return Tick < other.Tick; }
		friend bool operator == (const Input &a, const Input &b) { return a.Tick == b.Tick; }
	};
}
