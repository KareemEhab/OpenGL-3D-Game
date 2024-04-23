#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 fragColor;

//in vec3 ourColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

//uniform sampler2D texture1;
//uniform sampler2D texture2;
//uniform sampler2D texture3;
//uniform float mixVal;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
	//fragColor = vec4(1.0f, 0.2f, 0.6f, 1.0f);
	//fragColor = vec4(ourColor, 1.0);
	//fragColor = texture(texture3, TexCoord);
	//fragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixVal);
	
	// Ambient
	vec3 ambient = light.ambient * material.ambient;

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos); // Get distance from fragment position to the light
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular = light.specular * (spec * material.specular);
	
	fragColor = vec4(vec3(ambient + diffuse + specular), 1.0);
}