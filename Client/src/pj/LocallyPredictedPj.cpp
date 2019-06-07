#include "pch.h"
#include "LocallyPredictedPj.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/compatibility.hpp>
#include "ServiceLocator.h"
#include "Utils.h"

namespace rmkl {

	using glm::vec2;

	float LocallyPredictedPj::MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION = 2.0f;

	PjModes LocallyPredictedPj::GetType() { return PjModes::PREDICTED; }

	void LocallyPredictedPj::UpdateState(Pj& pj, PjState& state, Stage& stage)
	{
		if (pj.m_History.count(state.Tick) == 0) return;

		Rigidbody& body = pj.m_Body;
		std::vector<Input>* pendingInputs = ServiceLocator::GetInput();

		// if there was a desync on the tick jsut received, reconciliate
		vec2 serverPos = vec2{ state.posX, state.posY };
		vec2 predictedPos = pj.m_History.at(state.Tick).GetPos();
		vec2 positionError = serverPos - predictedPos;
		if (glm::length2(positionError) > 1e-8)
		{
			vec2 prevVisualPos = GetRawInterpolatedPos(pj) + m_PosDesync;

			// rewind
			body.SetPos(serverPos);
			body.m_InputV = vec2{ state.inputVX, state.inputVY };
			body.m_NonInputV = vec2{ state.nonInputVX, state.nonInputVY };

			// resimulate
			for (Input& input : *pendingInputs)
			{
				int tick = input.Tick;
				pj.ApplyStageSnapshot(tick, stage);
				body.FixedUpdate(input, stage);
				pj.m_History.insert_or_assign(tick, pj.SerializeState(tick));
			}

			// calculate desync
			vec2 desync = prevVisualPos - GetRawInterpolatedPos(pj);
			float maxDistance2 = MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION * MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION;
			m_PosDesync = glm::length2(desync) >= maxDistance2 ? vec2(0, 0) : desync;
		}
	}

	glm::vec2 LocallyPredictedPj::GetDrawPos(Pj& pj)
	{
		m_PosDesync *= 0.9;
		if (glm::length2(m_PosDesync) < 1e-4)
			m_PosDesync = vec2(0, 0);

		return GetRawInterpolatedPos(pj) + m_PosDesync;
	}

	glm::vec2 LocallyPredictedPj::GetRawInterpolatedPos(Pj& pj)
	{
		std::vector<Input>* pendingInputs = ServiceLocator::GetInput();
		vec2 pos = pj.m_Body.m_Pos;

		if (pendingInputs->size() > 1)
		{
			unsigned int previousInputTick = pendingInputs->at(pendingInputs->size() - 1).Tick;
			vec2 prevPos = pj.m_History.at(previousInputTick).GetPos();
			pos = utils::Lerp(prevPos, pos, *ServiceLocator::GetInterpolationAlpha());
		}

		return pos;
	}
}