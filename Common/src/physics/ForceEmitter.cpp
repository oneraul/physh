#include "pch.h"
#include "ForceEmitter.h"
#include "Utils.h"

namespace rmkl {

	void ForceEmitter::UpdateEmitters(Stage& stage)
	{
		const int tick = stage.PhysicsTick;
		auto& history = stage.History;

		for (ForceEmitter* emitter : stage.ForceEmitters)
		{
			if (emitter->Update(tick))
			{
				history.try_emplace(tick, StageSnapshot(tick));
				history.at(tick).Emitters.emplace_back(emitter);
			}
		}

		// remove old snapshots
		const int HistoryBufferLength = 60; // 2s, in ticks
		int historyStartsAtTick = tick - HistoryBufferLength;
		if (historyStartsAtTick > 0)
			utils::RemoveOldSnapshots(history, historyStartsAtTick);
	}

}