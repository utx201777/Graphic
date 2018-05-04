#version 330 core
in vec2 atexture;
struct Matrial
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shinness;

    sampler2D texture_diffuse;
};
uniform Matrial matrial;
out vec4 FragColor;

void main()
{
    FragColor = vec4(matrial.diffuse,1);
}