#version 430 core

out vec4 color;

in vec3 near;
in vec3 far;

uniform mat4 projection;
uniform mat4 view;

float checkerboard(vec2 R, float scale) {
	return float((
		int(floor(R.x / scale)) +
		int(floor(R.y / scale))
	) % 2);
}

float computeDepth(vec3 pos) {
	vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);
	float clip_space_depth = clip_space_pos.z / clip_space_pos.w;

	float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;

	float depth = (((far-near) * clip_space_depth) + near + far) / 2.0;

	return depth;
}
float computeLinearDepth(vec3 pos) {
	float far = gl_DepthRange.far;
	float near = gl_DepthRange.near;
    vec4 clip_space_pos = projection * view * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}
void main() {
	float t = -near.y / (far.y-near.y);

	vec3 R = near + t * (far-near);

    gl_FragDepth = computeDepth(R);
    float linearDepth = computeLinearDepth(R);
    float fading = max(0, (0.7 - linearDepth));
	
	float c =
		checkerboard(R.xz, 10) * 0.3 +
		checkerboard(R.xz, 100) * 0.2 +
		checkerboard(R.xz, 1000) * 0.1 +
		0.1;
	//c = c * float(t > 0);

	color = vec4(vec3(c), 0.7);
    color.a *= fading;

    if (t<0) {
        discard;
    }
}