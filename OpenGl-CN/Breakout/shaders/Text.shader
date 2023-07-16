#shader vertex
#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection;

void main ()
{
    gl_Position = projection * vec4(vertex.xy, 0.0f, 1.0f);
    TexCoords = vertex.zw;
}

#shader fragment
#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	color = vec4(textColor, 1.0f) * vec4(1.0f, 1.0f, 1.0f, texture(text, TexCoords).r);
}