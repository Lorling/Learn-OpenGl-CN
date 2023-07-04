#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    Normal = aNormal * mat3(transpose(inverse(model)));//法线矩阵
    FragPos = vec3(model * vec4(position, 1.0f));
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 Normal;
in vec3 FragPos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float shininess;
};

struct Light
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * material.ambient;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir , normal);//需要对光线方向取反，将光线的方向转换到和我们的视线一个方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//取幂为求反光度，反光度越高高光点越集中，反之发散
    vec3 specular = light.specular * spec * material.specular;
    
    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
}
