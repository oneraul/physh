#pragma once

#include <glm/glm.hpp>

namespace rmkl {

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

		inline glm::vec2 GetPos() const { return glm::vec2(posX, posY); }
		inline glm::vec2 GetInputV() const { return glm::vec2(inputVX, inputVY); }
		inline glm::vec2 GetNonInputV() const { return glm::vec2(nonInputVX, nonInputVY); }
	};

	struct PjSpawnState
	{
		int Id;
		float posX, posY;
		int Spritesheet;
		int Palette;
	};
}
