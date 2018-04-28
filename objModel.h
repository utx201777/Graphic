#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <map>
#include "shader.h"

namespace OBJ
{
	struct Matrial
	{
		glm::vec3 Ka;
		glm::vec3 Kd;
		glm::vec3 Ks;
		int illum;
		std::string mat_name;
	};

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 nor;
		glm::vec2 tex;
	};

	class Mesh
	{			
	public:
		std::string meshName;
		std::string matrialName;
		std::vector<glm::vec3> points;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec2> textCoords;
		// 存储网格信息，将顶点重新组织，每个顶点含有位置，纹理，法线		
		std::vector<Vertex> vertices;
		Matrial mat;
		void clear();
		void setUp();
		void draw(Shader *);
	private:
		GLuint VAO;
		GLuint VBO;		
	};	

	class Model
	{
	public:		
		Model(){};
		~Model(){};
		std::string path;
		std::string matrial_path;
		bool loadModel(std::string path);
		void setUp();
		void draw(Shader *);
	private:
		std::vector<Mesh> meshes;
		std::map<std::string, Matrial> matrials;
	};
}

