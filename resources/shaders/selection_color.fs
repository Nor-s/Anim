#version 330 core
out vec4 FragColor;

uniform vec3 selectionColor;

in float boneId;

void main()
{   
    FragColor = vec4(selectionColor.xy, boneId, 1.0);
}