#version 330 core

out vec4 Colour;

uniform vec3 colour;

void main() {
    Colour = vec4(colour, 0.3f);
}