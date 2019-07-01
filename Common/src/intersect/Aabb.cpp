#include "pch.h"
#include "Utils.h"
#include "Aabb.h"
#include <glm/geometric.hpp>

namespace rmkl {

	using glm::vec2;

	Aabb::Aabb(vec2 pos, vec2 half)
		: Pos(pos), Half(half)
	{
		ASSERT(half.x != 0, "the size of the aabb cannot be 0")
		ASSERT(half.y != 0, "the size of the aabb cannot be 0")
	}

	Aabb Aabb::CreateWithMaxMin(vec2 min, vec2 max)
	{
		//	min  _________
		//		|    .    |
		//		|_________|
		//					max

		vec2 hs = max - min;
		hs *= 0.5;
		vec2 center = min + hs;
		return Aabb(center, hs);
	}

	Aabb::~Aabb()
	{
		//TODO
	}

	Hit Aabb::IntersectPoint(vec2 point) 
	{
		Hit hit;

		float dx = point.x - this->Pos.x;
		float px = this->Half.x - std::fabs(dx);
		if (px <= 0)
			return hit;

		float dy = point.y - this->Pos.y;
		float py = this->Half.y - std::fabs(dy);
		if (py <= 0)
			return hit;

		hit.Intersects = true;
		if (px < py)
		{
			int sx = utils::Sign(dx);
			hit.Delta.x = px * sx;
			hit.Normal.x = sx;
			hit.Pos.x = this->Pos.x + this->Half.x * sx;
			hit.Pos.y = point.y;
		}
		else
		{
			int sy = utils::Sign(dy);
			hit.Delta.y = py * sy;
			hit.Normal.y = sy;
			hit.Pos.x = point.x;
			hit.Pos.y = this->Pos.y + this->Half.y * sy;
		}
		return hit;
	}

	Hit Aabb::IntersectSegment(vec2 pos, vec2 delta, float paddingX = 0.0f, float paddingY = 0.0f) 
	{
		Hit hit;

		float scaleX = 1.0f / delta.x;
		float scaleY = 1.0f / delta.y;
		int signX = utils::Sign(scaleX);
		int signY = utils::Sign(scaleY);
		float nearTimeX = (this->Pos.x - signX * (this->Half.x + paddingX) - pos.x) * scaleX;
		float nearTimeY = (this->Pos.y - signY * (this->Half.y + paddingY) - pos.y) * scaleY;
		float farTimeX = (this->Pos.x + signX * (this->Half.x + paddingX) - pos.x) * scaleX;
		float farTimeY = (this->Pos.y + signY * (this->Half.y + paddingY) - pos.y) * scaleY;
		if (nearTimeX > farTimeY || nearTimeY > farTimeX)
			return hit;

		float nearTime = nearTimeX > nearTimeY ? nearTimeX : nearTimeY;
		float farTime = farTimeX < farTimeY ? farTimeX : farTimeY;
		if (nearTime >= 1.0f || farTime <= 0)
			return hit;

		hit.Intersects = true;
		hit.Time = std::clamp(nearTime, 0.0f, 1.0f);
		if (nearTimeX > nearTimeY)
		{
			hit.Normal.x = -signX;
			hit.Normal.y = 0;
		}
		else
		{
			hit.Normal.x = 0;
			hit.Normal.y = -signY;
		}
		hit.Delta.x = (1.0f - hit.Time) * -delta.x;
		hit.Delta.y = (1.0f - hit.Time) * -delta.y;
		hit.Pos.x = pos.x + delta.x * hit.Time;
		hit.Pos.y = pos.y + delta.y * hit.Time;

		return hit;
	}

	Hit Aabb::IntersectAabb(Aabb& box) 
	{
		Hit hit;

		float dx = box.Pos.x - this->Pos.x;
		float px = box.Half.x + this->Half.x - std::fabs(dx);
		if (px <= 0)
			return hit;

		float dy = box.Pos.y - this->Pos.y;
		float py = box.Half.y + this->Half.y - std::fabs(dy);
		if (py <= 0)
			return hit;

		hit.Intersects = true;
		hit.Collider = &box;
		if (px < py) 
		{
			int sx = utils::Sign(dx);
			hit.Delta.x = px * sx;
			hit.Normal.x = sx;
			hit.Pos.x = this->Pos.x + this->Half.x * sx;
			hit.Pos.y = box.Pos.y;
		}
		else
		{
			int sy = utils::Sign(dy);
			hit.Delta.y = py * sy;
			hit.Normal.y = sy;
			hit.Pos.x = box.Pos.x;
			hit.Pos.y = this->Pos.y + this->Half.y * sy;
		}
		return hit;
	}

	Sweep Aabb::SweepAabb(Aabb& box, vec2 delta) 
	{
		Sweep sweep;
		if (delta.x == 0 && delta.y == 0)
		{
			sweep.Pos.x = box.Pos.x;
			sweep.Pos.y = box.Pos.y;
			sweep.Hit = this->IntersectAabb(box);
			if (sweep.Hit.Intersects) 
			{
				sweep.Time = 0;
				sweep.Hit.Time = 0;
			}
			else
			{
				sweep.Time = 1.0f;
			}
			return sweep;
		}

		sweep.Hit = this->IntersectSegment(box.Pos, delta, box.Half.x, box.Half.y);
		if (sweep.Hit.Intersects)
		{
			sweep.Time = std::clamp((float)(sweep.Hit.Time - 1e-4), 0.0f, 1.0f);
			sweep.Pos.x = box.Pos.x + delta.x * sweep.Time;
			sweep.Pos.y = box.Pos.y + delta.y * sweep.Time;
			vec2 direction = glm::normalize(delta);
			sweep.Hit.Pos.x = std::clamp(
				sweep.Hit.Pos.x + direction.x * box.Half.x,
				this->Pos.x - this->Half.x, this->Pos.x + this->Half.x);
			sweep.Hit.Pos.y = std::clamp(
				sweep.Hit.Pos.y + direction.y * box.Half.y,
				this->Pos.y - this->Half.y, this->Pos.y + this->Half.y);
		}
		else
		{
			sweep.Pos.x = box.Pos.x + delta.x;
			sweep.Pos.y = box.Pos.y + delta.y;
			sweep.Time = 1.0f;
		}
		return sweep;
	}

	Sweep Aabb::SweepInto(std::vector<Aabb> staticColliders, vec2 delta)
	{
		Sweep nearest;
		nearest.Time = 1.0f;
		nearest.Pos.x = this->Pos.x + delta.x;
		nearest.Pos.y = this->Pos.y + delta.y;
		for (Aabb collider : staticColliders)
		{
			Sweep sweep = collider.SweepAabb(*this, delta);
			if (sweep.Time < nearest.Time)
			{
				nearest = sweep;
			}
		}
		return nearest;
	}
}