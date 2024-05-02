#version 330 core

struct Material {
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform Material material;
uniform int noTex;

uniform sampler2D diffuse0;
uniform sampler2D specular0;

struct DirLight {
	vec3 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 10
struct PointLight {
	vec3 position;

	float k0;
	float k1;
	float k2;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
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

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;


out vec4 fragColor;

//in vec3 ourColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPos; // Camera position


vec4 calcDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec4 diffMap;
	vec4 specMap;
	
	if(noTex == 1)
	{
		diffMap = material.diffuse;
		specMap = material.specular;
	}
	else {
		diffMap = texture(diffuse0, TexCoord);
		specMap = texture(specular0, TexCoord);
	}

	vec4 result;

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

	fragColor = result;
}

vec4 calcDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap)
{
	// Ambient
	vec4 ambient = dirLight.ambient * diffMap;

	// Diffuse
	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = dirLight.diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec4 specular = dirLight.specular * (spec * specMap);

	return vec4(ambient + diffuse + specular);
}

vec4 calcPointLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap)
{
	// Ambient
	vec4 ambient = pointLights[idx].ambient * diffMap;

	// Diffuse
	vec3 lightDir = normalize(pointLights[idx].position - FragPos); // Get distance from fragment position to the light
	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = pointLights[idx].diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec4 specular = pointLights[idx].specular * (spec * specMap);
	
	float dist = length(pointLights[idx].position - FragPos);
	float attenuation = 1.0 / (pointLights[idx].k0 + pointLights[idx].k1 * dist + pointLights[idx].k2 * dist * dist);

	return vec4(ambient + diffuse + specular) * attenuation;
}

vec4 calcSpotLight(int idx, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap)
{
	// Ambient
	vec4 ambient = spotLights[idx].ambient * diffMap;

	vec3 lightDir = normalize(spotLights[idx].position - FragPos);
	float theta = dot(lightDir, normalize(-spotLights[idx].direction));

	if(theta > spotLights[idx].outerCutOff) // > because using cosines and not degrees
	{
		// If inside of cutOff, render the light
		
		// Diffuse
		float diff = max(dot(norm, lightDir), 0.0f);
		vec4 diffuse = spotLights[idx].diffuse * (diff * diffMap);

		// Specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
		vec4 specular = spotLights[idx].specular * (spec * specMap);
		
		float intensity = (theta - spotLights[idx].outerCutOff) / (spotLights[idx].cutOff - spotLights[idx].outerCutOff);
		intensity = clamp(intensity, 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;

		float dist = length(spotLights[idx].position - FragPos);
		float attenuation = 1.0 / (spotLights[idx].k0 + spotLights[idx].k1 * dist + spotLights[idx].k2 * dist * dist);

		return vec4(ambient + diffuse + specular) * attenuation;
	}
	else 
	{
		return ambient;
	}
}