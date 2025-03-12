#version 300 es
precision highp float;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 iResolution;

void main()
{
    FragColor = texture(screenTexture, TexCoords);
    EntityID =  0;
} 