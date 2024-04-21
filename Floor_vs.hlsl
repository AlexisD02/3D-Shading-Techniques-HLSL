#include "Common.hlsli" // Shaders can also use include files - note the extension

FloorMappingPixelShaderInput main(TangentVertex modelVertex)
{
    FloorMappingPixelShaderInput output; // This is the data the pixel shader requires from this vertex shader
        // Perform the transformation to world space
    
    float4 worldPosition = mul(gWorldMatrix, float4(modelVertex.position, 1.0f));

    // Transform to view space, then projection space
    float4 viewPosition = mul(gViewMatrix, worldPosition);
    output.projectedPosition = mul(gProjectionMatrix, viewPosition);

    // Pass the world position
    output.worldPosition = worldPosition.xyz;

    // Pass the model space normal and tangent for normal mapping in the pixel shader
    output.modelNormal = modelVertex.normal;
    output.modelTangent = modelVertex.tangent;

    // Transform the normal into world space for lighting calculations
    float4 worldNormal = mul(gWorldMatrix, float4(modelVertex.normal, 0.0f)); // Use 0.0 for w to indicate a direction vector
    output.worldNormal = normalize(worldNormal.xyz); // Normalize the normal as the transformation might skew it

    // Pass through the UV coordinates
    output.uv = modelVertex.uv;

    return output;
}