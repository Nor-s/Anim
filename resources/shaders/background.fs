#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 colorA;
uniform vec3 colorB;

void main()
{
    FragColor = vec4(mix(colorA, colorB,  TexCoords.y), 1.0);
} 