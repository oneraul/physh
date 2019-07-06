#pragma once

#include "Pj.h"

namespace rmkl {
	
	class LocallyPredictedPj : public PjMode
	{
	public:
		LocallyPredictedPj() : m_PosDesync(0, 0) {}
		virtual PjModes GetType() const override { return PjModes::PREDICTED; }
		virtual void Update(Pj& pj, float dt) override;
		virtual void ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage) override;
		virtual glm::vec2 GetDrawPos(const Pj& pj) const override;

	private:
		glm::vec2 GetRawInterpolatedPos(const Pj& pj) const;

	private:
		glm::vec2 m_PosDesync;

	private:
		static float MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION;
	};

}
