#version 330 core
layout (location=0) in vec3 vertexPos;
layout (location=1) in vec3 vertexColor;
out vec3 color;
out vec3 lightDir;
out vec3 FragPos;
uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;
uniform vec3 lightSource;

void main() {
	gl_Position = projection * camera * model * vec4(vertexPos, 1.0);
	FragPos = vec3(model * vec4(vertexPos, 1.0));
	color = vec3(1.f, 0.f, 0.f);//vertexColor;
	lightDir = normalize(lightSource - FragPos.xyz);
}