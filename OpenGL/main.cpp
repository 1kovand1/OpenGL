#define GLM_ENABLE_EXPERIMENTAL

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "utils.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glProgram.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include <eigen3/Eigen/Dense>

int width{ 1920 }, height{ 1080 };
std::unique_ptr<Camera> camera;

glm::vec3 lightSource{ 0, 0, 0 };

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	::width = width;
	::height = height;
	glViewport(0, 0, width, height);
	camera->setAspectRatio((float)width / height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	const float sensitivity = 0.01f;
	static float lastX = xpos, lastY = ypos;

	float xoffset = lastX - xpos;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera->processRotation(xoffset, yoffset);
}

int WinMain() 
{
	glfwInit();
	//SCOPE_EXIT(glfwTerminate);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	camera = std::make_unique<Camera>(window, (float)width / height, glm::vec3{ 0,0,25 }, glm::quatLookAt(glm::vec3 { 0, 0, -1  }, {1.,-1,0}));

	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	std::string vertexSource, fragmentSource;
	{
		std::stringstream buffer;
		std::ifstream vFile("vertex.shader");
		std::ifstream fFile("blackBodyFragment.shader");

		buffer << vFile.rdbuf();
		vertexSource = buffer.str();
		buffer.clear();
		buffer.str(std::string());

		buffer << fFile.rdbuf();
		fragmentSource = buffer.str();
	}
	GLProgram blackBodyShaderProgram(vertexSource.c_str(), fragmentSource.c_str());

	Sphere<1000> s1(0.2);
	Sphere<100> s2(0.1);
	Sphere<1000> s3(1);

	glEnable(GL_DEPTH_TEST);

	float lastTimeValue = glfwGetTime();
	Eigen::Matrix<double, 6, 3> Y{};
	Y.row(0) = Eigen::Vector<double, 3>{0,6,0};
	Y.row(1) = Eigen::Vector<double, 3>{ sqrt(30 / 6), 0, 0};

	Y.row(2) = Eigen::Vector<double, 3>{ 0,7,0 };
	Y.row(3) = Eigen::Vector<double, 3>{sqrt(30/6), 0, sqrt(30 * 0.01)};

	Y.row(4) = Eigen::Vector<double, 3>{ 0,0,0 };
	Y.row(5) = Eigen::Vector<double, 3>{ 0,0,0 };


	while (!glfwWindowShouldClose(window))
	{
		static float ambient = 0.5f;
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		float timeValue = glfwGetTime();
		float dt = timeValue - lastTimeValue;
		lastTimeValue = timeValue;
		camera->processInput(dt);
		if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
			ambient += dt * 0.3f;
		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
			ambient -= dt * 0.3f;
		ambient = std::min(1.f, std::max(0.f, ambient));

		glClearColor(ambient, ambient, ambient, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Y = RK4(Y, (double)dt, Eigen::Vector<double, 3>{0.01, 0.00001, 1});
		auto pos = glm::vec3{ Y.row(0)[0], Y.row(0)[1], Y.row(0)[2] };
		auto pos2 = glm::vec3{ Y.row(2)[0], Y.row(2)[1], Y.row(2)[2] };
		auto pos3 = glm::vec3{ Y.row(4)[0], Y.row(4)[1], Y.row(4)[2] };
		printf("(%f,%f,%f), |v|=%f\n", pos[0], pos[1], pos[2], Y.row(1).norm());
		s1.setPosition(pos);
		s2.setPosition(pos2);
		s3.setPosition(pos3);
		s1.Draw(*camera, pos3, ambient);
		s2.Draw(*camera, pos3, ambient);
		s3.Draw(*camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}

int main()
{
	return WinMain();
}
