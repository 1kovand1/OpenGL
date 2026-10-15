#version 330 core
in vec3 color;
in vec3 lightDir;
in vec3 FragPos;
out vec4 FragColor;
uniform float ambient;
uniform mat3 modelUnit;
uniform vec3 viewPos;
void main()
{
	vec3 X = dFdx(FragPos);
	vec3 Y = dFdy(FragPos);
	vec3 Normal = normalize(cross(X, Y));

	vec3 lightColor = vec3(1.f);
	float specularStrength = 0.5;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * lightColor;

	if (dot(Normal, lightDir) < 0)
		specular = vec3(0.f);

	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	FragColor = vec4((ambient + diffuse + specular) * color, 1.f);
}