#version 330 core

in vec3 normal;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D diffuseTexture1;
uniform vec3 incidentDir;

void main()
{
    vec3 originalColor = vec3(texture(diffuseTexture1, texCoord));
    if(all(lessThan(originalColor, vec3(0.1))))
        originalColor = vec3(1.0);
    
    FragColor = max(0, dot(incidentDir, normalize(normal))) * 
                vec4(originalColor, 1.0);
    return;
}