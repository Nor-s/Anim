#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int pixelateFactor;
uniform vec2 iResolution;

void main()
{
   	vec2 p = TexCoords.st;
    vec4 color = texture(screenTexture, p);
    p.x -= mod(p.x, 1.0 / pixelateFactor);
	 p.y -= mod(p.y, 1.0 / pixelateFactor);
//    color = texture(screenTexture, p);
//     if(color.a  < 0.02) {
//         discard;
//     }  
   color = texture(screenTexture, p);
    FragColor  = color;
    //FragColor = vec4(col, 1.0);
} 