#include "model.h"
#include "stb_image_write.h"
#include <algorithm>
#include <chrono>

namespace PureOpenGL
{	
	extern int WIDTH;
	extern int HEIGHT;
	
	void Model::bindShader(Shader * shader)
	{
		this->shader = shader;
	}
	void Model::bindCamera(Camera * camera)
	{
		this->camera = camera;
	}
	Camera * Model::getCamera()
	{
		return camera;
	}
	Shader * Model::getShader()
	{
		return shader;
	}
	
	glm::vec3 Model::getBoxMax()
	{
		return box_max;
	}

	glm::vec3 Model::getBoxMin()
	{
		return box_min;
	}

	float Model::getMaxEdge()
	{
		using std::max;
		return max(box_max.x - box_min.x, max(box_max.y - box_min.y, box_max.z - box_min.z));
	}
	void Model::setBoxMax(glm::vec3 bm)
	{
		box_max = bm;
	}

	void Model::setBoxMin(glm::vec3 bm)
	{
		box_min = bm;
	}

	glm::vec3 Model::getBoxCenter()
	{
		return (box_max + box_min) / 2.0f;
	}

	Model::Model()
	{
		memset(key, 0, sizeof(key));
	}

	Model::~Model()
	{

	}

	void Model::setKey(int k, bool f)
	{
		key[k] = f;
		if (key['K'])
			saveImage();
	}

	void Model::setLastPos(glm::ivec2 p)
	{
		lastPos = p;
	}

	void Model::saveImage()
	{
		using namespace std;
		GLubyte *pixel = new GLubyte[WIDTH * HEIGHT * 3];;		
		glReadBuffer(GL_FRONT);
		glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (void *)pixel);
		for (int i = 0; i < HEIGHT / 2; ++i)
			for (int j = 0; j < WIDTH; ++j)
			{
				swap(pixel[3 * (i*WIDTH + j)], pixel[3 * ((HEIGHT-1-i)*WIDTH + j)]);
				swap(pixel[3 * (i*WIDTH + j)+1], pixel[3 * ((HEIGHT-1 - i)*WIDTH + j)+1]);
				swap(pixel[3 * (i*WIDTH + j)+2], pixel[3 * ((HEIGHT-1 - i)*WIDTH + j)+2]);
			}
		std::string path = getImgName().c_str();
		stbi_write_png(path.c_str(), WIDTH, HEIGHT, 3, pixel, 0);
		delete []pixel;
		puts(("save "+path+" success").c_str());
	}

	// 根据屏幕位置获得三维位置
	glm::vec3 Model::getPixelPos(int x, int y)
	{
		float depth;
		glReadPixels(x, HEIGHT - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		depth = (depth - 0.5) * 2;
		printf("current depth:%f\n", depth);
		auto it = screen2NDC(x, HEIGHT - y, WIDTH, HEIGHT);
		glm::vec4 pos = glm::vec4(it.first, it.second, depth, 1.0f);

		glm::mat4 modelMatrix, view, projection;
		view = getCamera()->GetViewMatrix();
		projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.01f, 10.0f);
		glm::mat4 matrix = projection*view*modelMatrix;

		pos = glm::inverse(matrix)* pos;
		pos /= pos.w;
		return glm::vec3(pos.x, pos.y, pos.z);
	}
}