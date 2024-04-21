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
Texture2D DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular
                                                // map (shininess level) in their alpha channel. Repurposing the alpha channel means we can't use alpha blending
                                                // The t0 indicates this texture is in slot 0 and the C++ code must load the texture into the this slot
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(WiggleTextureShaderInput input) : SV_Target
{
    // Distort UV coordinates based on a sine wave and the Wiggle variable
    float sinY = sin(input.uv.y * radians(360.0f) + wiggle);
    float sinX = sin(input.uv.x * radians(360.0f) + wiggle);
    input.uv.x += 0.1f * sinY;
    input.uv.y += 0.1f * sinX;

    // Sample the texture with the distorted UV coordinates
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);

    // Calculate the sine wave value for the fade effect
    float fadeFactor = (sin(wiggle) + 1.0) * 0.5;

    // Define color as the material color
    float3 materialColor = float3(0.8f, 0.5f, 0.5f);

    // Interpolate between white material color and texture color based on fadeFactor
    float3 finalColor = lerp(materialColor, textureColour.rgb, fadeFactor);

    // Return the final color with full opacity
    return float4(finalColor, 1.0f);
}