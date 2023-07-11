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

const float offset = 1.0 / 300;

void main()
{
    vec3 col = texture(texture1, TexCoords).rgb;
    color = vec4(col, 1.0f);
    /*����
    vec3 col = texture(texture1, TexCoords).rgb;
    color = vec4(1- col, 1.0f);
    */
    /*
    �Ҷ�
    vec3 col = texture(texture1, TexCoords).rgb;
    float average = 0.216f * col.r + 0.7152f * col.g + 0.0722 * col.g;
    color = vec4(average, average, average, 1.0f);
    */
    //��Ч��
    /*
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // ����
        vec2(0.0f, offset), // ����
        vec2(offset, offset), // ����
        vec2(-offset, 0.0f), // ��
        vec2(0.0f, 0.0f), // ��
        vec2(offset, 0.0f), // ��
        vec2(-offset, -offset), // ����
        vec2(0.0f, -offset), // ����
        vec2(offset, -offset) // ����
    );  
    //�񻯺�
    float kernel[9] = float[](
        -1,-1,-1,
        -1,9,-1,
        -1,-1,-1
    );
    //ģ����
    float kernel[9] = float[](
        1.0/16,2.0/16,1.0/16,
        2.0/16,4.0/16,2.0/16,
        1.0/16,2.0/16,1.0/16
    );
    //��Ե���
    float kernel[9] = float[](
        1.0, 1.0, 1.0,
        1.0, -8.0, 1.0,
        1.0, 1.0, 1.0
    );
    
    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(texture1, TexCoords + offsets[i]));
    vec3 col = vec3(0.0f);
    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    color = vec4(col, 1.0f);
    */
}