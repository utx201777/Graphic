#version 330 core
layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNormal;
layout (location=2) in vec2 aTexture;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// 世界坐标
	FragPos = vec3(model *vec4(aPos,1.0));
	// 获得真实的坐标
	gl_Position = projection *view *model *vec4(aPos,1.0);
	// 法线
	Normal = mat3(transpose(inverse(model))) * aNormal;
	// 纹理
	TexCoords = aTexture;
}