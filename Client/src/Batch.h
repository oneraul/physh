#pragma once

#include <glm/glm.hpp>
#include "intersect/Aabb.h"

namespace rmkl {

	struct Rect
	{
		float X, Y, W, H;
	};

	class Batch
	{
	public:
		Batch();
		~Batch();
		void Begin();
		void End();
		void Draw(Rect& rect, glm::vec4& colour);
		void DrawAabb(Aabb& aabb, glm::vec4& colour);

	private:
		unsigned int m_SpritesInBatch;
		float m_Vertices[1000 * 4 * (2 + 4)]; //max sprites * vertices per sprite * floats per vertex
		unsigned int m_Indices[1000 * 6]; // max sprites * indices per sprite
		unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;

		struct Vertex
		{
			float Position[2];
			float Color[4];
		};
	};
}
