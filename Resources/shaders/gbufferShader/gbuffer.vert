#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform DefaultUniformBlock {
	mat4 viewMat;
	mat4 projMat;
} ubo;

uniform mat4 modelTransform;

out VERT_OUT {
	vec4 FragColor;
	vec3 FragPos;
	vec2 TexCoords;
	vec2 ScreenTexCoords;
	vec3 Normal;
} VertOut;

void main()
{
	vec4 worldPos = modelTransform * vec4(aPos, 1.0);
	VertOut.FragPos = worldPos.xyz; 
	VertOut.TexCoords = aTexCoords;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelTransform)));
	VertOut.Normal = normalMatrix * aNormal;

	vec4 clipPos = ubo.projMat * ubo.viewMat * worldPos;
	gl_Position = clipPos;

	vec2 ndc = clipPos.xy / clipPos.w;
	VertOut.ScreenTexCoords = ndc * 0.5 + 0.5;
}