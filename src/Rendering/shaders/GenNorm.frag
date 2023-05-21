#version 330 core

in vec3 normal;

out vec4 FragColor;

// Mapped from sphere of [-1, 1] to cube in [0, 1]^3
vec3 NormToColor(vec3 norm)
{
    return (normalize(norm) + 1) / 2;
}

void main()
{
    FragColor = vec4(NormToColor(normal), 1.0);
    return;
}