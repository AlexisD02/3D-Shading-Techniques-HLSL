//--------------------------------------------------------------------------------------
// Per-Pixel Lighting Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader receives position and normal from the vertex shader and uses them to calculate
// lighting per pixel. Also samples a diffuse + specular texture map and combines with light colour.

#include "Common.hlsli" // Shaders can also use include files - note the extension

//--------------------------------------------------------------------------------------
// Textures (texture maps)
//--------------------------------------------------------------------------------------

Texture2D DiffuseSpecularMap : register(t0); // Textures here can contain a diffuse map (main colour) in their rgb channels and a specular map (shininess) in the a channel
SamplerState TexSampler : register(s0); // A sampler is a filter for a texture like bilinear, trilinear or anisotropic - this is the sampler used for the texture above

Texture2D NormalHeightMap : register(t1); // Contains normal data and height for parallax mapping
Texture2D ShadowMapLight1 : register(t2); // Shadow map for the first light
Texture2D ShadowMapLight2 : register(t3); // Shadow map for the second light
Texture2D ShadowMapLight3 : register(t4); // Shadow map for the second light

SamplerState PointClamp : register(s1); // No filtering for shadow maps (you might think you could use trilinear or similar, but it will filter light depths not the shadows cast...)z

float4 main(FloorMappingPixelShaderInput input) : SV_Target
{
    // Constants
    const float DepthAdjust = 0.0005f;

    input.modelNormal = normalize(input.modelNormal);
    input.modelTangent = normalize(input.modelTangent);
    float3 modelBiTangent = cross(input.modelNormal, input.modelTangent);
    float3x3 invTangentMatrix = float3x3(input.modelTangent, modelBiTangent, input.modelNormal);
    
    // Camera direction
    float3 cameraDirection = normalize(gCameraPosition - input.worldPosition);
    float3x3 invWorldMatrix = transpose((float3x3) gWorldMatrix);
    float3 cameraModelDir = normalize(mul(invWorldMatrix, cameraDirection)); // Normalise in case world matrix is scaled
    
    float3x3 tangentMatrix = transpose(invTangentMatrix);
    float2 textureOffsetDir = mul(cameraModelDir, tangentMatrix).xy;
    float textureHeight = gParallaxDepth * (NormalHeightMap.Sample(TexSampler, input.uv).a - 0.5f);
    input.uv = input.uv + textureHeight * textureOffsetDir;
    
    float3 textureNormal = 2.0f * NormalHeightMap.Sample(TexSampler, input.uv).rgb - 1.0f; // Scale from 0->1 to -1->1
    input.worldNormal = normalize(mul((float3x3) gWorldMatrix, mul(textureNormal, invTangentMatrix)));
        
    // Light 1
    float3 lightVector = gLight1Position - input.worldPosition;
    float3 lightDist = length(lightVector);
    float3 lightDirection = lightVector / lightDist;
    float3 diffuseLight1 = gLight1Colour * max(dot(input.worldNormal, lightDirection), 0) / lightDist; // Equations from lighting lecture

    float3 halfway = normalize(lightDirection + cameraDirection);
    float3 specularLight1 = diffuseLight1 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower); // Multiplying by diffuseLight instead of light colour - my own personal preference
        
    // Light 5
    float3 diffuseLight5 = 0; // Initialy assume no contribution from this light
    float3 specularLight5 = 0;
    lightDirection = normalize(gLight5Position - input.worldPosition);
    float3 lightToPixel = normalize(input.worldPosition - gLight5Position);
    float dotProduct = dot(gLight5Facing, lightToPixel);
    
    // Check if pixel is within light cone
    if (dotProduct > gLight5CosHalfAngle) {
        float4 lightViewPosition = mul(gLight5ViewMatrix, float4(input.worldPosition, 1.0f));
        float4 lightProjection = mul(gLight5ProjectionMatrix, lightViewPosition);
        float2 shadowMapUV = 0.5f * lightProjection.xy / lightProjection.w + float2(0.5f, 0.5f);
        shadowMapUV.y = 1.0f - shadowMapUV.y; // Check if pixel is within light cone
        float depthFromLight = lightProjection.z / lightProjection.w - DepthAdjust;
        
        if (depthFromLight < ShadowMapLight1.Sample(PointClamp, shadowMapUV).r)
        {
            float3 lightDist = length(gLight5Position - input.worldPosition);
            diffuseLight5 = gLight5Colour * max(dot(input.worldNormal, lightDirection), 0) / lightDist;
            halfway = normalize(lightDirection + cameraDirection);
            specularLight5 = diffuseLight5 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
        }
    }

    // Light 6
    float3 diffuseLight6 = 0; // Use Light 2's colour directly as its contribution
    float3 specularLight6 = 0; // Ambient lights typically do not contribute to specular highlights
    lightDirection = normalize(gLight6Position - input.worldPosition);
    lightToPixel = normalize(input.worldPosition - gLight6Position);
    dotProduct = dot(gLight6Facing, lightToPixel);
    
    if (dotProduct > gLight6CosHalfAngle) {
        float4 lightViewPosition = mul(gLight6ViewMatrix, float4(input.worldPosition, 1.0f));
        float4 lightProjection = mul(gLight6ProjectionMatrix, lightViewPosition);
        float2 shadowMapUV2 = 0.5f * lightProjection.xy / lightProjection.w + float2(0.5f, 0.5f);
        shadowMapUV2.y = 1.0f - shadowMapUV2.y; // Check if pixel is within light cone
        float depthFromLight6 = lightProjection.z / lightProjection.w - DepthAdjust;
        
        if (depthFromLight6 < ShadowMapLight2.Sample(PointClamp, shadowMapUV2).r)
        {
            float3 lightDist = length(gLight6Position - input.worldPosition);
            diffuseLight6 = gLight6Colour * max(dot(input.worldNormal, lightDirection), 0) / lightDist;
            halfway = normalize(lightDirection + cameraDirection);
            specularLight6 = diffuseLight6 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
        }
    }
    
    // Light 8
    float3 diffuseLight8 = 0; // Use Light 2's colour directly as its contribution
    float3 specularLight8 = 0; // Ambient lights typically do not contribute to specular highlights
    lightDirection = normalize(gLight8Position - input.worldPosition);
    lightToPixel = normalize(input.worldPosition - gLight8Position);
    dotProduct = dot(gLight8Facing, lightToPixel);
    
    if (dotProduct > gLight8CosHalfAngle)
    {
        float4 lightViewPosition = mul(gLight8ViewMatrix, float4(input.worldPosition, 1.0f));
        float4 lightProjection = mul(gLight8ProjectionMatrix, lightViewPosition);
        float2 shadowMapUV3 = 0.5f * lightProjection.xy / lightProjection.w + float2(0.5f, 0.5f);
        shadowMapUV3.y = 1.0f - shadowMapUV3.y; // Check if pixel is within light cone
        float depthFromLight8 = lightProjection.z / lightProjection.w - DepthAdjust;
        
        if (depthFromLight8 < ShadowMapLight3.Sample(PointClamp, shadowMapUV3).r)
        {
            float3 lightDist = length(gLight6Position - input.worldPosition);
            diffuseLight8 = gLight8Colour * max(dot(input.worldNormal, lightDirection), 0) / lightDist;
            halfway = normalize(lightDirection + cameraDirection);
            specularLight8 = diffuseLight8 * pow(max(dot(input.worldNormal, halfway), 0), gSpecularPower);
        }
    }

    // Combine lighting from all lights
    float3 diffuseLight = gAmbientColour + (diffuseLight1 + diffuseLight5 + diffuseLight6 + diffuseLight8);
    float3 specularLight = specularLight1 + (specularLight5 + specularLight6 + specularLight8);

    // Sample diffuse and specular material color from texture
    float4 textureColour = DiffuseSpecularMap.Sample(TexSampler, input.uv);
    float3 diffuseMaterialColour = textureColour.rgb;
    float specularMaterialColour = textureColour.a;

    // Combine lighting with texture colors
    float3 finalColour = diffuseLight * diffuseMaterialColour + specularLight * specularMaterialColour;

    return float4(finalColour, 1.0f); // Output color with alpha 1.0
}
