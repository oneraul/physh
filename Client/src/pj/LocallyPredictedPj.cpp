#include "pch.h"
#include "LocallyPredictedPj.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtx/compatibility.hpp>
#include "Utils.h"
#include "GraphicApp.h"
#include <math.h>

namespace rmkl {

	float LocallyPredictedPj::MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION = 0.5f;

	void LocallyPredictedPj::ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage)
	{
		if (pj.History.find(state.Tick) == pj.History.end()) return;

		// if there was a desync on the tick just received, reconciliate
		glm::vec2 serverPos = state.GetPos();
		glm::vec2 predictedPos = pj.History.find(state.Tick)->GetPos();
		glm::vec2 positionError = serverPos - predictedPos;
		if (glm::length2(positionError) > 1e-8)
		{
			glm::vec2 prevVisualPos = GetRawInterpolatedPos(pj) + m_PosDesync;

			// rewind
			pj.SetState(state);

			// resimulate
			for (const Input& input : GraphicApp::GetPendingInputs())
			{
				int tick = input.Tick;
				pj.FixedUpdate(input, stage);
				pj.History.find(tick)->Update(pj.SerializeState(tick));
			}

			// calculate desync
			glm::vec2 desync = prevVisualPos - GetRawInterpolatedPos(pj);
			float maxDistance2 = MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION * MAX_ERROR_DISTANCE_FOR_SMOOTH_CORRECTION;
			m_PosDesync = glm::length2(desync) >= maxDistance2 ? glm::vec2(0, 0) : desync;
		}
	}

	void LocallyPredictedPj::Update(Pj& pj, float dt)
	{
		if (glm::length2(m_PosDesync) == 0) return;

		float smoothingRate = 0.1f;
		m_PosDesync *= pow(smoothingRate, dt);

		if (glm::length2(m_PosDesync) < 1e-4)
			m_PosDesync = glm::vec2(0, 0);
	}

	glm::vec2 LocallyPredictedPj::GetDrawPos(const Pj& pj) const
	{
		return GetRawInterpolatedPos(pj) + m_PosDesync;
	}

	glm::vec2 LocallyPredictedPj::GetRawInterpolatedPos(const Pj& pj) const
	{
		if (pj.History.size() < 2)
			return pj.GetPos();

		glm::vec2 currPos = pj.GetPos();
		glm::vec2 prevPos = (++pj.History.rbegin())->GetPos();
		float alpha = GraphicApp::GetInterpolationAlpha();
		return utils::Lerp(prevPos, currPos, alpha);
	}
}