#include "glControl.h"
#include "meshModel.h"

int main()
{
	PureOpenGL::RenderingApp::init(800, 600, "Pure OpenGL");
	PureOpenGL::Model * sModel = new PureOpenGL::meshModel("model/cow.obj");
	sModel->bindShader(new Shader("shader/diffuse.vs", "shader/diffuse.fs"));
	sModel->bindCamera(new Camera());
	PureOpenGL::RenderingApp::addModel(sModel);
	PureOpenGL::RenderingApp::run();
}