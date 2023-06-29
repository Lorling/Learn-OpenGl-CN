#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
out vec3 outcolor;

void main()
{
    gl_Position = vec4(position, 0.0f, 1.0f);
    outcolor = color;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec3 outcolor;

uniform vec4 u_color;

void main()
{
    color = vec4(outcolor,u_color.w);
}