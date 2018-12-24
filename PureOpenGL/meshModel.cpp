#include "meshModel.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace PureOpenGL
{
	// #define STB_IMAGE_IMPLEMENTATION	
	Mesh::Mesh(std::string meshname, std::vector<Vertex> vertices, std::vector<Texture> textures, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
	{
		this->vertices = vertices;
		this->textures = textures;

		this->meshname = meshname;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}

	// render the mesh
	void Mesh::Draw(Shader *shader, bool mode, glm::vec3 dcolor)
	{
		using std::string;
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < textures.size(); i++)
		{

			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			string number;
			string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			// now set the sampler to the correct texture unit
			shader->setInt((name + number).c_str(), i);
			// glUniform1i(glGetUniformLocation(shader->Program, (name + number).c_str()), i);		// 告诉shader这属于哪一个纹理单元
			// printf("%s\n", (name + number).c_str());
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		// set matrial
		shader->setVec3("ambient", ambient);
		shader->setVec3("diffuse", diffuse);
		shader->setVec3("specular", specular);
		shader->setFloat("shininess", shininess);
		if (textures.size() > 0)
			shader->setInt("id", 1);
		else
			shader->setInt("id", 0);

		// 每一个mesh画一种颜色
		if (mode)
		{
			shader->setInt("id", 0);
			shader->setVec3("diffuse", dcolor);
		}

		// draw mesh
		glBindVertexArray(VAO);
		// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	void Mesh::setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		// printf("vertices count :%d\n", vertices.size());
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		// vertex tangent
		//glEnableVertexAttribArray(3);
		//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
		// vertex bitangent
		//glEnableVertexAttribArray(4);
		//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// 拆分斜杠的三个数字
	std::vector<int> split(std::string s)
	{		
		std::vector<int> res;
		int data = 0;
		for (int i = 0; i < s.size(); ++i)
		{
			if (s[i] == '/')
			{
				res.push_back(data);
				data = 0;	
			}
			else	
				data = data * 10 + s[i] - '0';
		}
		res.push_back(data);
		return res;
	}

	meshModel::meshModel(std::string path)
	{		
		box_max = glm::vec3(-1e10);
		box_min = glm::vec3(1e10);
		using namespace std;
		ifstream infs(path);

		{
			int i = path.size() - 1;
			for (; i >= 0; i--)
				if (path[i] == '/') break;
			Dir = path.substr(0, i + 1);
			printf("dir: %s\n", Dir.c_str());
		}

		if (infs.is_open())
		{
			infs.seekg(0, ios::end);
			long long size = infs.tellg();
			vector<char> vc(size);
			infs.seekg(0, ios::beg);
			infs.read(&vc[0], size);
			stringstream ss;
			ss << vc;

			string tmp;
			string mtlname;
			string meshname = "";
			string matrialname;
			vector<glm::vec3> v;
			vector<glm::vec3> vn;
			vector<glm::vec3> vt;
			vector<Vertex> vertices;			
			glm::vec3 v3;
			while (ss >> tmp)
			{
				if (tmp == "mtllib")
				{
					ss >> mtlname;
					loadMtl(Dir+mtlname);
				}
				else if (tmp == "v")
				{
					ss >> v3[0] >> v3[1] >> v3[2];
					box_min.x = min(box_min.x, v3[0]);
					box_min.y = min(box_min.y, v3[1]);
					box_min.z = min(box_min.z, v3[2]);
					box_max.x = max(box_max.x, v3[0]);
					box_max.y = max(box_max.y, v3[1]);
					box_max.z = max(box_max.z, v3[2]);
					v.push_back(v3);
				}
				else if (tmp == "vn")
				{
					ss >> v3[0] >> v3[1] >> v3[2];
					vn.push_back(v3);
				}
				else if (tmp == "vt")
				{
					ss >> v3[0] >> v3[1] >> v3[2];
					vt.push_back(v3);
				}
				else if (tmp == "g")
				{
					if (meshname.size()>0)
					{
						auto &data = matrial_loaded[matrialname];
						meshes.push_back(Mesh(meshname, vertices, data.texture, data.ka, data.kd, data.ks, data.shininess));
						vertices.clear();						
					}
					ss >> meshname;
				}
				else if (tmp == "usemtl")
				{
					ss >> matrialname;
				}
				else if (tmp == "f")
				{
					string s1, s2, s3;
					ss >> s1 >> s2 >> s3;					
					vector<vector<int>> res = { split(s1), split(s2), split(s3) };
					Vertex vertex;
					for (int i = 0; i < res.size(); ++i)
					{
						vertex.Position = v[res[i][0] - 1];
						vertex.Normal = vn[res[i][2] - 1];
						vertex.TexCoords = glm::vec2(vt[res[i][1] - 1].r, vt[res[i][1] - 1].g);
						vertices.push_back(vertex);
					}
				}
				else				
					getline(ss, tmp);			
			}
			if (meshname.size() > 0)
			{
				auto &data = matrial_loaded[matrialname];
				meshes.push_back(Mesh(meshname, vertices, data.texture, data.ka, data.kd, data.ks, data.shininess));
				vertices.clear();
			}
		}
		else
		{
			Error("Open " + path);
		}
		infs.close();

		std::cout << "模型信息如下：\n";
		std::cout << "包围盒max：" << this->box_max[0] << " " << this->box_max[1] << " " << this->box_max[2] << std::endl;
		std::cout << "包围盒min：" << this->box_min[0] << " " << this->box_min[1] << " " << this->box_min[2] << std::endl;		
		std::cout << "最长边：" << this->getMaxEdge() << std::endl;

	}

	unsigned int meshModel::TextureFromFile(const char *path, bool gamma)
	{
		using std::string;
		
		string filename = string(path);
		filename = Dir + filename;
		printf("texture %s   ", filename.c_str());
		unsigned int textureID;
		glGenTextures(1, &textureID);		
		int width, height, nrComponents;
		unsigned char * data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		//unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{			
			stbi_image_free(data);			
			Error("Texture failed to load at path: " + string(path));
		}
		printf("load success\n", filename.c_str());
		return textureID;
	}

	void meshModel::loadMtl(const std::string &mtlname)
	{
		using namespace std;
		ifstream infs(mtlname);
		if (infs.is_open())
		{
			infs.seekg(0, ios::end);
			long long size = infs.tellg();
			vector<char> vc(size);
			infs.seekg(0, ios::beg);
			infs.read(&vc[0], size);
			stringstream ss;
			ss << vc;

			string tmp;
			string current_name;
			glm::vec3 k;
			float shininess;
			string path;
			while (!ss.eof())
			{
				ss >> tmp;
				if (tmp == "newmtl")
				{
					ss >> current_name;
				}
				else if (tmp == "illum")
				{
					ss >> shininess;
					matrial_loaded[current_name].shininess = shininess;
				}
				else if (tmp == "Ka")
				{
					ss >> k[0] >> k[1] >> k[2];
					matrial_loaded[current_name].ka = k;
				}
				else if (tmp == "Kd")
				{
					ss >> k[0] >> k[1] >> k[2];
					matrial_loaded[current_name].kd = k;
				}
				else if (tmp == "Ks")
				{
					ss >> k[0] >> k[1] >> k[2];
					matrial_loaded[current_name].ks = k;
				}
				else if (tmp == "map_Kd")
				{
					ss >> path;
					Texture texture;
					texture.type = "texture_diffuse";
					texture.path = path;
					if (texture_map.count(path))
						texture.id = texture_map[path];
					else
						texture_map[path] = texture.id = TextureFromFile(path.c_str());
					matrial_loaded[current_name].texture.push_back(texture);
				}
				else
					getline(ss, tmp);
			}
		}
		else
		{
			Error("Open " + mtlname);
		}
		infs.close();
	}

	// draws the model, and thus all its meshes
	void meshModel::Draw()
	{
		shader->Use();
		glm::mat4 modelMatrix, view, projection;
		
		// 平移，尺寸的归一化
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f / getMaxEdge()));
		modelMatrix = glm::translate(modelMatrix, -(box_max + box_min) / 2.0f);

		view = camera->GetViewMatrix();
		projection = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 150.0f);
		shader->setMat4("model", modelMatrix);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		shader->setVec3("lightPos", glm::vec3(1, 2, 2));
		shader->setVec3("lightColor", glm::vec3(0.3, 0.4, 0.5));
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}

}
