#include "camera.hpp"
#include <glm/matrix.hpp>

Camera::Camera(GLFWwindow* window, float aspectRatio) noexcept : window(window), cameraPos{0.f, 0.f, -1.f}, cameraDirection{ 1.f,0.f,0.f,0.f }, aspectRatio(aspectRatio)
{
}

Camera::Camera(GLFWwindow* window, float aspectRatio, glm::vec3 cameraPos, glm::quat cameraDirection) noexcept : window(window), cameraPos(cameraPos), cameraDirection(cameraDirection), aspectRatio(aspectRatio)
{
}

Camera::Camera(Camera&& other) noexcept : window(other.window), cameraPos(other.cameraPos), cameraDirection(other.cameraDirection), aspectRatio(other.aspectRatio)
{
	other.window = nullptr;
}

Camera& Camera::operator=(Camera&& other) noexcept
{
	window = other.window;
	cameraPos = other.cameraPos;
	cameraDirection = other.cameraDirection;
	aspectRatio = other.aspectRatio;
	other.window = nullptr;

	return *this;
}

void Camera::processInput(float dt) noexcept
{
	glm::vec3 forward = getForward();
	glm::vec3 up = getUp();
	glm::vec3 right = getRight();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += forward * (cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= forward * (cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= right * (cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += right * (cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cameraPos += up * (cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cameraPos -= up * (cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraDirection = glm::normalize(glm::angleAxis(glm::radians(cameraSpeed * 3 * dt), forward) * cameraDirection);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraDirection = glm::normalize(glm::angleAxis(glm::radians(-cameraSpeed * 3 * dt), forward) * cameraDirection);

	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		cameraSpeed *= (1 + dt / 1000);
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		cameraSpeed /= (1 + dt / 1000);
}

void Camera::processRotation(float dx, float dy) noexcept
{
	glm::vec3 forward = getForward();
	glm::vec3 up = getUp();
	glm::vec3 right = getRight();

	cameraDirection = glm::normalize(glm::angleAxis(dy * sensitivity, right) * glm::angleAxis(dx * sensitivity, up) * cameraDirection);
}

glm::vec3 Camera::getPosition() const noexcept
{
	return cameraPos;
}

glm::vec3 Camera::getForward() const noexcept
{
	return cameraDirection * glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::vec3 Camera::getUp() const noexcept
{
	return cameraDirection * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getRight() const noexcept
{
	return cameraDirection * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::mat4 Camera::getCameraToWorldRotation() const noexcept
{
	return glm::mat4(cameraDirection);
}

glm::mat4 Camera::getCameraToWorldConversion() const noexcept
{
	return glm::translate(getCameraToWorldRotation(), cameraPos);
}

glm::mat4 Camera::getWorldToCameraRotation() const noexcept
{
	return glm::mat4(glm::conjugate(cameraDirection));
}

glm::mat4 Camera::getWorldToCameraConversion() const noexcept
{
	return glm::translate(getWorldToCameraRotation(), -cameraPos);
}

glm::mat4 Camera::getPerspective() const noexcept
{
	return glm::perspective(glm::radians(45.f), aspectRatio, .1f, 100.f);
}

void Camera::setAspectRatio(float aspectRatio) noexcept
{
	this->aspectRatio = aspectRatio;
}
