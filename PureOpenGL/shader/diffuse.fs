#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 diffuse;
uniform int id;
uniform sampler2D texture_diffuse1;

void main()
{
    vec3 defaultLight[4];
    defaultLight[0] = vec3(1,1,1);
    defaultLight[1] = vec3(1,1,-1);
    defaultLight[2] = vec3(-1,1,-1);
    defaultLight[3] = vec3(1,1,-1);
    vec3 res = vec3(0);
    for(int i=0;i<4;++i)
    {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(defaultLight[i] - FragPos);
        vec3 diff = vec3(1.0);
        if(id==1)
            diff = vec3(texture(texture_diffuse1, TexCoords));
        else
            diff = diffuse;
        float d = max(dot(norm, lightDir), 0.0)*1.5;
        res += d * lightColor * diff; 
    }
    
    FragColor = vec4(res, 1.0);
}