//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "Mesh.h"
#include "MeshAnimation.h"
#include "Model.h"
#include "ModelAnimation.h"
#include "Camera.h"
#include "State.h"
#include "Shader.h"
#include "Input.h"
#include "Common.h"
#include "Texture.h" // Include Texture class
#include "Light.h" // Include Light class

#include "CVector2.h" 
#include "CVector3.h" 
#include "CMatrix4x4.h"
#include "MathHelpers.h"     // Helper functions for maths
#include "GraphicsHelpers.h" // Helper functions to unclutter the code here

#include "ColourRGBA.h" 

#include <sstream>
#include <vector>
#include <memory>

std::vector<std::unique_ptr<Texture>> textures; // Vector of textures managed with smart pointers for automatic memory management

//--------------------------------------------------------------------------------------
// Scene Data
//--------------------------------------------------------------------------------------
// Addition of Mesh, Model and Camera classes have greatly simplified this section
// Geometry data has gone to Mesh class. Positions, rotations, matrices have gone to Model and Camera classes

// Constants controlling speed of movement/rotation (measured in units per second because we're using frame time)
const float ROTATION_SPEED = 2.0f;  // 2 radians per second for rotation
const float MOVEMENT_SPEED = 50.0f; // 50 units per second for movement (what a unit of length is depends on 3D model - i.e. an artist decision usually)


// Meshes, models and cameras, same meaning as TL-Engine. Meshes prepared in InitGeometry function, Models & camera in InitScene
Mesh* gCubeMesh;
Mesh* gCubeMeshAdvanced;
Mesh* gDecalMesh;
Mesh* gCrateMesh;
Mesh* gSphereMesh;
Mesh* gGroundMesh;
Mesh* gLightMesh;
Mesh* gPortalMesh;
Mesh* gSecondPortalMesh;
Mesh* gTeapotMesh;
Mesh* gCharacterMesh;
Mesh* gTrollMesh;
Mesh* gCubeMultiMesh;
MeshAnimation* gAnimatedMesh;

Model* gDecal;
Model* gCrate;
Model* gSphere;
Model* gGround;
Model* gPortal;
Model* gSecondPortal;
Model* gTeapot;
Model* gCharacter;
Model* gTroll;
Model* gCubeMulti;
ModelAnimation* gBike;

// Two cameras now. The main camera, and the view through the portal
Camera* gCamera;
Camera* gPortalCamera;


// Store lights in an array in this exercise
const int NUM_LIGHTS = 10;
Light gLights[NUM_LIGHTS];

const int NUM_CUBES = 7;
Model* gCube[NUM_LIGHTS];

CVector3 gAmbientColour = { 0.4f, 0.4f, 0.5f }; // Background level of light (slightly bluish to match the far background, which is dark blue)
float    gSpecularPower = 256; // Specular power controls shininess - same for all models in this app

ColourRGBA gBackgroundColor = { 0.2f, 0.2f, 0.3f , 1.0f };


// Variables controlling light1's orbiting of the cube
const float gLightOrbit = 20.0f;
const float gLightOrbitSpeed = 0.7f;

// Spotlight data - using spotlights in this lab because shadow mapping needs to treat each light as a camera, which is easy with spotlights
float gSpotlightConeAngle = 90.0f; // Spot light cone angle (degrees), like the FOV (field-of-view) of the spot light

float gParallaxDepth = 0.08f; // Overall depth of bumpiness for parallax mapping
bool gUseParallax = true;  // Toggle for parallax

// Cell shading data
CVector3 OutlineColour = { 0, 0, 0 };
float    OutlineThickness = 0.015f;

// Lock FPS to monitor refresh rate, which will typically set it to 60fps. Press 'p' to toggle to full fps
bool lockFPS = true;


//--------------------------------------------------------------------------------------
//**** Shadow Texture  ****//
//--------------------------------------------------------------------------------------
// This texture will have the scene from the point of view of the light renderered on it. This texture is then used for shadow mapping

// Dimensions of shadow map texture - controls quality of shadows
int gShadowMapSize = 2048;

// The shadow texture - effectively a depth buffer of the scene **from the light's point of view**
//                      Each frame it is rendered to, then the texture is used to help the per-pixel lighting shader identify pixels in shadow
ID3D11Texture2D* gShadowMap1Texture = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11DepthStencilView* gShadowMap1DepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
ID3D11ShaderResourceView* gShadowMap1SRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

ID3D11Texture2D* gShadowMap2Texture = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11DepthStencilView* gShadowMap2DepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
ID3D11ShaderResourceView* gShadowMap2SRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

ID3D11Texture2D* gShadowMap3Texture = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11DepthStencilView* gShadowMap3DepthStencil = nullptr; // This object is used when we want to render to the texture above **as a depth buffer**
ID3D11ShaderResourceView* gShadowMap3SRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
//*********************//


//--------------------------------------------------------------------------------------
//**** Portal Texture  ****//
//--------------------------------------------------------------------------------------
// This texture will have the scene renderered on it. Then the texture is applied to a model

// Dimensions of portal texture - controls quality of rendered scene in portal
int gPortalWidth  = 512;
int gPortalHeight = 512;

// The portal texture - each frame it is rendered to, then it is used as a texture for model
ID3D11Texture2D*          gPortalTexture      = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11Texture2D*          gSecondPortalTexture = nullptr;
ID3D11RenderTargetView*   gPortalRenderTarget = nullptr; // This object is used when we want to render to the texture above
ID3D11RenderTargetView*   gSecondPortalRenderTarget = nullptr;
ID3D11ShaderResourceView* gPortalTextureSRV   = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)
ID3D11ShaderResourceView* gSecondPortalTextureSRV = nullptr;

// Also need a depth/stencil buffer for the portal - it's just another kind of texture
// NOTE: ***Can share this depth buffer between multiple portals of the same size***
ID3D11Texture2D*        gPortalDepthStencil     = nullptr; // This object represents the memory used by the texture on the GPU
ID3D11DepthStencilView* gPortalDepthStencilView = nullptr; // This object is used when we want to use the texture above as the depth buffer

//*********************//

int gCubeMapWidth = 1024;
int gCubeMapHeight = 1024;

ID3D11Texture2D* gCubeMapTexture = nullptr;
ID3D11ShaderResourceView* gCubeMapTextureSRV = nullptr; // This object is used when we want to render to the texture above


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
// Variables sent over to the GPU each frame
// The structures are now in Common.h
// IMPORTANT: Any new data you add in C++ code (CPU-side) is not automatically available to the GPU
//            Anything the shaders need (per-frame or per-model) needs to be sent via a constant buffer

PerFrameConstants gPerFrameConstants;      // The constants that need to be sent to the GPU each frame (see common.h for structure)
ID3D11Buffer*     gPerFrameConstantBuffer; // The GPU buffer that will recieve the constants above

PerModelConstants gPerModelConstants;      // As above, but constant that change per-model (e.g. world matrix)
ID3D11Buffer*     gPerModelConstantBuffer; // --"--



//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

ID3D11Resource* gCubeMultiDiffuseMap = nullptr;
ID3D11ShaderResourceView* gCubeMultiTextureSRVs[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };


//--------------------------------------------------------------------------------------
// Light Helper Functions
//--------------------------------------------------------------------------------------

// Get "camera-like" view matrix for a spotlight
CMatrix4x4 CalculateLightViewMatrix(int lightIndex)
{
    return InverseAffine(gLights[lightIndex].GetModel()->WorldMatrix());
}

// Get "camera-like" projection matrix for a spotlight
CMatrix4x4 CalculateLightProjectionMatrix(int lightIndex)
{
    return MakeProjectionMatrix(1.0f, ToRadians(gSpotlightConeAngle)); // Helper function in Utility\GraphicsHelpers.cpp
}


//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
    // Load mesh geometry data, just like TL-Engine this doesn't create anything in the scene. Create a Model for that.
    // IMPORTANT NOTE: Will only keep the first object from the mesh - multipart objects will have parts missing - see later lab for more robust loader
    try 
    {
        gCubeMesh   = new Mesh("Cube.x");
        gCubeMeshAdvanced = new Mesh("Cube.x", true);
        gDecalMesh  = new Mesh("Decal.x");
        gCrateMesh  = new Mesh("CargoContainer.x");
        gSphereMesh = new Mesh("Sphere.x");
        gGroundMesh = new Mesh("Floor.x", true);
        gLightMesh  = new Mesh("Light.x");
        gPortalMesh = new Mesh("Cube.x");
        gSecondPortalMesh = new Mesh("Sphere.x");
        gTeapotMesh = new Mesh("Teapot.x");
        gCharacterMesh = new Mesh("Troll.x");
        gTrollMesh = new Mesh("Troll.x");
        gCubeMultiMesh = new Mesh("Cube.x");
        gAnimatedMesh = new MeshAnimation("Bike.x");
    }
    catch (std::runtime_error e)  // Constructors cannot return error messages so use exceptions to catch mesh errors (fairly standard approach this)
    {
        gLastError = e.what(); // This picks up the error message put in the exception (see Mesh.cpp)
        return false;
    }


    // Load the shaders required for the geometry we will use (see Shader.cpp / .h)
    if (!LoadShaders())
    {
        gLastError = "Error loading shaders";
        return false;
    }


    // Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
    // These allow us to pass data from CPU to shaders such as lighting information or matrices
    // See the comments above where these variable are declared and also the UpdateScene function
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
        return false;
    }


    //// Load / prepare textures on the GPU ////

    // Load textures and create DirectX objects for them
    // The LoadTexture function requires you to pass a ID3D11Resource* (e.g. &gCubeDiffuseMap), which manages the GPU memory for the
    // texture and also a ID3D11ShaderResourceView* (e.g. &gCubeDiffuseMapSRV), which allows us to use the texture in shaders
    // The function will fill in these pointers with usable data. The variables used here are globals found near the top of the file.
    if (
        !LoadTexture("negx.jpg", &gCubeMultiDiffuseMap, &gCubeMultiTextureSRVs[0]) ||
        !LoadTexture("negy.jpg", &gCubeMultiDiffuseMap, &gCubeMultiTextureSRVs[1]) ||
        !LoadTexture("negz.jpg", &gCubeMultiDiffuseMap, &gCubeMultiTextureSRVs[2]) ||
        !LoadTexture("posx.jpg", &gCubeMultiDiffuseMap, &gCubeMultiTextureSRVs[3]) ||
        !LoadTexture("posy.jpg", &gCubeMultiDiffuseMap, &gCubeMultiTextureSRVs[4]) ||
        !LoadTexture("posz.jpg", &gCubeMultiDiffuseMap, &gCubeMultiTextureSRVs[5])
        )
    {
        gLastError = "Error loading textures";
        return false;
    }

    try {
        textures.emplace_back(std::make_unique<Texture>("StoneDiffuseSpecular.dds"));
        textures.emplace_back(std::make_unique<Texture>("PatternDiffuseSpecular.dds"));
        textures.emplace_back(std::make_unique<Texture>("PatternNormal.dds"));
        textures.emplace_back(std::make_unique<Texture>("TechDiffuseSpecular.dds"));
        textures.emplace_back(std::make_unique<Texture>("TechNormalHeight.dds"));
        textures.emplace_back(std::make_unique<Texture>("Moogle.png"));
        textures.emplace_back(std::make_unique<Texture>("CargoA.dds"));
        textures.emplace_back(std::make_unique<Texture>("Brick1.jpg"));
        textures.emplace_back(std::make_unique<Texture>("wood2.jpg"));
        textures.emplace_back(std::make_unique<Texture>("CobbleDiffuseSpecular.dds"));
        textures.emplace_back(std::make_unique<Texture>("CobbleNormalHeight.dds"));
        textures.emplace_back(std::make_unique<Texture>("Flare.jpg"));
        textures.emplace_back(std::make_unique<Texture>("tv.dds"));
        textures.emplace_back(std::make_unique<Texture>("TrollDiffuseSpecular.dds"));
        textures.emplace_back(std::make_unique<Texture>("MetalDiffuseSpecular.dds"));
        textures.emplace_back(std::make_unique<Texture>("Green.png"));
        textures.emplace_back(std::make_unique<Texture>("CellGradient.png"));
        textures.emplace_back(std::make_unique<Texture>("glass.jpg"));
        // Continue for other textures as needed
    }
    catch (const std::runtime_error& e) {
        // Handle error - since std::cerr isn't supported, adapt this to your needs
        gLastError = "Error loading textures: " + std::string(e.what());
        return false;
    }




    //**** Create Portal Texture ****//

	// Using a helper function to load textures from files above. Here we create the portal texture manually
	// as we are creating a special kind of texture (one that we can render to). Many settings to prepare:
	D3D11_TEXTURE2D_DESC portalDesc = {};
	portalDesc.Width  = gPortalWidth;  // Size of the portal texture determines its quality
	portalDesc.Height = gPortalHeight;
	portalDesc.MipLevels = 1; // No mip-maps when rendering to textures (or we would have to render every level)
	portalDesc.ArraySize = 1;
	portalDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA texture (8-bits each)
	portalDesc.SampleDesc.Count = 1;
	portalDesc.SampleDesc.Quality = 0;
	portalDesc.Usage = D3D11_USAGE_DEFAULT;
	portalDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE; // IMPORTANT: Indicate we will use texture as render target, and pass it to shaders
	portalDesc.CPUAccessFlags = 0;
	portalDesc.MiscFlags = 0;
	if (FAILED( gD3DDevice->CreateTexture2D(&portalDesc, NULL, &gPortalTexture) ))
	{
		gLastError = "Error creating portal texture";
		return false;
	}

    if (FAILED(gD3DDevice->CreateTexture2D(&portalDesc, NULL, &gSecondPortalTexture)))
    {
        gLastError = "Error creating second portal texture";
        return false;
    }

	// We created the portal texture above, now we get a "view" of it as a render target, i.e. get a special pointer to the texture that
	// we use when rendering to it (see RenderScene function below)
	if (FAILED( gD3DDevice->CreateRenderTargetView(gPortalTexture, NULL, &gPortalRenderTarget) ))
	{
		gLastError = "Error creating portal render target view";
		return false;
	}

    if (FAILED(gD3DDevice->CreateRenderTargetView(gSecondPortalTexture, NULL, &gSecondPortalRenderTarget)))
    {
        gLastError = "Error creating second portal render target view";
        return false;
    }

	// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc = {};
	srDesc.Format = portalDesc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;
	if (FAILED( gD3DDevice->CreateShaderResourceView(gPortalTexture, &srDesc, &gPortalTextureSRV) ))
	{
		gLastError = "Error creating portal shader resource view";
		return false;
	}

    if (FAILED(gD3DDevice->CreateShaderResourceView(gSecondPortalTexture, &srDesc, &gSecondPortalTextureSRV)))
    {
        gLastError = "Error creating second portal shader resource view";
        return false;
    }


	//**** Create Portal Depth Buffer ****//

	// We also need a depth buffer to go with our portal
	//**** This depth buffer can be shared with any other portals of the same size
    portalDesc = {};
	portalDesc.Width  = gPortalWidth;
	portalDesc.Height = gPortalHeight;
	portalDesc.MipLevels = 1;
	portalDesc.ArraySize = 1;
	portalDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth buffers contain a single float per pixel
	portalDesc.SampleDesc.Count = 1;
	portalDesc.SampleDesc.Quality = 0;
	portalDesc.Usage = D3D11_USAGE_DEFAULT;
	portalDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	portalDesc.CPUAccessFlags = 0;
	portalDesc.MiscFlags = 0;
	if (FAILED(gD3DDevice->CreateTexture2D(&portalDesc, NULL, &gPortalDepthStencil) ))
	{
		gLastError = "Error creating portal depth stencil texture";
		return false;
	}

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC portalDescDSV = {};
	portalDescDSV.Format = portalDesc.Format;
	portalDescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	portalDescDSV.Texture2D.MipSlice = 0;
    portalDescDSV.Flags = 0;
	if (FAILED(gD3DDevice->CreateDepthStencilView(gPortalDepthStencil, &portalDescDSV, &gPortalDepthStencilView) ))
	{
		gLastError = "Error creating portal depth stencil view";
		return false;
	}

    //**** Create Shadow Map texture ****//

    // We also need a depth buffer to go with our portal
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = gShadowMapSize; // Size of the shadow map determines quality / resolution of shadows
    textureDesc.Height = gShadowMapSize;
    textureDesc.MipLevels = 1; // 1 level, means just the main texture, no additional mip-maps. Usually don't use mip-maps when rendering to textures (or we would have to render every level)
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // The shadow map contains a single 32-bit value [tech gotcha: have to say typeless because depth buffer and shaders see things slightly differently]
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE; // Indicate we will use texture as a depth buffer and also pass it to shaders
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    if (FAILED(gD3DDevice->CreateTexture2D(&textureDesc, NULL, &gShadowMap1Texture)))
    {
        gLastError = "Error creating shadow map texture";
        return false;
    }
    if (FAILED(gD3DDevice->CreateTexture2D(&textureDesc, NULL, &gShadowMap2Texture)))
    {
        gLastError = "Error creating second shadow map texture";
        return false;
    }
    if (FAILED(gD3DDevice->CreateTexture2D(&textureDesc, NULL, &gShadowMap3Texture)))
    {
        gLastError = "Error creating second shadow map texture";
        return false;
    }


    // Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // See "tech gotcha" above. The depth buffer sees each pixel as a "depth" float
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    dsvDesc.Flags = 0;
    if (FAILED(gD3DDevice->CreateDepthStencilView(gShadowMap1Texture, &dsvDesc, &gShadowMap1DepthStencil)))
    {
        gLastError = "Error creating shadow map depth stencil view";
        return false;
    }
    if (FAILED(gD3DDevice->CreateDepthStencilView(gShadowMap2Texture, &dsvDesc, &gShadowMap2DepthStencil)))
    {
        gLastError = "Error creating second shadow map depth stencil view";
        return false;
    }
    if (FAILED(gD3DDevice->CreateDepthStencilView(gShadowMap3Texture, &dsvDesc, &gShadowMap3DepthStencil)))
    {
        gLastError = "Error creating second shadow map depth stencil view";
        return false;
    }


    // We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // See "tech gotcha" above. The shaders see textures as colours, so shadow map pixels are not seen as depths
    // but rather as "red" floats (one float taken from RGB). Although the shader code will use the value as a depth
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    if (FAILED(gD3DDevice->CreateShaderResourceView(gShadowMap1Texture, &srvDesc, &gShadowMap1SRV)))
    {
        gLastError = "Error creating shadow map shader resource view";
        return false;
    }
    if (FAILED(gD3DDevice->CreateShaderResourceView(gShadowMap2Texture, &srvDesc, &gShadowMap2SRV)))
    {
        gLastError = "Error creating second shadow map shader resource view";
        return false;
    }
    if (FAILED(gD3DDevice->CreateShaderResourceView(gShadowMap3Texture, &srvDesc, &gShadowMap3SRV)))
    {
        gLastError = "Error creating second shadow map shader resource view";
        return false;
    }

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = gCubeMapWidth;
    texDesc.Height = gCubeMapHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 6; // Cube map has 6 textures
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    // Create the texture for the cube map
    if (FAILED(gD3DDevice->CreateTexture2D(&texDesc, NULL, &gCubeMapTexture)))
    {
        gLastError = "Error creating cube map texture";
        return false;
    }

    // Load textures for each face of the cube map
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc1 = {};
    srvDesc1.Format = texDesc.Format;
    srvDesc1.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc1.TextureCube.MostDetailedMip = 0;
    srvDesc1.TextureCube.MipLevels = texDesc.MipLevels;

    if (FAILED(gD3DDevice->CreateShaderResourceView(gCubeMapTexture, &srvDesc1, &gCubeMapTextureSRV))) {
        gLastError = "Error creating cube map shader resource view";
        return false;
    }
    
    //*****************************//

  	// Create all filtering modes, blending modes etc. used by the app (see State.cpp/.h)
	if (!CreateStates())
	{
		gLastError = "Error creating states";
		return false;
	}

	return true;
}


// Prepare the scene
// Returns true on success
bool InitScene()
{
    //// Set up scene ////
    gDecal = new Model(gDecalMesh); // Initialize a new decal model with the decal mesh
    gCrate = new Model(gCrateMesh); // Initialize a new crate model with the crate mesh
    gSphere = new Model(gSphereMesh); // Initialize a new sphere model with the sphere mesh
    gGround = new Model(gGroundMesh); // Initialize a new ground model with the ground mesh
    gCubeMulti = new Model(gCubeMultiMesh); // Initialize a new multi-textured cube model with the cube multi-mesh
    gBike = new ModelAnimation(gAnimatedMesh); // Initialize a new bike model with animation capabilities

    // Light set-up - using an array to manage multiple lights
    for (int i = 0; i < NUM_LIGHTS; i++) {
        gLights[i].SetModel(new Model(gLightMesh)); // Assign a new light model to each light in the array
    }

    // Cube set-up - using an array to manage multiple cubes
    for (int i = 0; i < NUM_CUBES; i++) {
        if (i == 2 || i == 3) {
            gCube[i] = new Model(gCubeMeshAdvanced); // Assign an advanced cube model for supporting normal and parllax mapping
        }
        else {
            gCube[i] = new Model(gCubeMesh); // Assign a standard cube model to other cube indices
        }
    }

    // Create models for the portal, teapot, character, and troll
    gPortal = new Model(gPortalMesh); // Initialize a new portal model with the portal mesh
    gSecondPortal = new Model(gSecondPortalMesh); // Initialize a new second portal model with the second portal mesh
    gTeapot = new Model(gTeapotMesh); // Initialize a new teapot model with the teapot mesh
    gCharacter = new Model(gCharacterMesh); // Initialize a new character model with the character mesh
    gTroll = new Model(gTrollMesh); // Initialize a new troll model with the troll mesh

    // Setup the first light
    gLights[0].SetColor({ 0.8f, 0.8f, 1.0f }); // Set the color of the first light to a light blue
    gLights[0].SetStrength(10); // Set the strength/intensity of the first light
    gLights[0].GetModel()->SetPosition({ 0, 0, 0 }); // Position the first light's model at the origin
    gLights[0].GetModel()->SetScale(pow(gLights[0].GetStrength(), 0.7f)); // Scale the first light's model based on its strength for visual representation

    gLights[1].SetColor({ 1.0f, 0.8f, 0.2f });
    gLights[1].SetStrength(40);
    gLights[1].GetModel()->SetPosition({ -90, 40, -90 });
    gLights[1].GetModel()->SetScale(pow(gLights[1].GetStrength(), 0.7f));

    gLights[2].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[2].SetStrength(10);
    gLights[2].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[2].GetModel()->SetScale(pow(gLights[2].GetStrength(), 0.7f));

    gLights[3].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[3].SetStrength(10);
    gLights[3].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[3].GetModel()->SetScale(pow(gLights[3].GetStrength(), 0.7f));

    gLights[4].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[4].SetStrength(10);
    gLights[4].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[4].GetModel()->SetScale(pow(gLights[4].GetStrength(), 0.7f));
    gLights[4].GetModel()->FaceTarget(gCharacter->Position());

    gLights[5].SetColor({ 1.0f, 0.8f, 0.2f });
    gLights[5].SetStrength(40);
    gLights[5].GetModel()->SetPosition({ -20, 30, -60 });
    gLights[5].GetModel()->SetScale(pow(gLights[5].GetStrength(), 0.7f));
    gLights[5].GetModel()->FaceTarget(gCharacter->Position());

    gLights[6].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[6].SetStrength(10);
    gLights[6].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[6].GetModel()->SetScale(pow(gLights[6].GetStrength(), 0.7f));

    gLights[7].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[7].SetStrength(40);
    gLights[7].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[7].GetModel()->SetScale(pow(gLights[7].GetStrength(), 0.7f));

    gLights[8].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[8].SetStrength(10);
    gLights[8].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[8].GetModel()->SetScale(pow(gLights[8].GetStrength(), 0.7f));

    gLights[9].SetColor({ 0.8f, 0.8f, 1.0f });
    gLights[9].SetStrength(10);
    gLights[9].GetModel()->SetPosition({ 0, 0, 0 });
    gLights[9].GetModel()->SetScale(pow(gLights[9].GetStrength(), 0.7f));

	// Initial positions
    gCube[0]->SetPosition({ -90, 15, 0 }); // Set the position of the first cube to coordinates (-90, 15, 0) in the scene
    gCube[0]->SetScale(1.5f); // Set the scale of the first cube to 1.5 times its original size

    gCube[1]->SetPosition({-90, 15, 90});
    gCube[1]->SetScale(1.5f);

    gCube[2]->SetPosition({0, 15, 90});
    gCube[2]->SetScale(1.5f);

    gCube[3]->SetPosition({0, 15, 0});
    gCube[3]->SetScale(1.5f);

    gCube[4]->SetPosition({-90, 15, -180});
    gCube[4]->SetScale(1.5f);

    gCube[5]->SetPosition({90, 15, -180});

    gCube[6]->SetPosition({-90, 15, 180});
    gCube[6]->SetScale(1.5f);

    gDecal->SetPosition({ 90, 15, -180.1f });

	gSphere->SetPosition({ 90, 15,  0 });

	gCrate->SetPosition({ 0, 0, -180 });
	gCrate->SetScale( 6.0f );
	gCrate->SetRotation({ 0.0f, ToRadians(40.0f), 0.0f });

	gPortal->SetPosition({ 90, 15, 90 });
    gPortal->SetScale(1.5f);

    gSecondPortal->SetPosition({ 90, 15, -90 });
    gSecondPortal->SetScale(1.0f);

    gTeapot->SetPosition({ -90, 10, -90 });
    gTeapot->SetScale(1.5f);

    gCharacter->SetPosition({ 0, 0, -90 });
    gCharacter->SetScale(8);
    gCharacter->SetRotation({ 0, ToRadians(215.0f), 0 });

    gTroll->SetPosition({ 0, 0, 180 });
    gTroll->SetScale(8.0f);
    gTroll->SetRotation({ 0, ToRadians(215.0f), 0 });

    gBike->SetPosition({ 0, 0, 270 });
    gBike->SetScale(3.0f);
    gBike->SetRotation({ 0, ToRadians(215.0f), 0 });

    gCubeMulti->SetPosition({ 90, 15, 180 });
    gCubeMulti->SetScale(1.5f);

    //// Set up cameras ////

    gCamera = new Camera();
    gCamera->SetPosition({ 40, 50, -300 });
    gCamera->SetRotation({ ToRadians(15.0f), ToRadians(-18.0f), 0.0f });
    gCamera->SetNearClip(1.0f); // Increase from 0.0001f to reduce precision issues
    gCamera->SetFarClip(5000.0f); // Decrease from 100000.0f, assuming scene content doesn't extend beyond this distance


  	//**** Portal camera is the view shown in the portal object's texture ****//
	gPortalCamera = new Camera();
	gPortalCamera->SetPosition({ 45, 45, 85 });
	gPortalCamera->SetRotation({ ToRadians(20.0f), ToRadians(215.0f), 0 });

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();

    if (gShadowMap1DepthStencil)  gShadowMap1DepthStencil->Release();
    if (gShadowMap1SRV)           gShadowMap1SRV->Release();
    if (gShadowMap1Texture)       gShadowMap1Texture->Release();

    if (gShadowMap2DepthStencil)  gShadowMap2DepthStencil->Release();
    if (gShadowMap2SRV)           gShadowMap2SRV->Release();
    if (gShadowMap2Texture)       gShadowMap2Texture->Release();

    if (gCubeMapTextureSRV)           gCubeMapTextureSRV->Release();
    if (gCubeMapTexture)       gCubeMapTexture->Release();

    if (gShadowMap3DepthStencil)  gShadowMap3DepthStencil->Release();
    if (gShadowMap3SRV)           gShadowMap3SRV->Release();
    if (gShadowMap3Texture)       gShadowMap3Texture->Release();

    if (gPortalDepthStencilView)  gPortalDepthStencilView->Release();
    if (gPortalDepthStencil)      gPortalDepthStencil->Release();
    if (gPortalTextureSRV)        gPortalTextureSRV->Release();
    if (gPortalRenderTarget)      gPortalRenderTarget->Release();
    if (gPortalTexture)           gPortalTexture->Release();

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    ReleaseShaders();

    // See note in InitGeometry about why we're not using unique_ptr and having to manually delete
    delete gCamera;        gCamera       = nullptr;
    delete gPortalCamera;  gPortalCamera = nullptr;

    delete gPortal;  gPortal = nullptr;
    for (int i = 0; i < NUM_LIGHTS; i++) {
        delete gLights[i].GetModel();  gLights[i].SetModel(nullptr);
    }
    for (int i = 0; i < NUM_CUBES; i++) {
        delete gCube[i];    gCube[i] = nullptr;
    }
    delete gGround;  gGround = nullptr;
    delete gSphere;  gSphere = nullptr;
    delete gCrate;   gCrate  = nullptr;
    delete gDecal;   gDecal  = nullptr;
    delete gCubeMulti;    gCubeMulti = nullptr;
    delete gCharacter; gCharacter = nullptr;
    delete gTroll; gTroll = nullptr;
    delete gBike; gBike = nullptr;

    delete gPortalMesh;  gPortalMesh = nullptr;
    delete gLightMesh;   gLightMesh  = nullptr;
    delete gGroundMesh;  gGroundMesh = nullptr;
    delete gSphereMesh;  gSphereMesh = nullptr;
    delete gCrateMesh;   gCrateMesh  = nullptr;
    delete gDecalMesh;   gDecalMesh  = nullptr;
    delete gCubeMesh;    gCubeMesh = nullptr;
    delete gCubeMeshAdvanced;    gCubeMeshAdvanced = nullptr;
    delete gCubeMultiMesh;    gCubeMultiMesh = nullptr;
    delete gAnimatedMesh;    gAnimatedMesh = nullptr;
    delete gCharacterMesh; gCharacterMesh = nullptr;
    delete gTrollMesh; gTrollMesh = nullptr;
}



//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Render the scene from the given light's point of view. Only renders depth buffer
void RenderDepthBufferFromLight(int lightIndex)
{
    // Get camera-like matrices from the spotlight, seet in the constant buffer and send over to GPU
    gPerFrameConstants.viewMatrix = CalculateLightViewMatrix(lightIndex);
    gPerFrameConstants.projectionMatrix = CalculateLightProjectionMatrix(lightIndex);
    gPerFrameConstants.viewProjectionMatrix = gPerFrameConstants.viewMatrix * gPerFrameConstants.projectionMatrix;
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //// Only render models that cast shadows ////

    // Use special depth-only rendering shaders
    gD3DContext->VSSetShader(gBasicTransformVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gDepthOnlyPixelShader, nullptr, 0);

    // States - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullBackState);

    // Render models - no state changes required between each object in this situation (no textures used in this step)
    gGround->Render();
    gCharacter->Render();
    gCrate->Render();
}


// Render everything in the scene from the given camera
// This code is common between rendering the main scene and rendering the scene in the portal
// See RenderScene function below
void RenderSceneFromCamera(Camera* camera)
{
    // Set camera matrices in the constant buffer and send over to GPU
    gPerFrameConstants.viewMatrix           = camera->ViewMatrix();
    gPerFrameConstants.projectionMatrix     = camera->ProjectionMatrix();
    gPerFrameConstants.viewProjectionMatrix = camera->ViewProjectionMatrix();
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    //-------------------------------------------------------------------------
    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    // States - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullBackState);


    gD3DContext->VSSetShader(gFloorVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gFloorPixelShader, nullptr, 0);
    
    // Select the approriate textures and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &textures[9]->GetTextureSRV()); // First parameter must match texture slot number in the shader
    gD3DContext->PSSetShaderResources(1, 1, &textures[10]->GetTextureSRV());
    gD3DContext->PSSetShaderResources(2, 1, &gShadowMap1SRV); // Shadow map
    gD3DContext->PSSetShaderResources(3, 1, &gShadowMap2SRV); // Shadow map
    gD3DContext->PSSetShaderResources(4, 1, &gShadowMap3SRV); // Shadow map
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(2, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(3, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(4, 1, &gAnisotropic4xSampler);

    // Render model - it will update the model's world matrix and send it to the GPU in a constant buffer, then it will call
    // the Mesh render function, which will set up vertex & index buffer before finally calling Draw on the GPU
    gGround->Render();


    // Select which shaders to use next
    gD3DContext->VSSetShader(gShadowMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gShadowMappingPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[13]->GetTextureSRV());
    gD3DContext->PSSetShaderResources(1, 1, &gShadowMap1SRV); // Shadow map
    gD3DContext->PSSetShaderResources(2, 1, &gShadowMap2SRV); // Shadow map
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(2, 1, &gAnisotropic4xSampler);
    gCharacter->Render();


    gD3DContext->VSSetShader(gCrateShadowMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCrateShadowMappingPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[6]->GetTextureSRV());
    gD3DContext->PSSetShaderResources(1, 1, &gShadowMap3SRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);
    gCrate->Render();


    // Outline drawing - slightly scales object and draws black
    gD3DContext->VSSetShader(gCellShadingOutlineVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCellShadingOutlinePixelShader, nullptr, 0);

    gD3DContext->RSSetState(gCullFrontState);

    gTroll->Render();

    // Main cell shading shaders
    gD3DContext->VSSetShader(gCellShadingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gCellShadingPixelShader, nullptr, 0);

    // Switch back to the usual back face culling (not inside out)
    gD3DContext->RSSetState(gCullBackState);

    // Select the troll texture and sampler
    gD3DContext->PSSetShaderResources(0, 1, &textures[15]->GetTextureSRV()); // First parameter must match texture slot number in the shader
    gD3DContext->PSSetShaderResources(1, 1, &textures[16]->GetTextureSRV()); // First parameter must match texture slot number in the shader
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gPointSampler);

    // Render troll model
    gTroll->Render();



    gD3DContext->VSSetShader(gSpecularMapVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gSpecularMapPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[0]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gCube[0]->Render();


    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[0]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gTeapot->Render();


    gD3DContext->PSSetShaderResources(0, 1, &gCubeMapTextureSRV);
    gCubeMulti->Render();

    gD3DContext->PSSetShader(gTextureTransitionPixelShader, nullptr, 0);

    // Bind textures
    gD3DContext->PSSetShaderResources(0, 1, &textures[8]->GetTextureSRV());
    gD3DContext->PSSetShaderResources(1, 1, &textures[7]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);

    gCube[1]->Render();


    gD3DContext->PSSetShader(gTVPortalPixelShader, nullptr, 0);
    // Bind portal and TV textures
    gD3DContext->PSSetShaderResources(0, 1, &gPortalTextureSRV);
    gD3DContext->PSSetShaderResources(1, 1, &textures[12]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);

    // Now draw the portal model with these shaders and textures
    gPortal->Render();
    

    gD3DContext->VSSetShader(gAdditionalVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gAdditionalPixelShader, nullptr, 0);

    // Render other lit models, only change textures for each onee
    gD3DContext->PSSetShaderResources(0, 1, &textures[14]->GetTextureSRV());
    gBike->Render();


    gD3DContext->VSSetShader(gNormalMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gNormalMappingPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[1]->GetTextureSRV()); // First parameter must match texture slot number in the shared
    gD3DContext->PSSetShaderResources(1, 1, &textures[2]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);

    gCube[2]->Render();


    gD3DContext->VSSetShader(gParallaxMappingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gParallaxMappingPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[3]->GetTextureSRV()); // First parameter must match texture slot number in the shared
    gD3DContext->PSSetShaderResources(1, 1, &textures[4]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gD3DContext->PSSetSamplers(1, 1, &gAnisotropic4xSampler);

    gCube[3]->Render();



    gD3DContext->VSSetShader(gWiggleModelVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gWiggleModelPixelShader, nullptr, 0);

    gD3DContext->PSSetShaderResources(0, 1, &textures[7]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gSphere->Render();
    

    gD3DContext->VSSetShader(gWiggleTextureVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gWiggleTexturePixelShader, nullptr, 0);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
    gD3DContext->PSSetShaderResources(0, 1, &textures[7]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gCube[5]->Render();


    gD3DContext->VSSetShader(gLightModelVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gLightModelPixelShader, nullptr, 0);

    // Set the second portal texture to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gSecondPortalTextureSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);
    gSecondPortal->Render();


    //// Render decal ////
    // No change to shaders, but states are different (additive blending)
    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gPixelLightingPixelShader, nullptr, 0);

    // States - additive blending, read-only depth buffer and no culling (standard set-up for blending
    gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &textures[5]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
    gDecal->Render();

    gCube[6]->Render();
    

    gD3DContext->VSSetShader(gLightModelVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gLightModelPixelShader, nullptr, 0);

    // States - additive blending, read-only depth buffer and no culling (standard set-up for blending
    gD3DContext->OMSetBlendState(gMultiplicativeBlendState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &textures[17]->GetTextureSRV());
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model, sets world matrix, vertex and index buffer and calls Draw on the GPU
    gCube[4]->Render();
        
    //// Render lights ////
    // Rendered with different shaders, textures, states from other models

    // States - additive blending, read-only depth buffer and no culling (standard set-up for blending
    gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &textures[11]->GetTextureSRV()); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render all the lights in the array
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        gPerModelConstants.objectColour = gLights[i].GetColor(); // Set any per-model constants apart from the world matrix just before calling render (light colour here)
        gLights[i].GetModel()->Render();
    }
}




// Rendering the scene now renders everything twice. First it renders the scene for the portal into a texture.
// Then it renders the main scene using the portal texture on a model.
void RenderScene()
{
    //// Common settings for both main scene and portal scene ////

    // Set up the light information in the constant buffer - this is the same for portal and main render
    // Don't send to the GPU yet, the function RenderSceneFromCamera will do that
    gPerFrameConstants.light1Colour = gLights[0].GetColor() * gLights[0].GetStrength();
    gPerFrameConstants.light1Position = gLights[0].GetModel()->Position();

    gPerFrameConstants.light2Colour = gLights[1].GetColor() * gLights[1].GetStrength();
    gPerFrameConstants.light2Position = gLights[1].GetModel()->Position();

    gPerFrameConstants.light3Colour = gLights[2].GetColor() * gLights[2].GetStrength();
    gPerFrameConstants.light3Position = gLights[2].GetModel()->Position();

    gPerFrameConstants.light4Colour = gLights[3].GetColor() * gLights[3].GetStrength();
    gPerFrameConstants.light4Position = gLights[3].GetModel()->Position();
    
    gPerFrameConstants.light5Colour = gLights[4].GetColor() * gLights[4].GetStrength();
    gPerFrameConstants.light5Position = gLights[4].GetModel()->Position();
    gPerFrameConstants.light5Facing = Normalise(gLights[4].GetModel()->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light5CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light5ViewMatrix = CalculateLightViewMatrix(4);         // Calculate camera-like matrices for...
    gPerFrameConstants.light5ProjectionMatrix = CalculateLightProjectionMatrix(4);   //...lights to support shadow mapping

    gPerFrameConstants.light6Colour = gLights[5].GetColor() * gLights[5].GetStrength();
    gPerFrameConstants.light6Position = gLights[5].GetModel()->Position();
    gPerFrameConstants.light6Facing = Normalise(gLights[5].GetModel()->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light6CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light6ViewMatrix = CalculateLightViewMatrix(5);         // Calculate camera-like matrices for...
    gPerFrameConstants.light6ProjectionMatrix = CalculateLightProjectionMatrix(5);   //...lights to support shadow mapping

    gPerFrameConstants.light7Colour = gLights[6].GetColor() * gLights[6].GetStrength();
    gPerFrameConstants.light7Position = gLights[6].GetModel()->Position();

    gPerFrameConstants.light8Colour = gLights[7].GetColor() * gLights[7].GetStrength();
    gPerFrameConstants.light8Position = gLights[7].GetModel()->Position();
    gPerFrameConstants.light8Facing = Normalise(gLights[7].GetModel()->WorldMatrix().GetZAxis());    // Additional lighting information for spotlights
    gPerFrameConstants.light8CosHalfAngle = cos(ToRadians(gSpotlightConeAngle / 2)); // --"--
    gPerFrameConstants.light8ViewMatrix = CalculateLightViewMatrix(7);         // Calculate camera-like matrices for...
    gPerFrameConstants.light8ProjectionMatrix = CalculateLightProjectionMatrix(7);   //...lights to support shadow mapping

    gPerFrameConstants.light9Colour = gLights[8].GetColor() * gLights[8].GetStrength();
    gPerFrameConstants.light9Position = gLights[8].GetModel()->Position();

    gPerFrameConstants.light10Colour = gLights[9].GetColor() * gLights[9].GetStrength();
    gPerFrameConstants.light10Position = gLights[9].GetModel()->Position();

    gPerFrameConstants.ambientColour  = gAmbientColour;
    gPerFrameConstants.specularPower  = gSpecularPower;
    gPerFrameConstants.cameraPosition = gCamera->Position();
    gPerFrameConstants.outlineColour = OutlineColour;
    gPerFrameConstants.outlineThickness = OutlineThickness;
    gPerFrameConstants.parallaxDepth = (gUseParallax ? gParallaxDepth : 0);

    //-------------------------------------------------------------------------

    //// Portal scene rendering ////

    // Set the portal texture and portal depth buffer as the targets for rendering
    // The portal texture will later be used on models in the main scene
    gD3DContext->OMSetRenderTargets(1, &gPortalRenderTarget, gPortalDepthStencilView);

    // Clear the portal texture to a fixed colour and the portal depth buffer to the far distance
    gD3DContext->ClearRenderTargetView(gPortalRenderTarget, &gBackgroundColor.r);
    gD3DContext->ClearDepthStencilView(gPortalDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Render the scene for the portal
    RenderSceneFromCamera(gPortalCamera);

    // Setup the viewport for the portal texture size
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(gPortalWidth);
    vp.Height = static_cast<FLOAT>(gPortalHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Render the scene for the portal
    RenderSceneFromCamera(gPortalCamera);

    // Render to the second portal's texture
    gD3DContext->OMSetRenderTargets(1, &gSecondPortalRenderTarget, gPortalDepthStencilView);

    gD3DContext->ClearRenderTargetView(gSecondPortalRenderTarget, &gBackgroundColor.r);
    gD3DContext->ClearDepthStencilView(gPortalDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Render the scene for the portal
    RenderSceneFromCamera(gPortalCamera);


    vp.Width = static_cast<FLOAT>(gShadowMapSize);
    vp.Height = static_cast<FLOAT>(gShadowMapSize);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Select the shadow map texture as the current depth buffer. We will not be rendering any pixel colours
    // Also clear the the shadow map depth buffer to the far distance
    gD3DContext->OMSetRenderTargets(0, nullptr, gShadowMap1DepthStencil);
    gD3DContext->ClearDepthStencilView(gShadowMap1DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Render the scene from the point of view of light 1 (only depth values written)
    RenderDepthBufferFromLight(4);

    gD3DContext->OMSetRenderTargets(0, nullptr, gShadowMap2DepthStencil);
    gD3DContext->ClearDepthStencilView(gShadowMap2DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    RenderDepthBufferFromLight(5);

    gD3DContext->OMSetRenderTargets(0, nullptr, gShadowMap3DepthStencil);
    gD3DContext->ClearDepthStencilView(gShadowMap3DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    RenderDepthBufferFromLight(7);

    //-------------------------------------------------------------------------

    //// Main scene rendering ////

    // Now set the back buffer as the target for rendering and select the main depth buffer.
    // When finished the back buffer is sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);

    // Clear the back buffer to a fixed colour and the depth buffer to the far distance
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, &gBackgroundColor.r);
    gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Setup the viewport to the size of the main window
    vp.Width = static_cast<FLOAT>(gViewportWidth);
    vp.Height = static_cast<FLOAT>(gViewportHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Render the scene for the main window
    RenderSceneFromCamera(gCamera);

    // Unbind shadow maps from shaders - prevents warnings from DirectX when we try to render to the shadow maps again next frame
    ID3D11ShaderResourceView* nullView = nullptr;
    gD3DContext->PSSetShaderResources(1, 1, &nullView);
    gD3DContext->PSSetShaderResources(2, 1, &nullView);
    gD3DContext->PSSetShaderResources(3, 1, &nullView);


    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    // Set first parameter to 1 to lock to vsync (typically 60fps)
    gSwapChain->Present(lockFPS ? 1 : 0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------
float totalTime = 0.0f;

// Update models and camera. frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
    //// Control model for moving and rotating (this will update its world matrix)
    gCube[0]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCube[1]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCube[2]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCube[3]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCube[4]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCube[5]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCube[6]->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gDecal->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gCrate->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gSphere->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gPortal->Control(frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);
    gSecondPortal->Control(frameTime, Key_T, Key_G, Key_F, Key_H, Key_R, Key_Y, Key_Period, Key_Comma);
    gTeapot->Control(frameTime, Key_T, Key_G, Key_F, Key_H, Key_R, Key_Y, Key_Period, Key_Comma);
    gTroll->Control(frameTime, Key_T, Key_G, Key_F, Key_H, Key_R, Key_Y, Key_Period, Key_Comma);
    gCubeMulti->Control(frameTime, Key_T, Key_G, Key_F, Key_H, Key_R, Key_Y, Key_Period, Key_Comma);
    gBike->Control(0, frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma);

    //// To spin the front wheel (node 1)
    gBike->Control1(1, frameTime, Key_Period, Key_Comma);

    //// To spin the back wheel (node 2)
    gBike->Control2(2, frameTime, Key_Period, Key_Comma);

    // Control camera (will update its view matrix)
    gCamera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D);

    // Orbit the light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	static float rotate = 0.0f;

    // Lights orbits around the models, creating an illuminated area that highlights the model.
    gLights[0].GetModel()->SetPosition(gTeapot->Position() + CVector3{ cos(rotate) * gLightOrbit, 10.0f, sin(rotate) * gLightOrbit });
    gLights[2].GetModel()->SetPosition(gCube[2]->Position() + CVector3{cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit});
    gLights[3].GetModel()->SetPosition(gCube[3]->Position() + CVector3{cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit});
    gLights[6].GetModel()->SetPosition(gBike->Position() + CVector3{ cos(rotate) * gLightOrbit * 2.0f, 15.0f, sin(rotate) * gLightOrbit * 2.0f });
    gLights[8].GetModel()->SetPosition(gCube[0]->Position() + CVector3{ cos(rotate) * gLightOrbit, 0.0f, sin(rotate) * gLightOrbit });
    gLights[9].GetModel()->SetPosition(gTroll->Position() + CVector3{ cos(rotate) * gLightOrbit, 10.0f, sin(rotate) * gLightOrbit });

    // Lights orbits around the character model at a fixed elevation and distance, and always faces the character.
    gLights[4].GetModel()->SetPosition(gCharacter->Position() + CVector3{ cos(rotate) * gLightOrbit, 10, sin(rotate) * gLightOrbit });
    gLights[4].GetModel()->FaceTarget(gCharacter->Position());

    gLights[5].GetModel()->FaceTarget(gCharacter->Position());

    gLights[7].GetModel()->SetPosition(gCrate->Position() + CVector3{ cos(rotate) * gLightOrbit * 2.0f, 35.0f, sin(rotate) * gLightOrbit * 2.0f });
    gLights[7].GetModel()->FaceTarget(gCrate->Position());

    rotate -= gLightOrbitSpeed * frameTime;

    totalTime += frameTime; // Add the frame time to the total time

    gLights[0].SetStrength(5.0f + sinf(totalTime * 2.0f) * 5.0f);  // Oscillate the light strength between 0 and 10

    // Set the scale of the model representing the light source
    gLights[0].GetModel()->SetScale(pow(gLights[0].GetStrength(), 0.7f));

    // Gradually change the color of light 2 using totalTime
    float colorChange = (sinf(totalTime) + 1.0f) / 2.0f; // Oscillates between 0 and 1
    gLights[1].SetColor(CVector3(colorChange, 0.2f, 1.0f - colorChange));

    gLights[7].SetColor(CVector3(colorChange, 0.2f, 1.0f - colorChange));
    gLights[7].SetStrength(20.0f + sinf(totalTime * 2.0f) * 20.0f);  // Oscillate the light strength between 0 and 40
    gLights[7].GetModel()->SetScale(pow(gLights[7].GetStrength(), 0.7f)); // Set the scale of the model representing the light source

    // Increases the 'wiggle' variable each frame, scaled by the frame time to ensure smooth animation across different frame rates.
    // This variable is used to produce time-based effects such as oscillations or pulsations in shaders.
    gPerFrameConstants.wiggle += 3.0f * frameTime;

    // Uses the sine function to create a smoothly oscillating factor between 0 and 1 based on the 'wiggle' variable. 
    // This transition factor can be used to interpolate between different states or values in the shaders, 
    // such as blending textures or changing colors over time.
    gPerFrameConstants.transitionFactor = (sinf(gPerFrameConstants.wiggle) + 1.0f) * 0.5f;

    // Rotates the second portal model around the Y-axis, with the rotation speed proportional to the elapsed time ('totalTime'). 
    // This creates a continuous spinning effect, making the portal look more dynamic and engaging.
    gSecondPortal->SetRotation({ 0.0f, totalTime, 0.0f });

    float minBrightness = 0.5f; // A sensible minimum brightness to prevent darkness

    // Calculate color variations with a minimum brightness offset and ensure they stay within [0, 1] range
    gPerFrameConstants.colorVariation[0] = minBrightness + (sin(totalTime * 1.2f) + 1.0f) * 0.5f; // R
    gPerFrameConstants.colorVariation[1] = minBrightness + (cos(totalTime * 1.5f) + 1.0f) * 0.5f; // G
    gPerFrameConstants.colorVariation[2] = minBrightness + (sin(totalTime * 0.7f) + 1.0f) * 0.5f; // B

    // Update the constant buffer with the new color variation
    gD3DContext->UpdateSubresource(gPerFrameConstantBuffer, 0, nullptr, &gPerFrameConstants, 0, 0);

    // Toggle FPS limiting
    if (KeyHit(Key_P))  lockFPS = !lockFPS;

    // Toggle parallax
    if (KeyHit(Key_1))
    {
        gUseParallax = !gUseParallax;
    }

    // Show frame time / FPS in the window title //
    const float fpsUpdateTime = 0.5f; // How long between updates (in seconds)
    static float totalFrameTime = 0;
    static int frameCount = 0;
    totalFrameTime += frameTime;
    ++frameCount;
    if (totalFrameTime > fpsUpdateTime)
    {
        // Displays FPS rounded to nearest int, and frame time (more useful for developers) in milliseconds to 2 decimal places
        float avgFrameTime = totalFrameTime / frameCount;
        std::ostringstream frameTimeMs;
        frameTimeMs.precision(2);
        frameTimeMs << std::fixed << avgFrameTime * 1000;
        std::string windowTitle = "CO2409 Week 18: Render to Texture - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
