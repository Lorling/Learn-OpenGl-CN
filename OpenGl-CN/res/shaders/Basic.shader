#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    Normal = aNormal * mat3(transpose(inverse(model)));//法线矩阵
    FragPos = vec3(model * vec4(position, 1.0f));
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 direction;
    float cutoff;//点光源聚光的半径
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    //vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir , normal);//需要对光线方向取反，将光线的方向转换到和我们的视线一个方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//取幂为求反光度，反光度越高高光点越集中，反之发散
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient   *= attenuation;
    diffuse   *= attenuation;
    specular  *= attenuation;
    
    vec3 result;
    float thete = dot(lightDir, normalize(-light.direction));
    if (thete > light.cutoff)
    {
        result = ambient + diffuse + specular;
    }
    else
    {
        result = ambient;
    }
    
    color = vec4(result, 1.0f);
}
