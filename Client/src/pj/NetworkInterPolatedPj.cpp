#include "NetworkInterPolatedPj.h"
#include "ServiceLocator.h"
#include "Utils.h"

namespace rmkl {

	PjModes NetworkInterpolatedPj::GetType() const { return PjModes::INTERPOLATED; }

	void NetworkInterpolatedPj::UpdateState(Pj& pj, const PjState& state, const Stage& stage)
	{
		_history.emplace_back(state);
		std::sort(_history.begin(), _history.end());
	}

	glm::vec2 NetworkInterpolatedPj::GetDrawPos(Pj& pj)
	{
		int FIXED_UPDATE_FPS = 30;
		float tickrate = 1.0f / FIXED_UPDATE_FPS;
		float SERVER_TICKRATE = 1.0f / 20;
		float rtt = static_cast<int>(ServiceLocator::GetRtt()) / 100.0f;
		float interpolationDelay = SERVER_TICKRATE * 2 + rtt/2;

		// calculate the rendering time
		float currentTime = (ServiceLocator::GetPhysicsTick() + ServiceLocator::GetInterpolationAlpha()) * tickrate;
		float renderTime = currentTime - interpolationDelay;

		// remove old snapshots
		while (_history.size() > 1)
		{
			if (_history.at(0).Tick * tickrate < renderTime
			&&  _history.at(1).Tick * tickrate <= renderTime)
				_history.erase(_history.begin());
			else break;
		}

		if (_history.size() == 0)
		{
			return pj.GetBody().GetPos();
		}
		else if (_history.size() == 1)
		{
			PjState snapshot = _history.at(0);
			pj.GetBody().SetPos(snapshot.GetPos());
			pj.GetBody().m_InputV = snapshot.GetInputV();
			pj.GetBody().m_NonInputV = snapshot.GetNonInputV();

			return pj.GetBody().GetPos();
		}
		else
		{
			PjState before = _history.at(0);
			PjState after = _history.at(1);
			float alpha = (renderTime - before.Tick * tickrate) / ((after.Tick - before.Tick) * tickrate);

			pj.GetBody().SetPos(utils::Lerp(before.GetPos(), after.GetPos(), alpha));
			pj.GetBody().m_InputV = utils::Lerp(before.GetInputV(), after.GetInputV(), alpha);
			pj.GetBody().m_NonInputV = utils::Lerp(before.GetNonInputV(), after.GetNonInputV(), alpha);

			return pj.GetBody().GetPos();
		}
	}
}
