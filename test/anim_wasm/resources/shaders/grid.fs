#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
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

// https://madebyevan.com/shaders/grid/
float get_grid(vec2 xz, float size) {
	vec2 uv = xz/size;
  	vec2 grid = abs(fract(uv - 0.5) - 0.5)/ fwidth(uv);
    return 1.0 -smoothstep(0.0, width,  min(grid.x, grid.y));
}
vec2 get_axis(vec2 xz) {
	vec2 uv = xz;
  	vec2 axis = abs(uv)/ fwidth(uv);
    return 1.0 -smoothstep(0.0, width, axis);
}
void main() {
	float t = -near_vec.y / (far_vec.y-near_vec.y);
	vec3 R = near_vec + t * (far_vec-near_vec);
	vec4 bg_color = vec4(vec3(0.3),0.5);
	vec4 grid_color = vec4(vec3(0.6), 0.7);
	vec4 grid_color2 = vec4(vec3(1.0), 0.7);

	vec4 x_color = vec4(0.9569, 0.3686, 0.3686, 1.0);
	vec4 z_color = vec4(0.0549, 0.6235, 1.0, 1.0);

	// set depth
	gl_FragDepth = computeDepth(R);

	// depth fading
	float linear_depth = LinearizeDepth(gl_FragDepth);
    float depth_fading = 1.0-smoothstep(0.0, 1., linear_depth);

	// angle fading
	vec3 camera_pos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));
	vec3 view_dir = (camera_pos - R);
	float angle_fading =  abs(view_dir.y/length(view_dir));

	// grid color	
	float grid_ratio = get_grid(R.xz, scale);
	color = bg_color;
	color.rgb = mix(color.rgb, grid_color.rgb, grid_ratio);

	float grid_ratio1 = get_grid(R.xz, scale*10);
	color.rgb = mix(color.rgb, grid_color2.rgb, grid_ratio1 );//* (1.0 - depth_fading));

	// axes color
    vec2 axes = get_axis(R.xz);
	color.rgb =  mix(color.rgb, x_color.rgb, axes.y);
	color.rgb =  mix(color.rgb, z_color.rgb, axes.x);

	// alpha
	angle_fading = min(1.0, max(angle_fading, angle_fading + 0.3));
	color.a *= grid_ratio; 
	color.a = max(color.a, axes.x*x_color.a);
	color.a = max(color.a, axes.y*z_color.a);
	color.a *=  angle_fading*depth_fading;

	color2 = 0;

}