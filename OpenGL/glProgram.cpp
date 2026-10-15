#include "glProgram.hpp"
#include "glad/glad.h"
#include <exception>
#include <string>
#include "utils.h"

using namespace std::string_literals;

GLProgram::GLProgram(const char* vertexShader, const char* fragmentShader)
{
	char infoLog[512];
	int success;

	unsigned vertexHandle = glCreateShader(GL_VERTEX_SHADER), fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
	SCOPE_EXIT(([vertexHandle, fragmentHandle] {
		glDeleteShader(fragmentHandle);
		glDeleteShader(vertexHandle); 
	}));
	glShaderSource(vertexHandle, 1, &vertexShader, nullptr);
	glCompileShader(vertexHandle);
	glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexHandle, 512, nullptr, infoLog);
		throw std::exception(("ERROR::SHADER::VERTEX::COMPILATION_FAILED: "s + infoLog).c_str());
	}

	glShaderSource(fragmentHandle, 1, &fragmentShader, nullptr);
	glCompileShader(fragmentHandle);
	glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentHandle, 512, nullptr, infoLog);
		throw std::exception(("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: "s + infoLog).c_str());
	}

	this->program = glCreateProgram();
	glAttachShader(program, vertexHandle);
	glAttachShader(program, fragmentHandle);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		throw std::exception(("ERROR::SHADER::PROGRAM::LINKING_FAILED: "s + infoLog).c_str());
	}
}

void GLProgram::use() const noexcept
{
	glUseProgram(program);
}

void GLProgram::setUniform(const char* name, float x, float y, float z, float w) const noexcept
{
	glUniform4f(glGetUniformLocation(program, name), x, y, z, w);
}

void GLProgram::setUniform(const char* name, float x, float y, float z) const noexcept
{
	glUniform3f(glGetUniformLocation(program, name), x, y, z);
}

void GLProgram::setUniform(const char* name, glm::vec3 v) const noexcept
{
	setUniform(name, v.x, v.y, v.z);
}

void GLProgram::setUniform(const char* name, float x) const noexcept
{
	glUniform1f(glGetUniformLocation(program, name), x);
}

GLProgram::~GLProgram() noexcept
{
	glDeleteProgram(program);
}
