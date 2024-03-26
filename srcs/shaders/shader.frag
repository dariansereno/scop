#version 450

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

vec3 lightColor = vec3(1.0, 1.0, 1.0);

layout(set =  1, binding =  1) uniform MaterialProperties {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float refraction;
    int illumination;
    float shininess;
    float transparency;
} material;

void main() {
    outColor = vec4(lightColor * material.ambientColor, 1.0);
}