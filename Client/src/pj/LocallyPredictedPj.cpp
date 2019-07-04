#include "pch.h"
#include "LocallyPredictedPj.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/compatibility.hpp>
#include "ServiceLocator.h"
#include "Utils.h"

namespace rmkl {

	using glm::vec2;

	PjModes LocallyPredictedPj::GetType() const { return PjModes::PREDICTED; }

	void LocallyPredictedPj::UpdateState(Pj& pj, const PjState& state, const Stage& stage)
	{
		/*pj.m_Body.SetPos(state.GetPos());
		pj.m_Body.m_InputV = state.GetInputV();
		pj.m_Body.m_NonInputV = state.GetNonInputV();

		for (Input& input : ServiceLocator::GetInput())
		{
			int tick = input.Tick;
			pj.ApplyStageSnapshot(tick, stage);
			pj.m_Body.FixedUpdate(input, stage);
			pj.m_History.insert_or_assign(tick, pj.SerializeState(tick));
		}*/
	}

	vec2 LocallyPredictedPj::GetDrawPos(Pj& pj)
	{
		return GetRawInterpolatedPos(pj);
	}

	vec2 LocallyPredictedPj::GetRawInterpolatedPos(const Pj& pj)
	{
		std::vector<Input> pendingInputs = ServiceLocator::GetInput();
		vec2 pos = pj.GetBody().GetPos();

		if (pendingInputs.size() > 1)
		{
			int previousInputTick = pendingInputs.rbegin()->Tick;
			vec2 prevPos = pj.History.find(previousInputTick)->GetPos();
			pos = utils::Lerp(prevPos, pos, ServiceLocator::GetInterpolationAlpha());
		}

		return pos;
	}
}
