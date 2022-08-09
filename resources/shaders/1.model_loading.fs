#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos = vec3(0.0, 0.0, 0.0);

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool has_diffuse_texture;
};

struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};

#define NR_DIR_LIGHTS 2
uniform DirLight dir_lights[NR_DIR_LIGHTS];

uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 mat_diffuse)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient * mat_diffuse ;
    vec3 diffuse =  light.diffuse * diff * mat_diffuse;
    vec3 specular = light.specular * spec * material.specular;

    return  ambient + diffuse + specular;
}

void main()
{   
    vec4 texture_color = texture(texture_diffuse1, TexCoords);
    vec3 mat_diffuse = material.diffuse;

    if(material.has_diffuse_texture) {
        if(texture_color.a <= 0.8) {
            discard;
        }
        mat_diffuse = texture_color.rgb;
    }


    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);

    vec3 result = vec3(0.0);

    result += CalcDirLight(dir_lights[0], norm, viewDir, mat_diffuse);
    FragColor = vec4(result, 1.0);

    // for bone id debug
    // FragColor = vec4(tmp/52.0, 0.0, 0.0, 1.0);
}