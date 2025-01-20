#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float k_c; // Constant attenuation
    float k_l; // Linear attenuation
    float k_q; // Quadratic attenuation
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform PointLight pointLights[3]; // Adjust for three lights
uniform DirectionalLight directionalLight;

// Function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Cumulative light contributions
    vec3 result = vec3(0.0);
    for (int i = 0; i < 3; i++) { // three point lights
        result += CalcPointLight(pointLights[i], norm, viewDir);
    }
    result += CalcDirectionalLight(directionalLight, norm, viewDir);

    FragColor = vec4(result, 1.0);
}

// Point light calculation
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * distance + light.k_q * (distance * distance));

    vec3 ambient = light.ambient * material.ambient;   //Ambient Scaling intensity 
    vec3 diffuse = light.diffuse * diff * material.diffuse; 
    vec3 specular = light.specular * spec * material.specular; 

    return attenuation * (ambient + diffuse + specular); 
}

// Directional light calculation
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;

    return ambient + diffuse + specular;
}
