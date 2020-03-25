#version 330 core

// Define constants.
#define M_PI 3.141593

// The input from the vertex shader.
in vec3 vertNormal;
in vec3 vertPosition;
in vec3 relativeLightPosition;
in vec3 relativeCameraPosition;
in vec2 texCoords;

// Illumination model constants.
uniform vec4 material;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

// Texture sampler.
uniform sampler2D textureSampler;

// Specify the output of the fragment shader.
out vec4 vertColor;

void main()
{
    // Ambient color does not depend on any vectors.
    vec3 texColor = texture(textureSampler, texCoords).xyz;
    vec3 color    = material.x * texColor;

    // Calculate light direction vectors in the Phong illumination model.
    vec3 lightDirection    = normalize(lightPosition - vertPosition);
    vec3 normal            = normalize(vertNormal);

    // Diffuse color.
    float diffuseIntensity = max(dot(normal, lightDirection), 0.0F);
    color += texColor * material.y * diffuseIntensity;

    // Specular color.
    vec3 viewDirection      = normalize(cameraPosition-vertPosition);
    vec3 reflectDirection   = reflect(-lightDirection, normal);
    float specularIntensity = max(dot(reflectDirection, viewDirection), 0.0F);
    color += lightColor * material.z * pow(specularIntensity, material.w);

    vertColor = vec4(color, 1.0F);
}
