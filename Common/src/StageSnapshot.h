#pragma once

namespace rmkl {

	class ForceEmitter;

	struct StageSnapshot
	{
		StageSnapshot(int tick) : Tick(tick) {}
		
		int Tick;
		std::vector<ForceEmitter*> Emitters;


		bool operator < (const StageSnapshot &other) const { return Tick < other.Tick; }
		bool operator < (int tick) const { return Tick < tick; }
		friend bool operator == (const StageSnapshot &a, const StageSnapshot &b) { return a.Tick == b.Tick; }
		friend bool operator == (const StageSnapshot &state, int tick) { return state.Tick == tick; }
		friend bool operator == (int tick, const StageSnapshot &state) { return tick == state.Tick; }
	};

	struct StageSnapshotCompareTick
	{
		using is_transparent = int;
		bool operator()(StageSnapshot const& a, StageSnapshot const& b) const { return a.Tick < b.Tick; }
		bool operator()(int tick, StageSnapshot const& state) const { return tick < state.Tick; }
		bool operator()(StageSnapshot const& state, int tick) const { return state.Tick < tick; }
	};
}