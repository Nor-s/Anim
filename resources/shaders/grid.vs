#version 330 core
//https://github.com/martin-pr/possumwood/wiki/Skybox
//https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders
// http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform mat4 view;
uniform mat4 projection;


out vec3 near;
out vec3 far;


vec3 UnprojectPoint(float x, float y, float z, mat4 view1, mat4 projection1) {
    mat4 viewInv = inverse(view1);
    mat4 projInv = inverse(projection1);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = vec3(aPos.x, aPos.y, 0);
    near = UnprojectPoint(p.x, p.y, 0.0, view, projection).xyz; // unprojecting on the near plane
    far = UnprojectPoint(p.x, p.y, 1.0, view, projection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}