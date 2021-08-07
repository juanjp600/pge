#version 330 core

in vec4 position;
in vec4 color;

out vec4 fragColor;

void main() {
    gl_Position = position;
    fragColor = color;
}
