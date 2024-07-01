#version 330 core

precision mediump float;

out vec4 FragColor;
in vec2 TexCoord;

uniform int width;
uniform int height;
uniform sampler2D imageTexture1;
uniform int kernelSize; 

// Parameters for the LCG
const int a = 1664525;
const int c = 1013904223;
const int m = 4294967295; // 2^32 - 1

const float repeat = 1.0;

float rand(vec2 seed) {
    return fract(sin(dot(seed.xy, vec2(0.02898, 0.0233))) * 43758.5453);
}

int roundInt(float num)
{
    int integer = int(num);
    if(abs(num - integer) < 0.5)
        return int(num);
    else
        return int(num + 0.5);
}

void main()
{
    vec3 color = vec3(0.0);

    int totalPixels = roundInt(pow(kernelSize * 2.0 + 1.0, 2.0));

    float offsetX = 1.0 / width;
    float offsetY = 1.0 / height;

    for(int i = -kernelSize; i<= kernelSize; i++)
    {
        for(int j = -kernelSize; j<= kernelSize; j++)
        {
            vec4 texture = texture(imageTexture1, TexCoord + vec2(i*offsetX, j*offsetY));
            color += texture.rgb;
        }
    }

    color = color/totalPixels;
    
    FragColor = vec4(color, 1.0);
} 