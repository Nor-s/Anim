#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform vec2 iResolution;
uniform vec3 outlineColor;
uniform float outlineWidth;
uniform float outlineThreshold;

uniform sampler2D screenTexture;

void make_kernel(inout float kernel[9], sampler2D tex, vec2 coord)
{
	float w = outlineWidth /iResolution.x;
	float h = outlineWidth / iResolution.y;
    vec3 grey_scale = vec3(0.299, 0.587, 0.114);

	kernel[0] = length(dot(texture2D(tex, coord + vec2( -w, -h)).xyz, grey_scale));
	kernel[1] = length(dot(texture2D(tex, coord + vec2(0.0, -h)).xyz, grey_scale));
	kernel[2] = length(dot(texture2D(tex, coord + vec2(  w, -h)).xyz, grey_scale));
	kernel[3] = length(dot(texture2D(tex, coord + vec2( -w, 0.0)).xyz, grey_scale));
	kernel[4] = length(dot(texture2D(tex, coord).xyz, grey_scale));
	kernel[5] = length(dot(texture2D(tex, coord + vec2(  w, 0.0)).xyz, grey_scale));
	kernel[6] = length(dot(texture2D(tex, coord + vec2( -w, h)).xyz, grey_scale));
	kernel[7] = length(dot(texture2D(tex, coord + vec2(0.0, h)).xyz, grey_scale));
	kernel[8] = length(dot(texture2D(tex, coord + vec2(  w, h)).xyz, grey_scale));
}
void main() {
    float xFilter[9] = float[9](-2,0,2,
                                -2,0,2,
                                -2,0,2);
    float yFilter[9] = float[9](2,2,2,
                                0,0,0,
                                -2,-2,-2 );
    float kernel[9];
    make_kernel(kernel, screenTexture, TexCoords);  //gl_TexCoord[0].st 
       
    float sx=0.0, sy=0.0;
    for (int i = 0; i < 9; ++i)
    {
        sx += kernel[i] * xFilter[i];
        sy += kernel[i] * yFilter[i];
    }
    float dist = sqrt(sx * sx + sy * sy);
    float th = outlineThreshold;
    float edge = dist >th? 1 : 0.1;
 
    color = vec4(outlineColor, edge);
}