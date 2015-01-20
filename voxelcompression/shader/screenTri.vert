#version 450 core

#include "globalubos.glsl"

// input
layout(location = 0) in vec2 inPosition;

// output
layout(location = 0) out vec2 Texcoord;
layout(location = 1) out vec3 RayDirection;

void main()
{	
	gl_Position.xy = inPosition;
	gl_Position.zw = vec2(1.0, 1.0);
	Texcoord = inPosition*0.5 + 0.5;
}