//--------------------------------------------------------------------------------------
// Per-Pixel Lighting Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader receives position and normal from the vertex shader and uses them to calculate
// lighting per pixel. Also samples a samples a diffuse + specular texture map and combines with light colour.

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular map (shininess) in the a channel
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic - this is the sampler used for the texture above

Texture2D ShadowMapLight1 : register(t1); // Texture holding the view of the scene from a light

SamplerState PointClamp : register(s1); // No filtering for shadow maps (you might think you could use trilinear or similar, but it will filter light depths not the shadows cast...)


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
	// Slight adjustment to calculated depth of pixels so they don't shadow themselves
    const float DepthAdjust = 0.0005f;

    // Normal might have been scaled by model scaling or interpolation so renormalise
    input.worldNormal = normalize(input.worldNormal);

	///////////////////////
	// Calculate lighting
        
    // Direction from pixel to camera
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

	//----------
	// LIGHT 1

    float3 diffuseLight8 = 0; // Initialy assume no contribution from this light
    float3 specularLight8 = 0;

	// Direction from pixel to light
    float3 light8Direction = normalize(gLight8Position - input.worldPosition);

    // Calculate direction from light to pixel
    float3 lightToPixel = normalize(input.worldPosition - gLight8Position);
    float dotProduct = dot(gLight8Facing, lightToPixel);
    
	// Check if pixel is within light cone
    if (dotProduct > gLight8CosHalfAngle) //**** TODO: This condition needs to be written as the first exercise to get spotlights working
           //           As well as the variables above, you also will need values from the constant buffers in "common.hlsli"
    {
	    // Using the world position of the current pixel and the matrices of the light (as a camera), find the 2D position of the
	    // pixel *as seen from the light*. Will use this to find which part of the shadow map to look at.
	    // These are the same as the view / projection matrix multiplies in a vertex shader (can improve performance by putting these lines in vertex shader)
        float4 light8ViewPosition = mul(gLight8ViewMatrix, float4(input.worldPosition, 1.0f));
        float4 light8Projection = mul(gLight8ProjectionMatrix, light8ViewPosition);

		// Convert 2D pixel position as viewed from light into texture coordinates for shadow map - an advanced topic related to the projection step
		// Detail: 2D position x & y get perspective divide, then converted from range -1->1 to UV range 0->1. Also flip V axis
        float2 shadowMapUV = 0.5f * light8Projection.xy / light8Projection.w + float2(0.5f, 0.5f);
        shadowMapUV.y = 1.0f - shadowMapUV.y; // Check if pixel is within light cone

		// Get depth of this pixel if it were visible from the light (another advanced projection step)
        float depthFromLight = light8Projection.z / light8Projection.w - DepthAdjust;
		
		// Compare pixel depth from light with depth held in shadow map of the light. If shadow map depth is less than something is nearer
		// to the light than this pixel - so the pixel gets no effect from this light
        if (depthFromLight < ShadowMapLight1.Sample(PointClamp, shadowMapUV).r)
        {
            float3 light8Dist = length(gLight8Position - input.worldPosition);
            diffuseLight8 = gLight8Colour * max(dot(input.worldNormal, light8Direction), 0) / light8Dist; // Equations from lighting lecture
            float3 halfway = normalize(light8Direction + cameraDirection);
            specularLight8 = diffuseLight8 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference
        }
    }

	// Sum the effect of the lights - add the ambient at this stage rather than for each light (or we will get too much ambient)
    float3 diffuseLight = gAmbientColour + diffuseLight8;
    float3 specularLight = specularLight8;
    
	////////////////////
	// Combine lighting and textures

    // Sample diffuse material and specular material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb; // Diffuse material colour in texture RGB (base colour of model)
    float specularMaterialColour = textureColour.a; // Specular material colour in texture A (shininess of the surface)

    // Combine lighting with texture colours
    float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

    return float4(finalColour, 1.0f); // Always use 1.0f for output alpha - no alpha blending in this lab
}
