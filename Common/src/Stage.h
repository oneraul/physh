#pragma once

#include "intersect/Aabb.h"
#include "StageSnapshot.h"
#include <set>

namespace rmkl {

	struct Stage
	{
	public:
		Stage();
		void FixedUpdate();
		inline int GetTick() const { return m_PhysicsTick; }
		inline void SetTick(int tick) { m_PhysicsTick = tick; }

	public:
		std::vector<Aabb> Walls;
		std::vector<ForceEmitter*> ForceEmitters;
		std::set<StageSnapshot, StageSnapshotCompareTick> History;

	private:
		void UpdateEmitters();

	private:
		int m_PhysicsTick;
	};

}
