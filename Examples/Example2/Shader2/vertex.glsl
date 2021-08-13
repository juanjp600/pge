#version 330 core

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec4 position;
in vec4 color;

out vec4 fragPos;
out vec4 fragColor;

//TODO: add this in cross-compile
float dxZToGlZ(float z, float w) {
    return (((z / w) - 0.5) * 2) * w;
}

void main() {
    //it's worth pointing out that
    //   vec * mat
    //treats mat as row-major, and
    //   mat * vec
    //treats mat as column-major >:(
    fragPos = position * worldMatrix;
    fragPos = fragPos * viewMatrix;
    fragPos = fragPos * projectionMatrix;
    fragPos.z = dxZToGlZ(fragPos.z, fragPos.w);
    gl_Position = fragPos;
    fragColor = color;
}
