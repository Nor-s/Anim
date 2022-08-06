#version 330 core

out vec4 color;

in vec3 near;
in vec3 far;

void main() {
	float t = -near.y / (far.y-near.y);

	vec3 R = near + t * (far-near);

	if(t<0 || R.x > 10000.0 || R.x < -10000.0 || R.z > 10000.0 || R.z < -10000.0 ||( mod(int(R.x),100) != 0 &&mod(int(R.z),100) != 0)  ) 
	{
		discard;
	}

	float c = 0.5;

	color = vec4(vec3(c), 0.5);
	if(int(R.x) ==0) {
		color = vec4(0.0, 0.0, 1.0, 0.5);
	}
	if(int(R.z) ==0) {
		color = vec4(1.0, 0.0, 0.0, 0.5);
	}

}