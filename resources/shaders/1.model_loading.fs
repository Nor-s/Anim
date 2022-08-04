#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos = vec3(500.0,  500.0, 500.0);
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool has_diffuse_texture;
};

uniform Material material;

void main()
{   
    vec4 texture_color = texture(texture_diffuse1, TexCoords);
    vec3 objectColor = material.diffuse;

    if(material.has_diffuse_texture) {
        if(texture_color.a <= 0.8) {
            discard;
        }
        objectColor = texture_color.rgb;
    }

    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}