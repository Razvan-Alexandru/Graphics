#version 330 core

in vec3 FragPos;
in vec3 Normal; 
in vec2 TexCoord;

uniform vec3 lightPos; 
uniform vec3 cameraPos;
uniform float alpha;

uniform sampler2D ourTexture;

out vec4 Colour;

void main() {

    float ka = 0.1;
    float kd = 0.5;
    float ks = 0.3;

    vec3 norm  = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    // Ambient light
    vec3 ambientLight = vec3(1.0f) * ka;

    // Diffuse light
    vec3 diffuseColor = vec3(1.f, 1.f, 1.f);
    float diffuse = max(dot(lightDir, norm), 0.0f);
    vec3 diffuseLight = diffuseColor * diffuse * kd;

    // Specular light
    vec3 reflectedDirection = normalize(2 * norm * max(dot(norm, lightDir), 0.0f) - lightDir);
    vec3 positionToView = normalize(cameraPos - FragPos);
    float specular = pow(max(dot(positionToView, reflectedDirection), 0.0f), 2);
    vec3 specularLight = vec3(1.f, 1.f, 1.f) * specular * ks;

    vec3 result = (ambientLight + diffuseLight + specularLight) * vec3(1.0f, 1.0f, 1.0f);

    vec3 textColour = texture(ourTexture, TexCoord).rgb;
    result = result * textColour;

    Colour = vec4(result, alpha);
}