#pragma once
#include "camera.h"
#include "shader.h"
#include "function.h"

namespace PureOpenGL
{
	class Model
	{
	public:	// 虚函数
		virtual void Draw(){};							// 绘制接口，重写这个函数即可
		virtual void bindShader(Shader * shader);
		virtual void bindCamera(Camera * camera);
		virtual void setKey(int k, bool f);
		virtual glm::vec3 getPixelPos(int x, int y);	// 根据屏幕位置获得三维位置

	public:	// 返回接口的函数
		
		glm::vec3 getBoxMin();
		glm::vec3 getBoxMax();
		Camera * getCamera();
		Shader * getShader();
		float getMaxEdge();
		void setBoxMax(glm::vec3 bm);
		void setBoxMin(glm::vec3 bm);
		glm::vec3 getBoxCenter();		
	public:
		Model();
		~Model();
		void saveImage();		// 保存图片
		void setLastPos(glm::ivec2 p);			// 设置点击的位置
	protected:
		Camera * camera;
		Shader * shader;		
		bool key[256];			// 键盘事件
		glm::vec2 lastPos;		// 鼠标位置
	protected:
		// 模型本身信息
		glm::vec3 box_min = glm::vec3(1e10);
		glm::vec3 box_max = glm::vec3(-1e10);
		float max_edge;
	};
}