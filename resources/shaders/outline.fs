#version 330 core
in vec2 TexCoords;
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

uniform vec2 iResolution;
uniform vec3 outlineColor;
uniform float outlineWidth;
uniform float outlineThreshold;

uniform sampler2D screenTexture;

// ref https://cutecatgame.tistory.com/14
void make_kernel(inout float kernel[9], sampler2D tex, vec2 coord)
{
	float w = outlineWidth /iResolution.x;
	float h = outlineWidth / iResolution.y;

	kernel[0] = length(texture(tex, coord + vec2( -w, -h)).xyz);
	kernel[1] = length(texture(tex, coord + vec2(0.0, -h)).xyz);
	kernel[2] = length(texture(tex, coord + vec2(  w, -h)).xyz);
	kernel[3] = length(texture(tex, coord + vec2( -w, 0.0)).xyz);
	kernel[4] = length(texture(tex, coord).xyz);
	kernel[5] = length(texture(tex, coord + vec2(  w, 0.0)).xyz);
	kernel[6] = length(texture(tex, coord + vec2( -w, h)).xyz);
	kernel[7] = length(texture(tex, coord + vec2(0.0, h)).xyz);
	kernel[8] = length(texture(tex, coord + vec2(  w, h)).xyz);
}
void main() {
    // sobel
    float xFilter[9] = float[9](-1,0,1,
                                -2,0,2,
                                -1,0,1);
    float yFilter[9] = float[9](1,2,1,
                                0,0,0,
                                -1,-2,-1 );
    float kernel[9];
    make_kernel(kernel, screenTexture, TexCoords);  //gl_TexCoord[0].st 
       
    float sx=0.0, sy=0.0;
    for (int i = 0; i < 9; ++i)
    {
        sx += kernel[i] * xFilter[i];
        sy += kernel[i] * yFilter[i];
    }
    float dist = sqrt(sx * sx + sy * sy);
    float edge = dist > outlineThreshold? 1 : 0.1;
 
    color = vec4(outlineColor, edge);
    color2  = 0;
}
