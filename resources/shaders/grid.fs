#version 330 core

out vec4 color;
uniform mat4 view;
uniform mat4 projection;
uniform float width =3.0;


in vec3 near_vec;
in vec3 far_vec;

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

	if(t<0 )
	{
		discard;
	}
	if (!(mod(int(abs(R.x)),100)< width || mod(int(abs(R.z)),100) < width)){
		discard;
	}
	
	float c = 0.5;

	color = vec4(vec3(c), 0.6);
	if(abs(int(R.x)) < width) {
		color = vec4(0.2, 0.2, 1.0, 1.0);
	}
	if(abs(int(R.z)) < width) {
		color = vec4(1.0, 0.2, 0.2, 1.0);
	}

	gl_FragDepth = computeDepth(R);
    float fading = max(0, (0.5 - LinearizeDepth(gl_FragDepth)*2));

	color.a *= fading;
}