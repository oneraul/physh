#include "pch.h"
#include "Batch.h"
#include <glad/glad.h>

namespace rmkl {

	Batch::Batch()
		: m_SpritesInBatch(0), m_Vertices{}, m_Indices{}
	{
		glGenVertexArrays(1, &m_VertexArray);
		glGenBuffers(1, &m_IndexBuffer);
		glGenBuffers(1, &m_VertexBuffer);

		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
	}

	Batch::~Batch()
	{
		glDeleteBuffers(1, &m_IndexBuffer);
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	void Batch::Begin()
	{
	}

	void Batch::End()
	{
		if (m_SpritesInBatch == 0) return;

		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices, GL_DYNAMIC_DRAW);

		int count = m_SpritesInBatch * 6;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

		m_SpritesInBatch = 0;
	}

	void Batch::Draw(Rect& rect, glm::vec4& colour)
	{
		const int floatsPerSprite = 4 * (2 + 4); // vertices per sprite * components per vertex
		const int vertexIndex = m_SpritesInBatch * floatsPerSprite;

		m_Vertices[vertexIndex + 0] = rect.X;
		m_Vertices[vertexIndex + 1] = rect.Y;
		m_Vertices[vertexIndex + 2] = colour.r;
		m_Vertices[vertexIndex + 3] = colour.g;
		m_Vertices[vertexIndex + 4] = colour.b;
		m_Vertices[vertexIndex + 5] = colour.a;

		m_Vertices[vertexIndex + 6] = rect.X + rect.W;
		m_Vertices[vertexIndex + 7] = rect.Y;
		m_Vertices[vertexIndex + 8] = colour.r;
		m_Vertices[vertexIndex + 9] = colour.g;
		m_Vertices[vertexIndex + 10] = colour.b;
		m_Vertices[vertexIndex + 11] = colour.a;

		m_Vertices[vertexIndex + 12] = rect.X + rect.W;
		m_Vertices[vertexIndex + 13] = rect.Y + rect.H;
		m_Vertices[vertexIndex + 14] = colour.r;
		m_Vertices[vertexIndex + 15] = colour.g;
		m_Vertices[vertexIndex + 16] = colour.b;
		m_Vertices[vertexIndex + 17] = colour.a;

		m_Vertices[vertexIndex + 18] = rect.X;
		m_Vertices[vertexIndex + 19] = rect.Y + rect.H;
		m_Vertices[vertexIndex + 20] = colour.r;
		m_Vertices[vertexIndex + 21] = colour.g;
		m_Vertices[vertexIndex + 22] = colour.b;
		m_Vertices[vertexIndex + 23] = colour.a;

		const int first_index_sprite = m_SpritesInBatch * 4;
		const int indexIndex = m_SpritesInBatch * 6;

		m_Indices[indexIndex + 0] = first_index_sprite + 0;
		m_Indices[indexIndex + 1] = first_index_sprite + 1;
		m_Indices[indexIndex + 2] = first_index_sprite + 2;

		m_Indices[indexIndex + 3] = first_index_sprite + 0;
		m_Indices[indexIndex + 4] = first_index_sprite + 2;
		m_Indices[indexIndex + 5] = first_index_sprite + 3;

		m_SpritesInBatch++;
	}

	void Batch::DrawAabb(Aabb& aabb, glm::vec4& colour)
	{
		Rect rect = { aabb.Pos.x - aabb.Half.x, aabb.Pos.y - aabb.Half.y, aabb.Half.x * 2.0f, aabb.Half.y * 2.0f };
		Draw(rect, colour);
	}
}