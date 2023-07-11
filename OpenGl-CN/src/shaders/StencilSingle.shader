#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(position, 0.0f, 1.0f);
    TexCoords = aTexCoords;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec3 col = texture(texture1, TexCoords).rgb;
    color = vec4(col, 1.0f);
}