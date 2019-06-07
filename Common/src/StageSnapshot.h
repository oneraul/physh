#pragma once

namespace rmkl {

	class ForceEmitter;

	struct StageSnapshot
	{
		StageSnapshot(int tick) : Tick(tick) {}
		
		int Tick;
		std::vector<ForceEmitter*> Emitters;
	};

}