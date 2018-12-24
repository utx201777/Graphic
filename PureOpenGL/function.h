#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace PureOpenGL
{
	std::ostream & operator<<(std::ostream & onfs, const glm::vec3 & v);
	std::ostream & operator<<(std::ostream & onfs, const glm::ivec3 & v);
	std::istream & operator>>(std::istream & infs, glm::vec3 & v);
	std::istream & operator>>(std::istream & infs, glm::ivec3 & v);

	// 添加资源路径开头
	std::string treatSourcePath(std::string path);

	// 添加shader路径开头
	std::string treatShaderPath(std::string path);

	// 重载vector<char>的输出运算符，用于对数据进行一次性的读入
	std::ostream & operator<< (std::ostream & onfs, const std::vector<char> & v);	

	// 转换体素化数据的格式
	void transfer();

	void Error(std::string msg);

	// 获得随机数
	int randomNumber(int range);

	// 获得随机颜色
	glm::vec3 randomColor();

	// 生成图片的文件名
	std::string getImgName();

	std::pair<float, float> screen2NDC(int posx, int posy, int width, int height);
}