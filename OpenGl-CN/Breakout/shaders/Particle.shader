#shader vertex
#version 330 core

layout(location = 0) in vec4 vertex;

out vec2 Texcoords;
out vec4 ParticleColor;

uniform mat4 model;
uniform mat4 projection;
uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 10.0f;
    gl_Position = projection * vec4(vertex.xy * scale + offset, 0.0f, 1.0f);
	Texcoords = vertex.zw;
    ParticleColor = color;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 Texcoords;
in vec4 ParticleColor;

uniform sampler2D image;

void main()
{
    color = vec4(texture(image, Texcoords) * ParticleColor);
}