#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(position, 1.0f));
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    vec4 t = texture(texture_diffuse1, vec2(Normal));
    vec3 view = normalize(Position - cameraPos);
    vec3 reflect = reflect(view, normalize(Normal));
    color = vec4(texture(texture_specular1, vec2(reflect)).rgb, 1.0f);
}