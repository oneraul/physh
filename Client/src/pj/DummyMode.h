#pragma once

#include "Pj.h"

namespace rmkl {

	class DummyMode : public PjMode
	{
	public:
		DummyMode() : m_pos(0, 0) {}
		virtual void Update(Pj& pj, float dt) override;
		virtual PjModes GetType() const override { return PjModes::DUMMY; }
		virtual void ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage) override;
		virtual glm::vec2 GetDrawPos(const Pj& pj) const override;

	private:
		glm::vec2 m_pos;

	};

}
