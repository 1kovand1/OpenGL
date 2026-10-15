#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>

class Camera {
	GLFWwindow* window ;
	glm::vec3 cameraPos;
	glm::quat cameraDirection;
	float sensitivity = 0.01f;
	float cameraSpeed = 0.5f;
	float fov = 45, far=100, near=.1, aspectRatio;

public:
	Camera(GLFWwindow* window, float aspectRatio) noexcept;
	Camera(GLFWwindow* window, float aspectRatio, glm::vec3 cameraPos, glm::quat cameraDirection) noexcept;
	Camera(Camera const&) = delete;
	Camera(Camera&&) noexcept;

	Camera operator=(Camera const&) = delete;
	Camera& operator=(Camera&&) noexcept;

	void processInput(float dt) noexcept;
	void processRotation(float dx, float dy) noexcept;

	glm::vec3 getPosition() const noexcept;
	glm::vec3 getForward() const noexcept;
	glm::vec3 getUp() const noexcept;
	glm::vec3 getRight() const noexcept;
	glm::mat4 getCameraToWorldRotation() const noexcept;
	glm::mat4 getCameraToWorldConversion() const noexcept;
	glm::mat4 getWorldToCameraRotation() const noexcept;
	glm::mat4 getWorldToCameraConversion() const noexcept;
	glm::mat4 getPerspective() const noexcept;

	void setAspectRatio(float aspectRatio) noexcept;
};