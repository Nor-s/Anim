#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int pixelateFactor;
uniform vec2 iResolution;

void main()
{
   	vec2 p = TexCoords.st;
    p.x -= mod(p.x, 1.0 / pixelateFactor);
	p.y -= mod(p.y, 1.0 / pixelateFactor);

    FragColor = texture(screenTexture, p);

    //FragColor = vec4(col, 1.0);
} 