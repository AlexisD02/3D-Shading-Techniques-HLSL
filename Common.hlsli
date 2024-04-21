//--------------------------------------------------------------------------------------
// Common include file for all shaders
//--------------------------------------------------------------------------------------
// Using include files to define the type of data passed between the shaders


//--------------------------------------------------------------------------------------
// Shader input / output
//--------------------------------------------------------------------------------------

// The structure below describes the vertex data to be sent into the vertex shader.
struct BasicVertex
{
    float3 position : position;
    float3 normal   : normal;
    float2 uv       : uv;
};

// This structure describes what data the lighting pixel shader receives from the vertex shader.
// The projected position is a required output from all vertex shaders - where the vertex is on the screen
// The world position and normal at the vertex are sent to the pixel shader for the lighting equations.
// The texture coordinates (uv) are passed from vertex shader to pixel shader unchanged to allow textures to be sampled
struct LightingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition;   // The world position and normal of each vertex is passed to the pixel...
    float3 worldNormal   : worldNormal;     //...shader to calculate per-pixel lighting. These will be interpolated
                                            // automatically by the GPU (rasterizer stage) so each pixel will know
                                            // its position and normal in the world - required for lighting equations
    
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
};


// This structure is similar to the one above but for the light models, which aren't themselves lit
struct SimplePixelShaderInput
{
    float4 projectedPosition : SV_Position;
    float2 uv : uv;
};


// This structure is similar to the one above but for the light models, which aren't themselves lit
struct DepthOnlyInput
{
    float4 projectedPosition : SV_Position;
    float depth : depth;
};

// This structure is similar to the one above but for the light models, which aren't themselves lit
struct WiggleShaderInput
{
    float4 projectedPosition : SV_Position;
    float3 worldPosition : worldPosition;
    float3 worldNormal : worldNormal;
    float2 uv : uv;
};

struct TextureTransitionInput
{
    float4 projectedPosition : SV_Position;
    float3 worldPosition : worldPosition;
    float3 worldNormal : worldNormal;
    float2 uv : uv;
};


// The structure below describes the vertex data to be sent into vertex shaders that need tangents
//****| INFO | Models that contain tangents can only be sent into shaders that accept this structure ****//
struct TangentVertex
{
    float3 position : position;
    float3 normal : normal;
    float3 tangent : tangent;
    float2 uv : uv;
};


//****| INFO |*******************************************************************************************//
// Like per-pixel lighting, normal mapping expects the vertex shader to pass over the position and normal.
// However, it also expects the tangent (see lecturee). Furthermore the normal and tangent are left in
// model space, i.e. they are not transformed by the world matrix in the vertex shader - just sent as is.
// This is because the pixel shader will do the matrix transformations for normals in this case
//*******************************************************************************************************//
// The data sent from vertex to pixel shaders for normal mapping
struct NormalMappingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition; // Data required for lighting calculations in the pixel shader
    float3 worldNormal : worldNormal;
    float3 modelNormal : modelNormal; // --"--
    float3 modelTangent : modelTangent; // --"--
    
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
};

struct FloorMappingPixelShaderInput
{
    float4 projectedPosition : SV_Position; // This is the position of the pixel to render, this is a required input
                                            // to the pixel shader and so it uses the special semantic "SV_Position"
                                            // because the shader needs to identify this important information
    
    float3 worldPosition : worldPosition; // The world position and normal of each vertex is passed to the pixel...
    float3 worldNormal : worldNormal; //...shader to calculate per-pixel lighting. These will be interpolated
                                            // automatically by the GPU (rasterizer stage) so each pixel will know
                                            // its position and normal in the world - required for lighting equations
    float3 modelNormal : modelNormal; // --"--
    float3 modelTangent : modelTangent; // --"--
    
    float2 uv : uv; // UVs are texture coordinates. The artist specifies for every vertex which point on the texture is "pinned" to that vertex.
};

// The most basic pixel shader input, just the screen space position for the pixel
struct BasicPixelShaderInput
{
    float4 projectedPosition : SV_Position;
};

struct WiggleTextureShaderInput
{
    float4 projectedPosition : SV_Position;                                         
    float3 diffuseColour : color;
    float2 uv : uv;
};

//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

// These structures are "constant buffers" - a way of passing variables over from C++ to the GPU
// They are called constants but that only means they are constant for the duration of a single GPU draw call.
// These "constants" correspond to variables in C++ that we will change per-model, or per-frame etc.

// In this exercise the matrices used to position the camera are updated from C++ to GPU every frame along with lighting information
// These variables must match exactly the gPerFrameConstants structure in Scene.cpp
cbuffer PerFrameConstants : register(b0) // The b0 gives this constant buffer the number 0 - used in the C++ code
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix; // The above two matrices multiplied together to combine their effects

    float3   gLight1Position; // 3 floats: x, y z
    float    padding1;        // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
                              // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
                              // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
    float3   gLight1Colour;
    float    padding2;
    
    float3   gLight2Position;
    float    padding3;
    
    float3   gLight2Colour;
    float    padding4;
    
    float3   gLight3Position;
    float    padding5;
    
    float3   gLight3Colour;
    float    padding6;
    
    float3   gLight4Position;
    float    padding7;
    
    float3   gLight4Colour;
    float    padding8;
    
    float3   gLight5Position; // 3 floats: x, y z
    float    padding9; // Pad above variable to float4 (HLSL requirement - copied in the the C++ version of this structure)
    
    float3   gLight5Colour;
    float    padding10;
    
    float3   gLight5Facing; // Spotlight facing direction (normal)
    float    gLight5CosHalfAngle; // cos(Spot light cone angle / 2). Precalculate in C++ the spotlight angle in this form to save doing in the shader
    float4x4 gLight5ViewMatrix; // For shadow mapping we treat lights like cameras so we need camera matrices for them (prepared on the C++ side)
    float4x4 gLight5ProjectionMatrix; // --"--

    float3   gLight6Position;
    float    padding11;
    
    float3   gLight6Colour;
    float    padding12;
    
    float3   gLight6Facing;
    float    gLight6CosHalfAngle;
    float4x4 gLight6ViewMatrix;
    float4x4 gLight6ProjectionMatrix;

    float3   gLight7Position;
    float    padding13;
    
    float3   gLight7Colour;
    float    padding14;
    
    float3   gLight8Position;
    float    padding15;
    
    float3   gLight8Colour;
    float    padding16;
    
    float3   gLight8Facing;
    float    gLight8CosHalfAngle;
    float4x4 gLight8ViewMatrix;
    float4x4 gLight8ProjectionMatrix;
    
    float3   gLight9Position;
    float    padding17;
    
    float3   gLight9Colour;
    float    padding18;
    
    float3   gLight10Position;
    float    padding19;
    
    float3   gLight10Colour;
    float    padding20;
        
    float3   gAmbientColour;    
    float    gSpecularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4
    
    float3   gCameraPosition;
    float    padding21;
    
    float3   gOutlineColour; // Cell shading outline colour    
    float    padding23;
    
    float    gOutlineThickness; // Controls thickness of outlines for cell shading
    float    padding24;
    
    float    wiggle; // Variable for vertex distortion
    float    padding25;
    
    float    transitionFactor; // Control the blend between textures
    float    padding26;
    
    float    gParallaxDepth;
    float    padding27;
    
    float3   colorVariation;
    float    padding28;
    
}
// Note constant buffers are not structs: we don't use the name of the constant buffer, these are really just a collection of global variables (hence the 'g')


// If we have multiple models then we need to update the world matrix from C++ to GPU multiple times per frame because we
// only have one world matrix here. Because this data is updated more frequently it is kept in a different buffer for better performance.
// We also keep other data that changes per-model here
// These variables must match exactly the gPerModelConstants structure in Scene.cpp
cbuffer PerModelConstants : register(b1) // The b1 gives this constant buffer the number 1 - used in the C++ code
{
    float4x4 gWorldMatrix;

    float3   gObjectColour;
    float    padding22;  // See notes on padding in structure above
}