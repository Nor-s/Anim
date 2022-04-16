#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;


void main()
{    
    vec4 color = texture(texture_diffuse1, TexCoords);
    // color *= 1.2;
    // color.r = (trunc(clamp(color.r*255.0, 0.0, 255.0)/15.0)*15.0) / 255.0;
    // color.g = (trunc(clamp(color.g*255.0, 0.0, 255.0)/15.0)*15.0) / 255.0;
    // color.b = (trunc(clamp(color.b*255.0, 0.0, 255.0)/15.0)*15.0) / 255.0;
    if(color.a <= 0.8) {
        discard;
    }
    FragColor = color;
    
}