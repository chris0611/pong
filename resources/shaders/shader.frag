#version 330 core

// Input vertex attributes
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

uniform vec2 resolution;

const float renderWidth = 800;
const float renderHeight = 500;

void main() {
    float frequency = renderHeight/3.0;
    float globalPos = (fragTexCoord.y) * frequency;
    float wavePos = cos((fract(globalPos) - 0.5)*3.1416);

    vec4 texelColor = texture(  texture0, fragTexCoord);

    finalColor = mix(vec4(0.0, 0.3, 0.0, 0.0), texelColor, wavePos);
}