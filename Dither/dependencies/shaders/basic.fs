#version 330 core

precision mediump float;

out vec4 FragColor;
in vec2 TexCoord;

uniform int width;
uniform int height;

uniform sampler2D imageTexture1;

const float INTENSITY = 0.1;

const int numOfColors = 8;

const int bayer4[4*4] = int[4*4] (
    0, 8, 2, 10,
    12, 4, 14, 6,
    3, 11, 1, 9,
    15, 7, 13, 5
);

const int DIM = 4;

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
    // vec3 color = vec3(0.0);

    // int totalPixels = roundInt(pow(kernelSize * 2.0 + 1.0, 2.0));

    // float offsetX = 1.0 / width;
    // float offsetY = 1.0 / height;

    // for(int i = -kernelSize; i<= kernelSize; i++)
    // {
    //     for(int j = -kernelSize; j<= kernelSize; j++)
    //     {
    //         vec4 texture = texture(imageTexture1, TexCoord + vec2(i*offsetX, j*offsetY));
    //         color += texture.rgb;
    //     }
    // }

    // color = color/totalPixels;

    int mapX = int(TexCoord.x * width) % DIM;
    int mapY = int(TexCoord.y * height) % DIM;

    int mapValue = bayer4[4*mapY + mapX];
    float noise = INTENSITY * (mapValue/pow(DIM, 2) - 0.5);

    vec3 color = texture(imageTexture1, TexCoord).rgb + vec3(noise);

    // ACEROLA QUANTIZE

    color = floor(color * (numOfColors - 1) + 0.5)/(numOfColors - 1);

    // MY QUANTIZE
    
    FragColor = vec4(color, 1.0);
} 