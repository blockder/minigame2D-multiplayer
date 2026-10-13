#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec2 texCoord;
out vec3 color;

uniform vec3 movement;
uniform mat3 rotation;
uniform mat4 cameraMatrix;

void main()
{
	texCoord = aTex;
	color = aColor;
	gl_Position =  cameraMatrix * vec4(rotation * aPos + movement, 1.0f);
}