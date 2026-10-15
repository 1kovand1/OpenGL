#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glprogram.hpp"
#include "glad/glad.h"

template <unsigned resolution>
class Sphere {
	static inline unsigned VBO, EBO, VAO;
	static inline size_t counter{};
	static inline GLProgram* shaderProgram = nullptr;
	static inline GLProgram* blackBodyShaderProgram = nullptr;
	static void initStatic() noexcept;
	static void cleanStatic() noexcept;

	glm::mat4 model;
	glm::mat3 modelUniform;
	float R;
	glm::vec3 pos;
public:
	static constexpr size_t cordsLength = 3 * (resolution * (resolution - 2) + 2);
	static constexpr size_t indicesLength = 3 * (2 * resolution * (resolution - 2));

	Sphere(float radius = 1.f, glm::vec3 pos = glm::vec3()) noexcept;
	~Sphere() noexcept;

	void Draw(Camera const& camera, glm::vec3 lightSource, float ambient) const noexcept;
	void Draw(Camera const& camera) const noexcept;
	
	void setPosition(glm::vec3 pos = glm::vec3()) noexcept;
	void setRadius(float radius = 1.0f) noexcept;
};

template<unsigned resolution>
inline void Sphere<resolution>::initStatic() noexcept
{
	std::vector<float> cords;
	cords.reserve(cordsLength);
	cords.push_back(0);
	cords.push_back(0);
	cords.push_back(-1);
	for (int i = 1; i < resolution - 1; ++i)
		for (int j = 0; j < resolution; ++j)
		{
			const float phi = j * 2 * glm::pi<float>() / resolution,
				theta = -glm::pi<float>() / 2 + i * glm::pi<float>() / (resolution - 1);
			cords.push_back(cosf(phi) * cosf(theta));
			cords.push_back(sinf(phi) * cosf(theta));
			cords.push_back(sinf(theta));
		}
	cords.push_back(0);
	cords.push_back(0);
	cords.push_back(1);

	//auto indices = std::make_unique<unsigned[]>(indicesLength);
	std::vector<unsigned> indices;
	indices.reserve(indicesLength);
	for (int j = 0; j < resolution; ++j)
	{
		indices.push_back(0);
		indices.push_back(j + 1);
		indices.push_back(j < resolution - 1 ? j + 2 : 1);
	}
	for (int i = 1; i < resolution - 2; ++i)
		for (int j = 0; j < resolution; ++j)
		{
			indices.push_back(resolution * (i - 1) + j + 1);
			indices.push_back(resolution * i + j + 1);
			indices.push_back(j < resolution - 1 ? resolution * i + j + 2 : resolution * i + 1);

			indices.push_back(j < resolution - 1 ? resolution * (i - 1) + j + 2 : resolution * (i - 1) + 1);
			indices.push_back(resolution * (i - 1) + j + 1);
			indices.push_back(j < resolution - 1 ? resolution * i + j + 2 : resolution * i + 1);
		}
	for (int j = 0; j < resolution; ++j)
	{
		indices.push_back(cordsLength / 3 - 1);
		indices.push_back(cordsLength / 3 - j - 2);
		indices.push_back((j < resolution - 1 ? cordsLength / 3 - j - 3 : cordsLength / 3 - 2));
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, cordsLength * sizeof(float), cords.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	std::string vertexSource, fragmentSource, bbfragmentSource;
	{
		std::stringstream buffer;
		std::ifstream vFile("vertex.shader");
		std::ifstream fFile("fragment.shader");
		std::ifstream bbfFile("blackBodyFragment.shader");

		buffer << vFile.rdbuf();
		vertexSource = buffer.str();
		buffer.clear();
		buffer.str(std::string());

		buffer << fFile.rdbuf();
		fragmentSource = buffer.str();
		buffer.clear();
		buffer.str(std::string());

		buffer << bbfFile.rdbuf();
		bbfragmentSource = buffer.str();
	}
	shaderProgram = new GLProgram(vertexSource.c_str(), fragmentSource.c_str());
	blackBodyShaderProgram = new GLProgram(vertexSource.c_str(), bbfragmentSource.c_str());
}
template<unsigned resolution>
inline void Sphere<resolution>::cleanStatic() noexcept
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	delete shaderProgram;
	delete blackBodyShaderProgram;
}

template<unsigned resolution>
inline void Sphere<resolution>::Draw(Camera const& camera, glm::vec3 lightSource, float ambient) const noexcept
{
	glBindVertexArray(VAO);

	shaderProgram->use();

	shaderProgram->setUniform("ambient", ambient);
	shaderProgram->setUniform("model", model);
	shaderProgram->setUniform("camera", camera.getWorldToCameraConversion());
	shaderProgram->setUniform("projection", camera.getPerspective());
	shaderProgram->setUniform("lightSource", lightSource);
	shaderProgram->setUniform("modelUnit", modelUniform);
	shaderProgram->setUniform("viewPos", camera.getPosition());
	glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

template<unsigned resolution>
inline void Sphere<resolution>::Draw(Camera const& camera) const noexcept
{
	glBindVertexArray(VAO);
	blackBodyShaderProgram->use();

	blackBodyShaderProgram->setUniform("model", model);
	blackBodyShaderProgram->setUniform("color", {1.f, 1.f, 1.f});
	blackBodyShaderProgram->setUniform("camera", camera.getWorldToCameraConversion());
	blackBodyShaderProgram->setUniform("projection", camera.getPerspective());
	blackBodyShaderProgram->setUniform("viewPos", camera.getPosition());
	glDrawElements(GL_TRIANGLES, indicesLength, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

template<unsigned resolution>
inline void Sphere<resolution>::setPosition(glm::vec3 pos) noexcept
{
	this->pos = pos;
	model = glm::scale(glm::translate(glm::mat4(1.f), pos), glm::vec3(R));
	modelUniform = glm::transpose(glm::inverse(glm::mat3(model)));
}

template<unsigned resolution>
inline void Sphere<resolution>::setRadius(float radius) noexcept
{
	R = radius;
	model = glm::scale(glm::translate(glm::mat4(1.f), pos), glm::vec3(radius));
	modelUniform = glm::transpose(glm::inverse(glm::mat3(model)));
}

template <unsigned resolution>
Sphere<resolution>::Sphere(float radius, glm::vec3 pos) noexcept : R(radius), pos(pos)
{
	if (!counter++)
		initStatic();

	model = glm::scale(glm::translate(glm::mat4(1.f), pos), glm::vec3(radius));
	modelUniform = glm::transpose(glm::inverse(glm::mat3(model)));
}

template<unsigned resolution>
inline Sphere<resolution>::~Sphere() noexcept
{
	if (!--counter)
		cleanStatic();
}
