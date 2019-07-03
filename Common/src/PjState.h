#pragma once

#include <glm/glm.hpp>

namespace rmkl {

	struct PjSpawnState
	{
		int Id;
		float posX, posY;
		int Spritesheet;
		int Palette;
	};


	struct PjState
	{
		PjState(int tick, int id, glm::vec2 pos, glm::vec2 inputV, glm::vec2 nonInputV)
			: Tick(tick), Id(id), posX(pos.x), posY(pos.y), inputVX(inputV.x), inputVY(inputV.y), nonInputVX(nonInputV.x), nonInputVY(nonInputV.y) {}

		PjState(int tick, int id, float posx, float posy, float inputVx, float inputVy, float nonInputVx, float nonInputVy)
			: Tick(tick), Id(id), posX(posx), posY(posy), inputVX(inputVx), inputVY(inputVy), nonInputVX(nonInputVx), nonInputVY(nonInputVy) {}

		int Tick;
		int Id;
		float posX, posY;
		float inputVX, inputVY;
		float nonInputVX, nonInputVY;
		// animation
		//currentDir = self.sprite.dir,
		//currentSprite = self.sprite.currentSprite,
		//currentAction = self.sprite.currentAction

		bool operator < (const PjState &other) const { return Tick < other.Tick; }
		bool operator < (int tick) const { return Tick < tick; }
		friend bool operator == (const PjState &a, const PjState &b) { return a.Tick == b.Tick; }
		friend bool operator == (const PjState &state, int tick) { return state.Tick == tick; }
		friend bool operator == (int tick, const PjState &state) { return tick == state.Tick; }

		inline glm::vec2 GetPos() const { return glm::vec2(posX, posY); }
		inline glm::vec2 GetInputV() const { return glm::vec2(inputVX, inputVY); }
		inline glm::vec2 GetNonInputV() const { return glm::vec2(nonInputVX, nonInputVY); }
	};

	struct PjStateCompareTick
	{
		using is_transparent = int;
		bool operator()(PjState const& a, PjState const& b) const { return a.Tick < b.Tick; }
		bool operator()(int tick, PjState const& state) const { return tick < state.Tick; }
		bool operator()(PjState const& state, int tick) const { return state.Tick < tick; }
	};
}
