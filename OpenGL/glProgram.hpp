#pragma once

#include <glm/glm.hpp>
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>

class GLProgram {
	unsigned program;
public:
	GLProgram(const char* vertexShader, const char* fragmentShader);
	GLProgram(GLProgram const&) = delete;
	GLProgram (GLProgram&&) = delete;
	GLProgram operator=(GLProgram const&) = delete;
	GLProgram operator=(GLProgram&&) = delete;

	void use() const noexcept;
	template <glm::qualifier Q>
	void setUniform(const char* name, glm::mat<4, 4, float, Q> const& val) const noexcept;
	template <glm::qualifier Q>
	void setUniform(const char* name, glm::mat<3, 3, float, Q> const& val) const noexcept;
	void setUniform(const char* name, float, float, float, float) const noexcept;
	void setUniform(const char* name, float, float, float) const noexcept;
	void setUniform(const char* name, glm::vec3) const noexcept;
	void setUniform(const char* name, float) const noexcept;
	~GLProgram() noexcept;
};

template<glm::qualifier Q>
inline void GLProgram::setUniform(const char* name, glm::mat<4, 4, float, Q> const& val) const noexcept
{
	glUniformMatrix4fv(glGetUniformLocation(program, name), 1, false, glm::value_ptr(val));
}

template<glm::qualifier Q>
inline void GLProgram::setUniform(const char* name, glm::mat<3, 3, float, Q> const& val) const noexcept
{
	glUniformMatrix3fv(glGetUniformLocation(program, name), 1, false, glm::value_ptr(val));
}
