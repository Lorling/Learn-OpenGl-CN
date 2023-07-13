#shader vertex
#version 330 core

layout(location = 0) in vec4 vertex;

out vec2 Texcoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	gl_Position = projection * model * vec4(vertex.xy, 0.0f, 1.0f);
	Texcoords = vertex.zw;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 Texcoords;

uniform sampler2D image;
uniform vec3 spritecolor;

void main()
{
	color = vec4(spritecolor, 1.0f) * texture(image, Texcoords);
}