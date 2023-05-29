#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexture;

out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    normal = mat3(transpose(inverse(model))) * aNorm;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    texCoord = aTexture;
    return;
}