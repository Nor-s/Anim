// https://gist.github.com/xoppa/33589b7d5805205f8f08
// https://www.youtube.com/watch?v=d9QkhRefiEA
// https://www.youtube.com/watch?v=y5mCqjaacvs&list=PLiKs97d-BatF4rKLGxxco0Xne9fgs6aUM&index=9
// https://github.com/kiwipxl/GLSL-shaders/blob/master/outline.glsl
// https://gist.github.com/Hebali/6ebfc66106459aacee6a9fac029d0115
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 iResolution;



uniform float outline_thickness = 1.;
uniform vec3 outline_color = vec3(0.8, 0.8, 0.8);
uniform float outline_threshold = .1;


void main()
{
    FragColor = texture(screenTexture, TexCoords);
    
    if (FragColor.a <= outline_threshold) {
        ivec2 size = textureSize(screenTexture, 0);

        float uv_x = TexCoords.x * size.x;
        float uv_y = TexCoords.y * size.y;

        float sum = 0.0;
        float h_sum = 0.0;
        //3x3
        for (int n = 0; n < 3; ++n) {
            uv_y = (TexCoords.y * size.y) + (outline_thickness * float(n - 1.));
            h_sum = 0.0;
            //texelfetch : 정규화 되지 않은 좌표를 사용. texel(최소 그래픽 구성요소)에 직접 접근
            h_sum += texelFetch(screenTexture, ivec2(uv_x - outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(screenTexture, ivec2(uv_x, uv_y), 0).a;
            h_sum += texelFetch(screenTexture, ivec2(uv_x + outline_thickness, uv_y), 0).a;
            sum += h_sum / 3.0;
        }

        if (sum / 3.0 >= 0.0001) {
            FragColor = vec4(outline_color, 1);
        }
    }
} 