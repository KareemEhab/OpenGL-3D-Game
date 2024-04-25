#version 330 core

struct Material {
	vec3 ambient;
	sampler2D diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 10
struct PointLight {
	vec3 position;

	float k0;
	float k1;
	float k2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

#define MAX_SPOT_LIGHTS 10
struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;			// Field of view of the light
	float outerCutOff;		// Field of view of the light for fading out
	
	float k0;
	float k1;
	float k2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;


out vec4 fragColor;

//in vec3 ourColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos; // Camera position

vec3 calcDirLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 diffMap = vec3(texture(material.diffuse, TexCoord));
	vec3 specMap = material.specular;
	//vec3 specMap = vec3(texture(material.specular, TexCoord)); // That is if it's a texture passed like diffMap
	
	vec3 result;

	// Directional Light
	result = calcDirLight(norm, viewDir, diffMap, specMap);

	// Point Lights
	for(int i = 0; i < noPointLights; i++)
	{
		result += calcPointLight(i, norm, viewDir, diffMap, specMap);
	}

	// Spot Lights
	for(int i = 0; i < noSpotLights; i++)
	{
		result += calcSpotLight(i, norm, viewDir, diffMap, specMap);
	}

	fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient = dirLight.ambient * diffMap;

	// Diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular = dirLight.specular * (spec * specMap);

	return vec3(ambient + diffuse + specular);
}

vec3 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	//vec3 ambient = pointLights[idx].ambient * material.ambient;
	vec3 ambient = pointLights[idx].ambient * diffMap;

	// Diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos); // Get distance from fragment position to the light
	float diff = max(dot(norm, lightDir), 0.0);
	//vec3 diffuse = pointLights[idx].diffuse * (diff * material.diffuse); // That is if a material is passed not a texture
	vec3 diffuse = pointLights[idx].diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular = pointLights[idx].specular * (spec * specMap);
	
	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * dist * dist);

	return vec3(ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient = spotLights[idx].ambient * diffMap;

	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	if(theta > spotLights[idx].outerCutOff) // > because using cosines and not degrees
	{
		// If inside of cutOff, render the light
		
		// Diffuse
		float diff = max(dot(norm, lightDir), 0.0f);
		vec3 diffuse = spotLights[idx].diffuse * (diff * diffMap);

		// Specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
		vec3 specular = spotLights[idx].specular * (spec * specMap);
		
		float intensity = (theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff);
		intensity = clamp(intensity, 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;

		float dist = length(spotLights[idx].position - FragPos);
		float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * dist * dist);

		return vec3(ambient + diffuse + specular) * attenuation;
	}
	else 
	{
		return ambient;
	}
}