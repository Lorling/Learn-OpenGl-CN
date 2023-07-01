#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

out vec2 outTexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    outTexcoord = texcoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec2 outTexcoord;

uniform vec4 u_color;
uniform sampler2D u_texture1;
uniform sampler2D u_terture2;

void main()
{
    color = mix(texture(u_texture1, outTexcoord), texture(u_terture2, outTexcoord), 0.2f) * vec4(u_color);
}