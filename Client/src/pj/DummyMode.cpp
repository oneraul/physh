#include "pch.h"
#include "DummyMode.h"
#include "Utils.h"
#include "GraphicApp.h"

namespace rmkl {

	void DummyMode::ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage)
	{
	}

	void DummyMode::Update(Pj& pj, float dt)
	{
		if (pj.History.size() < 2)
			return;

		glm::vec2 currPos = pj.GetPos();
		glm::vec2 prevPos = (++pj.History.rbegin())->GetPos();
		m_pos = utils::Lerp(prevPos, currPos, GraphicApp::GetInterpolationAlpha());
	}

	glm::vec2 DummyMode::GetDrawPos(const Pj& pj) const
	{
		return m_pos;
	}
}
