#version 450 core

in vec2 Texcoord;

out vec3 OutputColor;

layout(location = 0) uniform float Exposure;
layout(binding=0) uniform sampler2D HDRInputTexture;

void main()
{
	// No tonemapping yet. Just texture output.
	OutputColor = texture(HDRInputTexture, Texcoord).rgb * Exposure;
}