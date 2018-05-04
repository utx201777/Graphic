#pragma once 

#include "Callbacks.h"
#include "camera.h"
#include "GlfwInit.h"
#include "objModel.h"
#include "shader.h"
#include <vector>

class Entrance :public ICallbacks
{
private:
	GLfloat lastX = 400, lastY = 300;	
	GLfloat lastFrame = 0.0f;
	bool firstMouse = true;
public :
	Camera camera;
	std::vector<int> key;
	OBJ::Model * o_model;
	Shader * shader;
	Entrance(int width, int height, std::string title)
	{	
		key.resize(256, 0);
		InitGlfw(this,width, height, title);
		o_model = new OBJ::Model();
		o_model->loadModel("model/ch.obj");
		o_model->setUp();
		shader = new Shader("shader/shader.vs", "shader/shader.fs");		
	}

	void Run()
	{
		RunShader();
	}

	virtual void RenderSceneCB() override
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		Do_Movement();	// 处理键盘事件

		glm::mat4 model, view, projection;
		view = camera.GetViewMatrix();
		projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
		shader->Use();
		shader->setMat4("model", model);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		o_model->draw(shader);
	}

	void Do_Movement()
	{
		if (key['W'])
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (key['S'])
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (key['A'])
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (key['D'])
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	virtual void KeyboardCB(GLFWwindow* window, int k, int action = GLFW_RELEASE) override
	{
		if (k == 256 && action == GLFW_PRESS)
		{
			printf("Escape press\n");
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (action == GLFW_PRESS)
			key[k] = true;
		else if (action == GLFW_RELEASE)
			key[k] = false;
	}

	virtual void PassiveMouseCB(GLfloat xpos, GLfloat ypos) override
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
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
};