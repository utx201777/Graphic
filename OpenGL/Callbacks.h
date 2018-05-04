#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

enum MOUSE {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT,
	MOUSE_UNDEFINED = 999
};

enum KEY_STATE {
	KEY_STATE_PRESS,
	KEY_STATE_RELEASE
};

class ICallbacks
{
public :	
	GLfloat deltaTime = 0.0f;
	virtual void RenderSceneCB() {};
	virtual void MouseCB(MOUSE Button, KEY_STATE State, int x, int y){};	
	virtual void PassiveMouseCB(GLfloat x, GLfloat y) {};
	virtual void KeyboardCB(GLFWwindow* window, int Key, int KeyState){};
};