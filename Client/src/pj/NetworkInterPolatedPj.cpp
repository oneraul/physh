#include "NetworkInterPolatedPj.h"
#include "ServiceLocator.h"
#include "Utils.h"

namespace rmkl {

	PjModes NetworkInterpolatedPj::GetType() { return PjModes::INTERPOLATED; }

	void NetworkInterpolatedPj::UpdateState(Pj& pj, PjState& state, Stage& stage)
	{
		_history.emplace_back(state);
	}

	glm::vec2 NetworkInterpolatedPj::GetDrawPos(Pj& pj)
	{
		int FIXED_UPDATE_FPS = 30;
		float tickrate = 1.0f / FIXED_UPDATE_FPS;
		float SERVER_TICKRATE = 1.0f / 20;
		float interpolationDelay = SERVER_TICKRATE * 2 + 0.15f; // this 0.15 accounts for rtt in clock sync.should be changed
		//auto& snapshots = pj.m_History;

		// calculate the rendering time
		float currentTime = ((*ServiceLocator::GetPhysicsTick()) + (*ServiceLocator::GetInterpolationAlpha())) * tickrate;
		float renderTime = currentTime - interpolationDelay;

		// remove old snapshots
		while (_history.size() > 1)
		{
			if (_history.at(0).Tick * tickrate < renderTime
				&& _history.at(1).Tick * tickrate <= renderTime)
					_history.erase(_history.begin());
			else break;
		}

		if (_history.size() == 0)
		{
			return pj.m_Body.m_Pos;
		}
		else if (_history.size() == 1)
		{
			PjState snapshot = _history.at(0);
			pj.m_Body.SetPos(snapshot.GetPos());
			pj.m_Body.m_InputV = snapshot.GetInputV();
			pj.m_Body.m_NonInputV = snapshot.GetNonInputV();

			return pj.m_Body.m_Pos;
		}
		else
		{
			PjState before = _history.at(0);
			PjState after = _history.at(1);
			float alpha = (renderTime - before.Tick * tickrate) / ((after.Tick - before.Tick) * tickrate);

			pj.m_Body.SetPos(utils::Lerp(before.GetPos(), after.GetPos(), alpha));
			pj.m_Body.m_InputV = utils::Lerp(before.GetInputV(), after.GetInputV(), alpha);
			pj.m_Body.m_NonInputV = utils::Lerp(before.GetNonInputV(), after.GetNonInputV(), alpha);

			return pj.m_Body.m_Pos;
		}
	}
}
