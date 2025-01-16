#version 330 core

in vec2 TexCoord;

uniform sampler2D ourTexture;

out vec4 FragColor;

void main() {
    vec3 textColour = texture(ourTexture, TexCoord).rgb;
    vec3 result = vec3(1.0f) * textColour;

    FragColor = vec4(result, 1.0f); // set all 4 vector values to 1.0
}