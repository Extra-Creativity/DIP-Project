#version 330 core

uniform sampler2D normalTexture;
uniform samplerCube skybox;
uniform vec3 incident;

in vec2 texCoords;
out vec4 FragColor;

vec3 ColorToNorm(vec3 color)
{
    return normalize(color * 4 - 3);
}

void main()
{
    vec3 color = vec3(texture(normalTexture, texCoords));
    if(any(lessThan(color, vec3(0.1, 0.1, 0.1)))){
        FragColor = vec4(1, 1, 1, 0);
    }
    else{
        vec3 approxIncident = normalize(incident);
        vec3 normal = ColorToNorm(color);
        vec3 reflection = reflect(approxIncident, normal);
        FragColor = vec4(texture(skybox, reflection).rgb, 1.0);
    }
    return;
}