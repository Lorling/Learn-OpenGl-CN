#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

#define POINT_LIGHT_NUMBER 4

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(0.04f, 0.28f, 0.26f, 1.0f);
}