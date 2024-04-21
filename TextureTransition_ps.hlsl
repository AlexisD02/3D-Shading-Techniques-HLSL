//--------------------------------------------------------------------------------------
// Light Model Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader simply samples a diffuse texture map and tints with a fixed colour sent over from the CPU via a constant buffer

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D DiffuseMapA : register(t0); // A diffuse map is the main texture for a model.
                                        // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
Texture2D DiffuseMapB : register(t1); // A diffuse map is the main texture for a model.
                                        // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(TextureTransitionInput input) : SV_Target
{
    // Sample and blend textures
    float3 diffuseMapColourA = DiffuseMapA.Sample(TexSampler, input.uv).rgb;
    float3 diffuseMapColourB = DiffuseMapB.Sample(TexSampler, input.uv).rgb;
    float3 blendedColor = lerp(diffuseMapColourA, diffuseMapColourB, transitionFactor);

    // Normalize the normal vector
    float3 normalizedNormal = normalize(input.worldNormal);

    // Camera direction for specular calculations
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);
    
    // Combine lighting with the blended texture color
    float3 ambientLight = gAmbientColour * blendedColor;
    float3 finalColor = ambientLight * blendedColor;

    return float4(finalColor, 1.0f); // Output the final color with full opacity
}