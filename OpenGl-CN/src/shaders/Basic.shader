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

layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    color = texture(texture1, TexCoords);
}