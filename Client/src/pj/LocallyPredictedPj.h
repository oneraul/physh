#pragma once

#include "Pj.h"

namespace rmkl {
	
	class LocallyPredictedPj : public PjMode
	{
	public:
		LocallyPredictedPj() : m_PosDesync(0) {}
		virtual void UpdateState(Pj& pj, const PjState& state, const Stage& stage) override;
		virtual glm::vec2 GetDrawPos(Pj& pj) override;
		virtual PjModes GetType() const override;

	private:
		glm::vec2 GetRawInterpolatedPos(const Pj& pj);

	private:
		glm::vec2 m_PosDesync;

	private:
		static float MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION;
	};

}
