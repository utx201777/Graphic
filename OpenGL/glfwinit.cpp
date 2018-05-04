#include "GlfwInit.h"
using namespace std;

static ICallbacks* callBacks = nullptr;
static GLFWwindow* window = nullptr;
static int cnt = 0;

void InitGlfw(ICallbacks * s,int width, int height, std::string title)
{
	callBacks = s;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyfunc);
	glfwSetCursorPosCallback(window, mouse_callback);
}

void RunShader()
{
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		callBacks->RenderSceneCB();		
		if (cnt == 20)
		{
			string str = "fps: " + to_string(1 / callBacks->deltaTime);
			glfwSetWindowTitle(window, str.c_str());
			cnt = 0;
		}
		cnt++;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

// 点击和鼠标事件
void keyfunc(GLFWwindow* window, int k, int scancode, int action, int mode)
{
	callBacks->KeyboardCB(window, k, action);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	callBacks->PassiveMouseCB(xpos, ypos);
}
