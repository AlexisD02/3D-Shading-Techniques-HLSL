//--------------------------------------------------------------------------------------
// Texture Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader simply samples a diffuse texture map and tints with colours from vertex shadeer

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
float4 main(LightingPixelShaderInput input) : SV_Target
{    
    //**** Lighting equations
    input.worldNormal = normalize(input.worldNormal);
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);
    
    // Light 1
    float3 light9Vector = gLight9Position - input.worldPosition;
    float3 light9Dist = length(light9Vector);
    float3 light9Direction = light9Vector / light9Dist;
    float3 diffuseLight9 = gLight9Colour * max(dot(input.worldNormal, light9Direction), 0) / light9Dist; // Equations from lighting lecture

    float3 halfway = normalize(light9Direction + cameraDirection);
    float3 specularLight9 = diffuseLight9 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference
    
    // Add the diffuse and specular light values from both lights together
    float3 totalDiffuseLight = gAmbientColour + diffuseLight9;
    float3 totalSpecularLight = specularLight9;
    
    // Combine diffuse and specular lighting
    float3 finalLighting = totalDiffuseLight + totalSpecularLight;
    
    // Sample diffuse material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    // Ignoring any alpha in the texture, just reading RGB
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb;
    float specularMaterialIntensity = textureColour.a;
    
    // Apply lighting to the diffuse material color
    float3 finalColour = finalLighting * diffuseMaterialColour * specularMaterialIntensity;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}