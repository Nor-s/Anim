#version 330 core

out vec4 color;
uniform mat4 view;
uniform mat4 projection;
uniform float width =1.0;
uniform int scale =100;


in vec3 near_vec;
in vec3 far_vec;
out vec4 frag_color;
#define linearstep(p0, p1, v) (clamp(((v) - (p0)) / abs((p1) - (p0)), 0.0, 1.0))


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
	gl_FragDepth = computeDepth(R);
	float linear_depth = LinearizeDepth(gl_FragDepth);
    float fading = 1.0-smoothstep(0.0, 1., linear_depth);

	color = vec4(vec3(0.1), 0.5);
	vec4 grid_color = vec4(0.6, 0.6, 0.6, 0.5);
	vec4 x_color = vec4(1.0, 0.3, 0.3, 1.0);
	vec4 z_color = vec4(0.3, 0.3, 1.0, 1.0);
	if (R.z > 10000.0 || R.z < -10000.0 || R.x < -10000.0, R.x > 10000.0) {
		discard;
	}

	vec2 uv = R.xz;
  	float half_size = scale / 2.0;
  	vec2 grid = abs(mod(uv + half_size, scale) - half_size)/ fwidth(uv);
  	vec2 axis = abs(uv)/ fwidth(uv);
 	float line_dist = min(grid.x, grid.y);
    float ratio = 1.0 -smoothstep(0.0, width, line_dist);

	color = mix(color, grid_color, ratio);
    ratio = 1.0 -smoothstep(0.0, width*2, axis.x );
	color = mix(color, x_color, ratio);
    ratio = 1.0 -smoothstep(0.0, width*2, axis.y);
	color = mix(color, z_color, ratio);

	vec3 camera_pos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));
	vec3 view_dir = (camera_pos - R);
	float dist = length(view_dir);
	view_dir/=dist;
	float angle = view_dir.y;
	angle = 1.0 - abs(angle);
	angle*=angle;
    float fade = 1.0 - angle * angle;
    fade *= 1.0 - smoothstep(0.0, 10000.0/2.0, dist - 10000.0/2.0);

	// fading *= mix(0.0, 1.0, angle);
	color.a*= fade* fading;
}