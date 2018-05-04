#include "objModel.h"
#include <fstream>
using namespace std;

void OBJ::Mesh::clear()
{
	points.clear();
	vertices.clear();
	normal.clear();
	textCoords.clear();
}

void OBJ::Mesh::setUp()
{
	printf("vertex count: %d\n", vertices.size());
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nor));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex));

	glBindVertexArray(0);
}

void OBJ::Mesh::draw(Shader * shader)
{
	shader->Use();
	shader->setVec3("matrial.diffuse", mat.Kd);	
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);
}

bool OBJ::Model::loadModel(std::string str)
{
	path = str;
	ifstream infs(path);
	if (!infs.is_open())
	{
		cerr << "open file "<<path<<" error!\n";
		return false;
	}
	string tmp;
	glm::vec3 v;
	int f = -1;
	int points_cnt = 0;
	int normal_cnt = 0;
	int texture_cnt = 0;	
	Mesh mesh;
	while (!infs.eof())
	{
		infs >> tmp;
		if (tmp == "v")
		{
			if (f == -1)
			{
				f = 0;
			}
			else if (f == 1)
			{
				meshes.push_back(mesh);
				points_cnt += mesh.points.size();
				normal_cnt += mesh.normal.size();
				texture_cnt += mesh.textCoords.size();
				mesh.clear();
				f = 0;
			}
			infs >> v[0] >> v[1] >> v[2];
			mesh.points.push_back(v);
		}
		else if (tmp == "vt")
		{
			infs >> v[0] >> v[1] >> v[2];
			mesh.textCoords.push_back(glm::vec2(v[0],v[1]));
		}
		else if (tmp == "vn")
		{
			infs >> v[0] >> v[1] >> v[2];
			mesh.normal.push_back(v);
		}
		else if (tmp == "g")
		{
			f = 1;
			infs >> tmp;
			mesh.meshName = tmp;
		}
		else if (tmp == "usemtl")
		{
			infs >> tmp;
			mesh.matrialName = tmp;
		}
		else if (tmp == "mtllib")
		{
			infs >> tmp;
			matrial_path = tmp;
			printf("mat path: %s\n", tmp.c_str());
		}
		else if (tmp == "f")
		{
			vector<string> vs(3);
			infs >> vs[0] >> vs[1] >> vs[2];
			glm::ivec3 iv;
			if (sscanf(vs[0].c_str(), "%d/%d/%d", &iv[0], &iv[1], &iv[2]) == 3)	// 三个坐标都有
			{
				glm::ivec3 p_idx, t_idx, n_idx;
				for (int i = 0; i < vs.size(); ++i)
				{					
					sscanf(vs[i].c_str(), "%d/%d/%d", &p_idx[i], &t_idx[i], &n_idx[i]);					
				}
				p_idx -= points_cnt;
				t_idx -= texture_cnt;
				n_idx -= normal_cnt;
				
				for (int i = 0; i < 3; ++i)
				{
					Vertex v;
					v.pos = mesh.points[p_idx[i] - 1];
					v.nor = mesh.normal[n_idx[i] - 1];
					v.tex = mesh.textCoords[t_idx[i] - 1];
					mesh.vertices.push_back(v);
				}								
			}
			else
			{
				glm::ivec3 p_idx, n_idx;				
				for (int i = 0; i < vs.size(); ++i)
				{
					sscanf(vs[i].c_str(), "%d//%d", &p_idx[i], &n_idx[i]);					
				}
				p_idx -= points_cnt;
				n_idx -= normal_cnt;
				for (int i = 0; i < 3; ++i)
				{
					Vertex v;
					v.pos = mesh.points[p_idx[i] - 1];
					v.tex = glm::vec2(0);
					v.nor = mesh.normal[n_idx[i] - 1];
					mesh.vertices.push_back(v);
				}
			}
		}
		else
		{
			getline(infs, tmp);
		}
	}
	meshes.push_back(mesh);
	mesh.clear();
	infs.close();	
	// 打开纹理
	infs.open("model/"+matrial_path);
	f = -1;
	Matrial mat;
	while (!infs.eof())
	{
		infs >> tmp;		
		if (tmp == "newmtl")
		{
			if (f == -1)
			{
				f = 1;
			}
			else
			{
				matrials[mat.mat_name] = mat;												
			}
			infs >> tmp;
			mat.mat_name = tmp;
		}
		else if (tmp == "Ka")
		{			
			infs >> v[0] >> v[1] >> v[2];
			mat.Ka = v;
		}
		else if (tmp == "Kd")
		{
			infs >> v[0] >> v[1] >> v[2];			
			mat.Kd = v;
		}		
		else if (tmp == "Ks")
		{
			infs >> v[0] >> v[1] >> v[2];
			mat.Ks = v;
		}		
		else if (tmp == "illum")
		{
			infs >> v[0];
			mat.illum = v[0];
		}
		else
		{
			getline(infs, tmp);
		}
	}
	infs.close();
	matrials[mat.mat_name] = mat;
	for (int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].mat = matrials[meshes[i].matrialName];		
	}
		
	printf("matrial cnt: %d\n", matrials.size());
	printf("mesh cnt: %d\n", meshes.size());
}

void OBJ::Model::setUp()
{
	for (int i = 0; i < meshes.size(); ++i)
		meshes[i].setUp();
}

void OBJ::Model::draw(Shader * shader)
{
	for (int i = 0; i < meshes.size(); ++i)
		meshes[i].draw(shader);
}