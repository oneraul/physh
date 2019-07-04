#pragma once

#include "Pj.h"

namespace rmkl {

	class NetworkInterpolatedPj : public PjMode
	{
	public:
		virtual void UpdateState(Pj& pj, const PjState& state, const Stage& stage) override;
		virtual glm::vec2 GetDrawPos(Pj& pj) override;
		virtual PjModes GetType() const override;

	private:
		std::vector<PjState> _history;
	};

}
