//--------------------------------------------------------------------------------------
// Loading GPU shaders
// Creation of constant buffers to help send C++ values to shaders each frame
//--------------------------------------------------------------------------------------

#include "Shader.h"
#include <fstream>
#include <vector>
#include <d3dcompiler.h>

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Globals used to keep code simpler, but try to architect your own code in a better way
//**** Update Shader.h if you add things here ****//

// Vertex and pixel shader DirectX objects
ID3D11VertexShader* gPixelLightingVertexShader = nullptr;
ID3D11PixelShader*  gPixelLightingPixelShader  = nullptr;
ID3D11VertexShader* gLightModelVertexShader = nullptr;
ID3D11PixelShader*  gLightModelPixelShader  = nullptr;
ID3D11PixelShader*  gTVPortalPixelShader = nullptr;
ID3D11VertexShader* gWiggleModelVertexShader = nullptr;
ID3D11PixelShader* gWiggleModelPixelShader = nullptr;
ID3D11PixelShader* gTextureTransitionPixelShader = nullptr;
ID3D11VertexShader* gNormalMappingVertexShader = nullptr;
ID3D11PixelShader* gNormalMappingPixelShader = nullptr;
ID3D11VertexShader* gParallaxMappingVertexShader = nullptr;
ID3D11PixelShader* gParallaxMappingPixelShader = nullptr;
ID3D11VertexShader* gShadowMappingVertexShader = nullptr;
ID3D11PixelShader* gShadowMappingPixelShader = nullptr;
ID3D11VertexShader* gBasicTransformVertexShader = nullptr; // Used before light model and depth-only pixel shader
ID3D11PixelShader* gDepthOnlyPixelShader = nullptr;
ID3D11VertexShader* gFloorVertexShader = nullptr; // Used before light model and depth-only pixel shader
ID3D11PixelShader* gFloorPixelShader = nullptr;
ID3D11VertexShader* gSpecularMapVertexShader = nullptr;
ID3D11PixelShader* gSpecularMapPixelShader = nullptr;
ID3D11VertexShader* gCellShadingOutlineVertexShader = nullptr;
ID3D11PixelShader* gCellShadingOutlinePixelShader = nullptr;
ID3D11VertexShader* gCellShadingVertexShader = nullptr;
ID3D11PixelShader* gCellShadingPixelShader = nullptr;
ID3D11VertexShader* gWiggleTextureVertexShader = nullptr;
ID3D11PixelShader* gWiggleTexturePixelShader = nullptr;
ID3D11VertexShader* gAdditionalVertexShader = nullptr;
ID3D11PixelShader* gAdditionalPixelShader = nullptr;
ID3D11VertexShader* gCrateShadowMappingVertexShader = nullptr;
ID3D11PixelShader* gCrateShadowMappingPixelShader = nullptr;

//--------------------------------------------------------------------------------------
// Shader creation / destruction
//--------------------------------------------------------------------------------------

// Load shaders required for this app, returns true on success
bool LoadShaders()
{
    // Shaders must be added to the Visual Studio project to be compiled, they use the extension ".hlsl".
    // To load them for use, include them here without the extension. Use the correct function for each.
    // Ensure you release the shaders in the ShutdownDirect3D function below
    gPixelLightingVertexShader = LoadVertexShader("PixelLighting_vs"); // Note how the shader files are named to show what type they are
    gPixelLightingPixelShader  = LoadPixelShader ("PixelLighting_ps");
    gLightModelVertexShader = LoadVertexShader("LightModel_vs");
    gLightModelPixelShader  = LoadPixelShader ("LightModel_ps");
    gTVPortalPixelShader = LoadPixelShader("TVPortal_ps");
    gWiggleModelVertexShader = LoadVertexShader("PulsatingSphere_vs");
    gWiggleModelPixelShader = LoadPixelShader("PulsatingSphere_ps");
    gTextureTransitionPixelShader = LoadPixelShader("TextureTransition_ps");
    gNormalMappingVertexShader = LoadVertexShader("NormalMapping_vs");
    gNormalMappingPixelShader = LoadPixelShader("NormalMapping_ps");
    gParallaxMappingVertexShader = LoadVertexShader("ParallaxMapping_vs"); // Note how the shader files are named to show what type they are
    gParallaxMappingPixelShader = LoadPixelShader("ParallaxMapping_ps");
    gShadowMappingVertexShader = LoadVertexShader("ShadowMapping_vs");
    gShadowMappingPixelShader = LoadPixelShader("ShadowMapping_ps");
    gBasicTransformVertexShader = LoadVertexShader("BasicTransform_vs");
    gDepthOnlyPixelShader = LoadPixelShader("DepthOnly_ps");
    gFloorVertexShader = LoadVertexShader("Floor_vs");
    gFloorPixelShader = LoadPixelShader("Floor_ps");
    gSpecularMapVertexShader = LoadVertexShader("TransformLighting_vs");
    gSpecularMapPixelShader = LoadPixelShader("TextureLighting_ps");
    gCellShadingOutlineVertexShader = LoadVertexShader("CellShadingOutline_vs"); // Note how the shader files are named to show what type they are
    gCellShadingOutlinePixelShader = LoadPixelShader("CellShadingOutline_ps");
    gCellShadingVertexShader = LoadVertexShader("CellShading_vs");
    gCellShadingPixelShader = LoadPixelShader("CellShading_ps");
    gWiggleTextureVertexShader = LoadVertexShader("WiggleTexture_vs");
    gWiggleTexturePixelShader = LoadPixelShader("WiggleTexture_ps");
    gAdditionalVertexShader = LoadVertexShader("Additional_vs");
    gAdditionalPixelShader = LoadPixelShader("Additional_ps");
    gCrateShadowMappingVertexShader = LoadVertexShader("CrateShadowMapping_vs");
    gCrateShadowMappingPixelShader = LoadPixelShader("CrateShadowMapping_ps");


    if (gPixelLightingVertexShader == nullptr || gPixelLightingPixelShader == nullptr ||
        gLightModelVertexShader    == nullptr || gLightModelPixelShader    == nullptr ||
        gTVPortalPixelShader       == nullptr || gWiggleModelVertexShader  == nullptr ||
        gWiggleModelPixelShader == nullptr    || gTextureTransitionPixelShader == nullptr ||
        gNormalMappingVertexShader == nullptr || gNormalMappingPixelShader == nullptr ||
        gParallaxMappingVertexShader == nullptr || gParallaxMappingPixelShader == nullptr ||
        gShadowMappingVertexShader == nullptr || gShadowMappingPixelShader == nullptr ||
        gBasicTransformVertexShader == nullptr || gDepthOnlyPixelShader == nullptr ||
        gFloorVertexShader == nullptr || gFloorPixelShader == nullptr ||
        gSpecularMapVertexShader == nullptr || gSpecularMapPixelShader == nullptr ||
        gCellShadingOutlineVertexShader == nullptr || gCellShadingOutlinePixelShader == nullptr ||
        gCellShadingVertexShader == nullptr || gCellShadingPixelShader == nullptr ||
        gWiggleTextureVertexShader == nullptr || gWiggleTexturePixelShader == nullptr ||
        gAdditionalVertexShader == nullptr || gAdditionalPixelShader == nullptr ||
        gCrateShadowMappingVertexShader == nullptr || gCrateShadowMappingPixelShader == nullptr
        )
    {
        gLastError = "Error loading shaders";
        return false;
    }

    return true;
}


void ReleaseShaders()
{
    if (gLightModelVertexShader)     gLightModelVertexShader->Release();
    if (gLightModelPixelShader)      gLightModelPixelShader->Release();
    if (gPixelLightingVertexShader)  gPixelLightingVertexShader->Release();
    if (gPixelLightingPixelShader)   gPixelLightingPixelShader->Release();
    if (gTVPortalPixelShader)        gTVPortalPixelShader->Release();
    if (gWiggleModelVertexShader)        gWiggleModelVertexShader->Release();
    if (gWiggleModelPixelShader)        gWiggleModelPixelShader->Release();
    if (gTextureTransitionPixelShader)  gTextureTransitionPixelShader->Release();
    if (gNormalMappingVertexShader)  gNormalMappingVertexShader->Release();
    if (gNormalMappingPixelShader)   gNormalMappingPixelShader->Release();
    if (gParallaxMappingVertexShader)  gParallaxMappingVertexShader->Release();
    if (gParallaxMappingPixelShader)   gParallaxMappingPixelShader->Release();
    if (gShadowMappingVertexShader)  gShadowMappingVertexShader->Release();
    if (gShadowMappingPixelShader)   gShadowMappingPixelShader->Release();
    if (gFloorVertexShader)  gFloorVertexShader->Release();
    if (gFloorPixelShader)   gFloorPixelShader->Release();
    if (gSpecularMapVertexShader)  gSpecularMapVertexShader->Release();
    if (gSpecularMapPixelShader)   gSpecularMapPixelShader->Release();
    if (gCellShadingVertexShader)         gCellShadingVertexShader->Release();
    if (gCellShadingPixelShader)          gCellShadingPixelShader->Release();
    if (gCellShadingOutlineVertexShader)  gCellShadingOutlineVertexShader->Release();
    if (gCellShadingOutlinePixelShader)   gCellShadingOutlinePixelShader->Release();
    if (gWiggleTextureVertexShader)  gWiggleTextureVertexShader->Release();
    if (gWiggleTexturePixelShader)   gWiggleTexturePixelShader->Release();
    if (gAdditionalVertexShader)  gAdditionalVertexShader->Release();
    if (gAdditionalPixelShader)   gAdditionalPixelShader->Release();
    if (gCrateShadowMappingVertexShader)  gCrateShadowMappingVertexShader->Release();
    if (gCrateShadowMappingPixelShader)   gCrateShadowMappingPixelShader->Release();
}


// Load a vertex shader, include the file in the project and pass the name (without the .hlsl extension)
// to this function. The returned pointer needs to be released before quitting. Returns nullptr on failure. 
ID3D11VertexShader* LoadVertexShader(std::string shaderName)
{
    // Open compiled shader object file
    std::ifstream shaderFile(shaderName + ".cso", std::ios::in | std::ios::binary | std::ios::ate);
    if (!shaderFile.is_open())
    {
        return nullptr;
    }

    // Read file into vector of chars
    std::streamoff fileSize = shaderFile.tellg();
    shaderFile.seekg(0, std::ios::beg);
    std::vector<char> byteCode(fileSize);
    shaderFile.read(&byteCode[0], fileSize);
    if (shaderFile.fail())
    {
        return nullptr;
    }

    // Create shader object from loaded file (we will use the object later when rendering)
    ID3D11VertexShader* shader;
    HRESULT hr = gD3DDevice->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, &shader);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return shader;
}


// Load a pixel shader, include the file in the project and pass the name (without the .hlsl extension)
// to this function. The returned pointer needs to be released before quitting. Returns nullptr on failure. 
// Basically the same code as above but for pixel shaders
ID3D11PixelShader* LoadPixelShader(std::string shaderName)
{
    // Open compiled shader object file
    std::ifstream shaderFile(shaderName + ".cso", std::ios::in | std::ios::binary | std::ios::ate);
    if (!shaderFile.is_open())
    {
        return nullptr;
    }

    // Read file into vector of chars
    std::streamoff fileSize = shaderFile.tellg();
    shaderFile.seekg(0, std::ios::beg);
    std::vector<char>byteCode(fileSize);
    shaderFile.read(&byteCode[0], fileSize);
    if (shaderFile.fail())
    {
        return nullptr;
    }

    // Create shader object from loaded file (we will use the object later when rendering)
    ID3D11PixelShader* shader;
    HRESULT hr = gD3DDevice->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, &shader);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return shader;
}

// Very advanced topic: When creating a vertex layout for geometry (see Scene.cpp), you need the signature
// (bytecode) of a shader that uses that vertex layout. This is an annoying requirement and tends to create
// unnecessary coupling between shaders and vertex buffers.
// This is a trick to simplify things - pass a vertex layout to this function and it will write and compile
// a temporary shader to match. You don't need to know about the actual shaders in use in the app.
// Release the signature (called a ID3DBlob!) after use. Returns nullptr on failure.
ID3DBlob* CreateSignatureForVertexLayout(const D3D11_INPUT_ELEMENT_DESC vertexLayout[], int numElements)
{
    std::string shaderSource = "float4 main(";
    for (int elt = 0; elt < numElements; ++elt)
    {
        auto& format = vertexLayout[elt].Format;
        // This list should be more complete for production use
        if      (format == DXGI_FORMAT_R32G32B32A32_FLOAT) shaderSource += "float4";
        else if (format == DXGI_FORMAT_R32G32B32_FLOAT)    shaderSource += "float3";
        else if (format == DXGI_FORMAT_R32G32_FLOAT)       shaderSource += "float2";
        else if (format == DXGI_FORMAT_R32_FLOAT)          shaderSource += "float";
        else return nullptr; // Unsupported type in layout

        uint8_t index = static_cast<uint8_t>(vertexLayout[elt].SemanticIndex);
        std::string semanticName = vertexLayout[elt].SemanticName;
        semanticName += ('0' + index);

        shaderSource += " ";
        shaderSource += semanticName;
        shaderSource += " : ";
        shaderSource += semanticName;
        if (elt != numElements - 1)  shaderSource += " , ";
    }
    shaderSource += ") : SV_Position {return 0;}";

    ID3DBlob* compiledShader;
    HRESULT hr = D3DCompile(shaderSource.c_str(), shaderSource.length(), NULL, NULL, NULL, "main",
        "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL0, 0, &compiledShader, NULL);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return compiledShader;
}


//--------------------------------------------------------------------------------------
// Constant buffer creation / destruction
//--------------------------------------------------------------------------------------

// Constant Buffers are a way of passing data from C++ to the GPU. They are called constants but that only means
// they are constant for the duration of a single GPU draw call. The "constants" correspond to variables in C++
// that we will change per-model, or per-frame etc.
//
// We typically set up a C++ structure to exactly match the values we need in a shader and then create a constant
// buffer the same size as the structure. That makes updating values from C++ to shader easy - see the main code.

// Create and return a constant buffer of the given size
// The returned pointer needs to be released before quitting. Returns nullptr on failure. 
ID3D11Buffer* CreateConstantBuffer(int size)
{
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = 16 * ((size + 15) / 16);     // Constant buffer size must be a multiple of 16 - this maths rounds up to the nearest multiple
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;             // Indicates that the buffer is frequently updated
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU is only going to write to the constants (not read them)
    cbDesc.MiscFlags = 0;
    ID3D11Buffer* constantBuffer;
    HRESULT hr = gD3DDevice->CreateBuffer(&cbDesc, nullptr, &constantBuffer);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return constantBuffer;
}


