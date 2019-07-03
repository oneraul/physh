#include "pch.h"
#include "Utils.h"
#include "Stage.h"
#include "physics/RectForceEmitterContinuous.h"
#include "physics/RectForceEmitterPulsating.h"

namespace rmkl {

	Stage::Stage() 
		: m_PhysicsTick(0)
	{
		float o = 1.0f; //offset
		float s = 2.0f; //space between
		float w = 1.0f; //width

		Walls.emplace_back(Aabb::CreateWithMaxMin(glm::vec2(0, 0), glm::vec2(9.8f, 1.0f)));
		Walls.emplace_back(Aabb::CreateWithMaxMin(glm::vec2(0, 1.0f), glm::vec2(1.0f, 9.0f)));

		ForceEmitters.emplace_back(new RectForceEmitterContinuous(
			Aabb(glm::vec2(o + w * 0 + s * 1, o + w * 0 + s * 1), glm::vec2(w, w)),
			glm::vec2(1.0f, 0), 40.0f));
		
		ForceEmitters.emplace_back(new RectForceEmitterPulsating(
			Aabb(glm::vec2(o + w * 0 + s * 1, o + w * 1 + s * 2), glm::vec2(w, w)),
			glm::vec2(1, 0), 15.0f, 
			utils::secondsToTicks(1.5f), 0));
	}

	void Stage::FixedUpdate()
	{
		m_PhysicsTick++;
		UpdateEmitters();
	}

	void Stage::UpdateEmitters()
	{
		for (ForceEmitter* emitter : ForceEmitters)
		{
			if (emitter->Update(m_PhysicsTick))
			{
				auto[snapshot, inserted] = History.emplace(StageSnapshot(m_PhysicsTick));
				//snapshot->Emitters.emplace_back(emitter); // TODO ------------------------------------
			}
		}

		// remove old snapshots
		const int HistoryBufferLength = 60; // 2s, in ticks
		int historyStartsAtTick = m_PhysicsTick - HistoryBufferLength;
		if (historyStartsAtTick > 0)
			History.erase(History.begin(), History.lower_bound(historyStartsAtTick));
	}
}