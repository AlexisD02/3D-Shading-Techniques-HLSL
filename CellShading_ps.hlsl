//--------------------------------------------------------------------------------------
// Vertex shader for cell shading
//--------------------------------------------------------------------------------------

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

// Here we allow the shader access to a texture that has been loaded from the C++ side and stored in GPU memory.
// Note that textures are often called maps (because texture mapping describes wrapping a texture round a mesh).
// Get used to people using the word "texture" and "map" interchangably.
Texture2D DiffuseMap : register(t0); // Diffuse map only
Texture2D CellMap : register(t1); // CellMap is a 1D map that is used to limit the range of colours used in cell shading

SamplerState TexSampler : register(s0); // Sampler for use on textures
SamplerState PointSampleClamp : register(s1); // No filtering of cell maps (otherwise the cell edges would be blurred)


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Pixel shader entry point - each shader has a "main" function
// This shader just samples a diffuse texture map
float4 main(LightingPixelShaderInput input) : SV_Target
{
    // Lighting equations
    input.worldNormal = normalize(input.worldNormal); // Normal might have been scaled by model scaling or interpolation so renormalise
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);

    // Light 10
    float3 light10Vector = gLight10Position - input.worldPosition;
    float light10Distance = length(light10Vector);
    float3 light10Direction = light10Vector / light10Distance;

    float diffuseLevel10 = max(dot(input.worldNormal, light10Direction), 0);
    float cellDiffuseLevel10 = CellMap.Sample(PointSampleClamp, diffuseLevel10).r;
    float3 diffuseLight10 = gLight10Colour * cellDiffuseLevel10 / light10Distance;

    float halfway = normalize(light10Direction + cameraDirection);
    float3 specularLight10 = diffuseLight10 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);


    // Sample diffuse material colour for this pixel from a texture using a given sampler that you set up in the C++ code
    // Ignoring any alpha in the texture, just reading RGB
    float4 textureColour = DiffuseMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb;
    float specularMaterialColour = textureColour.a;

    float3 finalColour = (gAmbientColour + diffuseLight10) * diffuseMaterialColour +
                         (specularLight10) * specularMaterialColour;

    return float4(finalColour, 1.0f); // Always use 1.0f for alpha - no alpha blending in this lab
}