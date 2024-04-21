//--------------------------------------------------------------------------------------
// Including common shader input/output structures and constant buffers
//--------------------------------------------------------------------------------------
#include "Common.hlsli"

//--------------------------------------------------------------------------------------
// Textures and Sampler Declaration
//--------------------------------------------------------------------------------------
Texture2D portalTexture : register(t0); // The portal scene texture
Texture2D tvTexture : register(t1); // The TV frame texture with alpha channel for screen area
SamplerState TVsampler    : register(s0); // Common sampler for both textures

//--------------------------------------------------------------------------------------
// Pixel Shader for TV Portal Effect
//--------------------------------------------------------------------------------------
float4 main(LightingPixelShaderInput input) : SV_TARGET
{
    // Sample both textures using interpolated UV coordinates
    float4 portalColor = portalTexture.Sample(TVsampler, input.uv);
    float4 tvColor = tvTexture.Sample(TVsampler, input.uv);

    // Convert portal scene to grayscale using luminance weighting
    float grayscale = dot(portalColor.rgb, float3(0.299, 0.587, 0.114));
    portalColor.rgb = grayscale.xxx; // Replicate grayscale value across RGB channels

    // Blend the portal and TV textures based on the alpha channel of the TV texture
    // Alpha of 1 indicates full TV texture color, 0 blends to the portal scene
    float4 finalColor = lerp(portalColor, tvColor, tvColor.a);

    return finalColor;
}
