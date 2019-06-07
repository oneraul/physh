#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace rmkl {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
		void SetUniform4f(const std::string& name, glm::vec4 vec);
		void SetUniformMat4f(const std::string& name, glm::mat4x4 mat);
	private:
		int GetUniformLocation(const std::string& name);
	private:
		int m_RendererID;
		std::unordered_map<std::string, int> m_UniformLocationCache;
	};

}
