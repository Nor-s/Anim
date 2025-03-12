#version 330 core
//https://github.com/martin-pr/possumwood/wiki/Skybox
//https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders
// http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/
// https://stackoverflow.com/questions/71520410/how-i-can-apply-antialiasing-to-a-2d-grid-on-a-shader
// https://opentutorials.org/module/3659/22204
// https://thebookofshaders.com/05/
// https://iquilezles.org/articles/filterableprocedurals/
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 near_vec;
out vec3 far_vec;


vec3 UnprojectPoint(float x, float y, float z, mat4 view1, mat4 projection1) {
    mat4 viewInv = inverse(view1);
    mat4 projInv = inverse(projection1);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = vec3(aPos.x, aPos.y, 0);
    near_vec = UnprojectPoint(p.x, p.y, 0.0, view, projection).xyz; // unprojecting on the near plane
    far_vec = UnprojectPoint(p.x, p.y, 1.0, view, projection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}