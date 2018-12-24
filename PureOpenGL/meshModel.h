#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include "shader.h"
#include "camera.h"
#include "model.h"

namespace PureOpenGL
{
	struct Vertex {
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
		// tangent
		//glm::vec3 Tangent;
		// bitangent
		//glm::vec3 Bitangent;
	};

	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	struct Matrial
	{
		glm::vec3 ka;
		glm::vec3 kd;
		glm::vec3 ks;
		float shininess;
		std::vector<Texture> texture;
	};

	class Mesh
	{
	public:
		std::vector<Vertex>  vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int VAO;
		std::string meshname;
		/*材质属性，如果没有贴图的话*/
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
		Mesh(std::string meshname, std::vector<Vertex> vertices, std::vector<Texture> textures, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

		// render the mesh
		void Draw(Shader *shader, bool mode=false, glm::vec3 dcolor=glm::vec3(0));
		
	private:
		unsigned int VBO, EBO;
		void setupMesh();		
	};

	class meshModel:public Model
	{
	public:
		std::string modelName;
		std::string directory;
		meshModel(std::string path);
		virtual void Draw() override;
	protected:
		std::string Dir;
		
		void loadMtl(const std::string &mtlname);
		std::map<std::string, Matrial>  matrial_loaded;
		std::map<std::string, unsigned int> texture_map;
		std::vector<Mesh> meshes;
		unsigned int TextureFromFile(const char *path, bool gamma = false);
	
	};
}
