#version 330 core

in vec4 fragPos;
in vec4 fragColor;

out vec4 outColor;

void main() {
    outColor = fragColor;
    //outColor.g = fragPos.z;
}
