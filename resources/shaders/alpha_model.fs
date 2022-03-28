#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 color = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(color.www,1.0);
}