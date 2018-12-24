#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>
#include "model.h"
#include "shader.h"
#include "camera.h"

namespace PureOpenGL
{
	class RenderingApp
	{
	public:
		static bool f[256];
		static bool firstMouse;
		static GLfloat lastX;
		static GLfloat lastY;
		static std::vector<Model *> model_list;
		static GLFWwindow * window;
		static GLfloat deltaTime;
		static GLfloat lastFrame;
		static void init(int WIDTH, int HEIGHT, std::string name)
		{
			if (glfwInit() == -1)
				Error("glfw init failed");
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			window = glfwCreateWindow(WIDTH, HEIGHT, name.data(), nullptr, nullptr);
			if (window == NULL)
			{
				glfwTerminate();
				Error("Failed to create GLFW window");
			}
			glfwMakeContextCurrent(window);
			glfwInitCallback();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				Error("Failed to initialize GLAD");
			}
			glEnable(GL_DEPTH_TEST);
			glViewport(0, 0, WIDTH, HEIGHT);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		static void glfwInitCallback()
		{
			glfwSetKeyCallback(window, RenderingApp::keyfunc);
			glfwSetCursorPosCallback(window, RenderingApp::mouse_callback);
		}

		// 处理键盘
		static void keyfunc(GLFWwindow* window, int key, int scancode, int action, int mode)
		{
			if (key == GLFW_KEY_ESCAPE)
				return;
			if (action == GLFW_PRESS)
			{
				if (key == GLFW_KEY_Q)
				{
					glfwSetWindowShouldClose(window, GL_TRUE);
					return;
				}
				for (auto &m : model_list)
					m->setKey(key, true);
				f[key] = true;
			}
			else if (action == GLFW_RELEASE)
			{
				f[key] = false;
				for (auto &m : model_list)
					m->setKey(key, false);
			}
			// 设置每个model的键盘事件
			for (auto &m : model_list)
			{
				if (f['W'])
					m->getCamera()->ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
				if (f['S'])
					m->getCamera()->ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
				if (f['A'])
					m->getCamera()->ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
				if (f['D'])
					m->getCamera()->ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
				if (f['O'])
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				if (f['P'])
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}

		// 处理鼠标
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
		{			
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}
			GLfloat xoffset = xpos - lastX;
			GLfloat yoffset = lastY - ypos;
			lastX = xpos;
			lastY = ypos;
			// 处理每个model的鼠标事件
			for (auto &m : model_list)
			{
				m->getCamera()->ProcessMouseMovement(xoffset, yoffset);
				m->setLastPos(glm::ivec2(lastX, lastY));
			}
		}

		static void run()
		{
			while (!glfwWindowShouldClose(window))
			{
				GLfloat currentFrame = glfwGetTime();
				deltaTime = currentFrame - lastFrame;
				lastFrame = currentFrame;
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glfwPollEvents();
				for (auto & m : model_list)
					m->Draw();
				glfwSwapBuffers(window);
			}
			glfwTerminate();
		}
		static void addModel(Model * m)
		{
			model_list.push_back(m);
		}
		static void reset()
		{
			firstMouse = true;
			deltaTime = 0;
			lastFrame = 0;
			model_list.clear();
			window = nullptr;
			lastX = 0;
			lastY = 0;
			memset(f, false, sizeof(f));
		}
	};
	// 静态变量定义
	bool RenderingApp::f[256] = { false };
	bool RenderingApp::firstMouse = true;
	GLfloat RenderingApp::deltaTime = 0;
	GLfloat RenderingApp::lastFrame = 0;
	std::vector<Model *> RenderingApp::model_list;
	GLFWwindow * RenderingApp::window = nullptr;
	GLfloat RenderingApp::lastX = 0;
	GLfloat RenderingApp::lastY = 0;
}