#pragma once

#include "Pj.h"

namespace rmkl {

	class NetworkInterpolatedPj : public PjMode
	{
	public:
		virtual void Update(Pj& pj, float dt) override;
		virtual PjModes GetType() const override { return PjModes::INTERPOLATED; }
		virtual void ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage) override;
		virtual glm::vec2 GetDrawPos(const Pj& pj) const override;

	private:
		float CalculateRenderTime() const;
	};

}
