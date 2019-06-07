#pragma once

#include "intersect/Aabb.h"
#include "StageSnapshot.h"
#include <map>

namespace rmkl {

	struct Stage
	{
	public:
		Stage();

		int PhysicsTick;
		std::vector<Aabb> Walls;
		std::vector<ForceEmitter*> ForceEmitters;
		std::map<int, StageSnapshot> History;
	};
}