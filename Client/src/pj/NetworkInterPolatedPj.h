#pragma once

#include "Pj.h"

namespace rmkl {

	class NetworkInterpolatedPj : public PjMode
	{
	public:
		virtual void UpdateState(Pj& pj, PjState& state, Stage& stage) override;
		virtual glm::vec2 GetDrawPos(Pj& pj) override;
		virtual PjModes GetType() override;
	private:
		std::vector<PjState> _history;
	};

}