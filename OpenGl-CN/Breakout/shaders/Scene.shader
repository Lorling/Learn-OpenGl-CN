#shader vertex
#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;
uniform float time;

void main ()
{
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
    vec2 texture = vertex.zw;
    if (chaos)
    {
        float strength = 0.03f;
        //按时间往右上角移动
        vec2 pos = vec2(texture.x + time * strength, texture.y + time * strength);
        //按一个点转动
        //vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength);
        TexCoords = pos;
    }
    else if (confuse)
    {
        //反转纹理坐标
        TexCoords = vec2(1.0 - texture);
    }
    else
    {
        TexCoords = texture;
    }
    if (shake)
    {
        float strength = 0.01f;
        gl_Position.x += cos(time * 10) * strength;
        gl_Position.y += cos(time * 15) * strength;
    }
}

#shader fragment
#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

uniform sampler2D scene;
uniform vec2 offsets[9];
uniform int edge_kernel[9];//锐化
uniform float blur_kernel[9]; //模糊

void main()
{
    color = vec4(0.0f);
    vec3 sample[9];
    if (chaos || shake)
        for (int i = 0; i < 9; i++)
            sample[i] = vec3(texture(scene, TexCoords + offsets[i]));
    if (chaos)
    {
        for (int i = 0; i < 9; i++)
            color += vec4(sample[i] * edge_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else if (confuse)
    {
        color = vec4(1.0f - texture(scene, TexCoords).rgb, 1.0f);
    }
    else if (shake)
    {
        for (int i = 0; i < 9; i++)
            color += vec4(sample[i] * blur_kernel[i], 0.0f);
        color.a = 1.0f;
    }
    else
    {
        color = texture(scene, TexCoords);
    }
}