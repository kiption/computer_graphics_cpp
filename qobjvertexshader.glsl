#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 ModelTransform;
uniform mat4 ViewTransform;
uniform mat4 ProjectionTransform;
uniform mat4 NormalTransform;

void main()
{
	FragPos = vec3(ModelTransform * vec4(vPos, 1));
	gl_Position = ProjectionTransform * ViewTransform * ModelTransform * vec4(vPos, 1.0);
	Normal = vec3(NormalTransform * vec4(vNormal, 1));
	TexCoord = vTexCoord;
}
