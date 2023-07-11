#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0f);
    TexCoords = position;
    gl_Position = gl_Position.xyww;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 TexCoords;

uniform samplerCube texture1;

void main()
{
    color = texture(texture1, TexCoords);
}