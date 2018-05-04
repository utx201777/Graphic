#pragma once

#include "Callbacks.h"
#include <string>

void InitGlfw(ICallbacks * s,int width, int height, std::string title);
void keyfunc(GLFWwindow* window, int k, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void RunShader();
