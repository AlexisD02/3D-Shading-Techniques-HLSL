//--------------------------------------------------------------------------------------
// Light Model Vertex Shader
//--------------------------------------------------------------------------------------
// Basic matrix transformations only

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Vertex shader gets vertices from the mesh one at a time. It transforms their positions
// from 3D into 2D (see lectures) and passes that position down the pipeline so pixels can be rendered. 
WiggleShaderInput main(BasicVertex modelVertex)
{
    WiggleShaderInput output; // This is the data the pixel shader requires from this vertex shader

    // Input position is x,y,z only - need a 4th element to multiply by a 4x4 matrix. Use 1 for a point (0 for a vector) - recall lectures
    float4 modelPosition = float4(modelVertex.position, 1);

    float4 worldPosition = mul(gWorldMatrix, modelPosition);
    
    // Apply distortion based on the wiggle effect
    worldPosition.xy += sin(worldPosition.xy + wiggle) * 0.2f; // Distortion in x direction

    float4 viewPosition = mul(gViewMatrix, worldPosition);
    output.projectedPosition = mul(gProjectionMatrix, viewPosition);
    
        // Also transform model normals into world space using world matrix - lighting will be calculated in world space
    // Pass this normal to the pixel shader as it is needed to calculate per-pixel lighting
    float4 modelNormal = float4(modelVertex.normal, 0); // For normals add a 0 in the 4th element to indicate it is a vector
    output.worldNormal = mul(gWorldMatrix, modelNormal).xyz; // Only needed the 4th element to do this multiplication by 4x4 matrix...
                                                             //... it is not needed for lighting so discard afterwards with the .xyz
    output.worldPosition = worldPosition.xyz; // Also pass world position to pixel shader for lighting

    // Pass texture coordinates (UVs) on to the pixel shader, the vertex shader doesn't need them
    output.uv = modelVertex.uv;

    return output; // Ouput data sent down the pipeline (to the pixel shader)
}
