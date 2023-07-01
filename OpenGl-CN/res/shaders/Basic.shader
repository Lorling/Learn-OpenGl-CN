#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texcoord;

out vec3 outColor;
out vec2 outTexcoord;

uniform mat4 transfrom;

void main()
{
    gl_Position = transfrom * vec4(position, 0.0f, 1.0f);
    outColor = color;
    outTexcoord = texcoord;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec3 outColor;
in vec2 outTexcoord;

uniform vec4 u_color;
uniform sampler2D u_texture1;
uniform sampler2D u_terture2;

void main()
{
    color = mix(texture(u_texture1, outTexcoord), texture(u_terture2, outTexcoord), 0.2f) * vec4(u_color);
}