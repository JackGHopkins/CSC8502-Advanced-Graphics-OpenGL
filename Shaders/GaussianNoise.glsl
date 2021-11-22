#version 430 core

layout (local_size_x = 16, local_size_y = 16) in;
layout (binding = 0, rgba32f) writeonly uniform image2D _h0K;
layout (binding = 1, rgba32f) writeonly uniform image2D _h0SubtractK;

uniform sampler2D noiseR0;
uniform sampler2D noiseI0;
uniform sampler2D noiseR1;
uniform sampler2D noiseI1;

uniform int fftRes;
uniform int oceanHorizontal;
uniform float amp;
uniform vec2 windDir;
uniform float windSpeed;

const float g = 9.81;
const float pi = 3.1415926535;

vec4 gaussianRND(){
    vec2 texCoord = vec2(gl_GlobalInvocationID.xy)/float(fftRes);

    float noise0 = clamp(texture(noiseR0, texCoord).r + 0.00001, 0, 1);
    float noise1 = clamp(texture(noiseI0, texCoord).r + 0.00001, 0, 1);
    float noise2 = clamp(texture(noiseR1, texCoord).r + 0.00001, 0, 1);
    float noise3 = clamp(texture(noiseI1, texCoord).r + 0.00001, 0, 1);

    float u0 = noise0 * 2.0 * pi;
    float v0 = sqrt(-2.0 * log(noise1));
    float u1 = noise2 * pi * 2.0;
    float v1 = sqrt(-2.0 * log(noise3));

    vec4 gRND = vec4(v0 * cos(u0), v0 * sin(u0), v1 * cos(u1), v1 * sin(u1));

    return gRND;
}

void main(void) {
    vec2 x = vec2(gl_GlobalInvocationID.xy);
    vec2 k = vec2(2.0 * pi * x.x / oceanHorizontal, 2.0 * pi * x.y / oceanHorizontal);

    float L = pow(windSpeed, 2)/g;
    float magnitude = length(k);

    if (magnitude < 0.0001){
        magnitude = 0.0001;
    }

    float magSq = pow(magnitude, 2);

    //sqrt(Ph(k))/sqrt(2)
    float h0K = clamp(sqrt((amp/pow(magSq,2)) * pow(dot(normalize(k), normalize(windDir)), 4.0) * exp(-(1.0/(magSq * pow(L,2)))) * exp(-magSq*pow(oceanHorizontal/2000.0, 2)))
            / sqrt(2.0), 0, 1000000);
    
    //sqrt(Ph(-k))/sqrt(2)
    float h0SubtractK = clamp(sqrt((amp/pow(magSq,2)) * pow(dot(normalize(-k), normalize(windDir)), 4.0) * exp(-(1.0/(magSq * pow(L,2)))) * exp(-magSq*pow(oceanHorizontal/2000.0, 2)))
            / sqrt(2.0), 0, 1000000);

    vec4 guassRND = gaussianRND();
    imageStore(_h0K, ivec2(gl_GlobalInvocationID.xy), vec4(guassRND.xy * h0K, 0, 1));
    imageStore(_h0SubtractK, ivec2(gl_GlobalInvocationID.xy), vec4(guassRND.xy * h0SubtractK, 0, 1));
}