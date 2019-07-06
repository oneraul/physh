#include "NetworkInterPolatedPj.h"
#include "Utils.h"
#include "GraphicApp.h"

namespace rmkl {

	void NetworkInterpolatedPj::ProcessStateUpdate(Pj& pj, const PjState& state, const Stage& stage)
	{
		pj.History.emplace(state);
	}

	glm::vec2 NetworkInterpolatedPj::GetDrawPos(const Pj& pj) const
	{
		if (pj.History.size() < 2)
			return pj.GetPos();
		
		auto it = pj.History.begin();
		PjState before = *it;
		PjState after = *(++it);

		float tickrate = GraphicApp::GetFixedTickrate();
		float renderTime = CalculateRenderTime();
		float alpha = (renderTime - before.Tick * tickrate) / ((after.Tick - before.Tick) * tickrate);

		return utils::Lerp(before.GetPos(), after.GetPos(), alpha);
	}

	void NetworkInterpolatedPj::Update(Pj& pj, float dt)
	{
		float tickrate = GraphicApp::GetFixedTickrate();
		float renderTime = CalculateRenderTime();

		// remove old state
		while (pj.History.size() > 1)
		{
			auto first = pj.History.begin();
			auto second = ++pj.History.begin();

			if (first->Tick * tickrate < renderTime
				&& second->Tick * tickrate <= renderTime)
				pj.History.erase(first);

			else break;
		}

		// update pj state
		if (pj.History.size() > 0)
		{
			PjState state = *pj.History.begin();
			if (pj.GetPos() != state.GetPos())
				pj.SetState(state);
		}
	}

	float NetworkInterpolatedPj::CalculateRenderTime() const
	{
		/*float rtt = GraphicApp::GetRtt() / 100.0f;
		float interpolationdelay = GraphicApp::GetServerTickrate() * 2 + rtt / 2;
		float currenttime = (GraphicApp::GetTick() + GraphicApp::GetInterpolationAlpha()) * GraphicApp::GetFixedTickrate();
		float rendertime = currenttime - interpolationdelay;*/



		float currenttime = (GraphicApp::GetTick() + GraphicApp::GetInterpolationAlpha()) * GraphicApp::GetFixedTickrate();
		float rendertime = currenttime - 0.3f;
		rendertime = rendertime >= 0 ? rendertime : 0;
		return rendertime;
	}

}
