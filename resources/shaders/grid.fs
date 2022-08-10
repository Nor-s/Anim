#version 330 core

out vec4 color;
uniform mat4 view;
uniform mat4 projection;
uniform float width =2.0;
uniform int scale =100;


in vec3 near_vec;
in vec3 far_vec;
out vec4 frag_color;
#define linearstep(p0, p1, v) (clamp(((v) - (p0)) / abs((p1) - (p0)), 0.0, 1.0))

//https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders
// computes Z-buffer depth value, and converts the range.
float computeDepth(vec3 pos) {
	// get the clip-space coordinates
	vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);

	// get the depth value in normalized device coordinates
	float clip_space_depth = clip_space_pos.z / clip_space_pos.w;

	// and compute the range based on gl_DepthRange settings (not necessary with default settings, but left for completeness)
    float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;

	float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;

	// and return the result
	return depth;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
	float near = (2.0f*projection[3][2])/(2.0f*projection[2][2]-2.0f);
	float far = ((projection[2][2]-1.0f)*near)/(projection[2][2]+1.0);
    return (2.0 * near * far) / (far + near - z * (far - near))/far;
}

void main() {
	float t = -near_vec.y / (far_vec.y-near_vec.y);
	vec3 R = near_vec + t * (far_vec-near_vec);
	vec4 bg_color = vec4(vec3(0.3),0.5);
	vec4 grid_color = vec4(vec3(0.6), 0.7);
	vec4 x_color = vec4(1.0, 0.3, 0.3, 0.7);
	vec4 z_color = vec4(0.0706, 0.2471, 0.6235, 0.7);


	// https://madebyevan.com/shaders/grid/
	vec2 uv = R.xz/scale;
  	float half_scale = scale / 2.0;
  	vec2 grid = abs(fract(uv - 0.5) - 0.5)/ fwidth(uv);
  	vec2 axis = abs(uv)/ fwidth(uv);
    float ratio = 1.0 -smoothstep(0.0, width,  min(grid.x, grid.y));

	// grid color
	color = bg_color;
	color.a *= ratio; 
	color.rgb = mix(color.rgb, grid_color.rgb, ratio);

	// axes color
    vec2 axes = 1.0 -smoothstep(0.0, width, axis);
	color.rgb =  mix(color.rgb, x_color.rgb, axes.y);
	color.rgb =  mix(color.rgb, z_color.rgb, axes.x);
	color.a = max(color.a, axes.x*x_color.a);
	color.a = max(color.a, axes.y*z_color.a);

	// angle fading
	vec3 camera_pos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));
	vec3 view_dir = (camera_pos - R);
	float dist = length(view_dir);
	float angle_fading =  abs(view_dir.y/dist);

	// set depth
	gl_FragDepth = computeDepth(R);

	// depth fading
	float linear_depth = LinearizeDepth(gl_FragDepth);
    float depth_fading = 1.0-smoothstep(0.0, 1., linear_depth);

	color.a *= angle_fading * depth_fading;
}