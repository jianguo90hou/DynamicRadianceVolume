#version 450 core

#include "gbuffer.glsl"
#include "utils.glsl"
#include "globalubos.glsl"
#include "lightingfunctions.glsl"

layout(binding=4) uniform sampler2D RSM_Flux;
layout(binding=5) uniform sampler2D RSM_DepthLinSq;
layout(binding=6) uniform isampler2D RSM_Normal;

in vec2 Texcoord;
out vec3 OutputColor;

void main()
{
	// Get world position and normal.
	vec4 worldPosition4D = vec4(Texcoord * 2.0 - vec2(1.0), textureLod(GBuffer_Depth, Texcoord, 0).r, 1.0) * InverseViewProjection;
	vec3 worldPosition = worldPosition4D.xyz / worldPosition4D.w;
	vec3 worldNormal = UnpackNormal16I(texture(GBuffer_Normal, Texcoord).rg);

	// Direction to camera.
	vec3 toCamera = normalize(vec3(CameraPosition - worldPosition));

	// BRDF parameters from GBuffer.
	vec3 baseColor = textureLod(GBuffer_Diffuse, Texcoord, 0).rgb;
	vec2 roughnessMetallic = textureLod(GBuffer_RoughnessMetalic, Texcoord, 0).rg;

	// Compute direct lighting from all reflective shadow map pixels as virtual point lights.
	// (no area lights yet!)
	// Note that this there is no shadowing!




	//OutputColor = textureLod(RSM_Flux, Texcoord, 0).rgb;
//	return;




	OutputColor = vec3(0.0);

	ivec2 rsmSamplePos = ivec2(0);
	for(; rsmSamplePos.x<RSMReadResolution; ++rsmSamplePos.x)
	{
		rsmSamplePos.y = 0;
		for(; rsmSamplePos.y<RSMReadResolution; ++rsmSamplePos.y)
		{
			
			float lightDepth = texelFetch(RSM_DepthLinSq, rsmSamplePos, 0).r;
			vec4 rsmClipSpace = vec4((rsmSamplePos + vec2(0.5)) / RSMReadResolution * 2.0 - vec2(1.0), 0.0, 1.0) * InverseLightViewProjection;
			vec3 valPosition = LightPosition + normalize(rsmClipSpace.xyz / rsmClipSpace.w - LightPosition) * lightDepth;


			// Direction and distance to light.
			vec3 toVal = valPosition.xyz - worldPosition;
			//if(dot(toVpl, worldNormal) > 0) // Early out if facing away from the light. 
			//	continue;
			float lightDistanceSq = dot(toVal, toVal);
			toVal *= inversesqrt(lightDistanceSq);
			float cosTheta = saturate(dot(toVal, worldNormal));

			// Light intensity
			vec3 valTotalExitantFlux = texelFetch(RSM_Flux, rsmSamplePos, 0).rgb; // Actually this is flux divided with PI!
			vec3 valNormal = UnpackNormal16I(texelFetch(RSM_Normal, rsmSamplePos, 0).xy);


			
			// Handle light as disc with area. This can be computed analytically for the diffuse term.
			vec3 valToLight = valPosition.xyz - LightPosition;
			float valToLightDistSq = dot(valToLight, valToLight); // todo: Compute directly from lightDepth
			float valArea = valToLightDistSq * ValAreaFactor;
			float fluxToIntensity = saturate(dot(valNormal, -toVal));
			float fluxToIrradiance = fluxToIntensity * cosTheta / (lightDistanceSq + valArea);
		//	float fluxToIrradiance = fluxToIntensity * cosTheta / (lightDistanceSq); // VPL instead of VAL

			vec3 irradiance = valTotalExitantFlux * fluxToIrradiance;

			OutputColor += BRDF(toVal, toCamera, worldNormal, baseColor, roughnessMetallic) * irradiance;
		}	
	}


	// Debug rsm
	//OutputColor = textureLod(RSM_Flux, Texcoord * 2, 0 ).rgb;
	//OutputColor = abs(UnpackNormal16I(texture(RSM_Normal, Texcoord * 2).xy));
}