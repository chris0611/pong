#version 330 core

// Input vertex attributes
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

uniform vec2 uResolution;
uniform vec2 uCurvature;

vec2 curveRemapUV(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(uCurvature.x, uCurvature.y);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

void main() {
    vec2 remappedUV = curveRemapUV(fragTexCoord);

    float frequency = uResolution.y / 3.0;
    float globalPos = (remappedUV.y) * frequency;
    float wavePos = cos((fract(globalPos) - 0.5) * 3.14159);

    vec4 texelColor = texture(texture0, remappedUV);

    if (remappedUV.x < 0.0 || remappedUV.y < 0.0 || remappedUV.x > 1.0 || remappedUV.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        finalColor = mix(vec4(0.3, 0.3, 0.3, 0.0), texelColor, wavePos);
    }
}