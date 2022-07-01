#version 330 core

out vec4 color;

in vec3 near;
in vec3 far;

float checkerboard(vec2 R, float scale) {
	return float(mod(
		int(floor(R.x / scale)) +
		int(floor(R.y / scale))
		,2
	));
}

void main() {
	float t = -near.y / (far.y-near.y);

	vec3 R = near + t * (far-near);

	if(t<0 || R.x >= 1000.0 || R.x <= -1000.0 || R.z >= 1000.0 || R.z <= -1000.0 ) 
	{
		discard;
	}

	float c =
		0.2 +
		checkerboard(R.xz, 100) * 0.1 +
		checkerboard(R.xz, 1000) * 0.3;

	color = vec4(vec3(c), 0.7);
}