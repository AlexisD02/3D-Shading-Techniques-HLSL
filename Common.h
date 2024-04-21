//--------------------------------------------------------------------------------------
// Commonly used definitions across entire project
//--------------------------------------------------------------------------------------
#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_

#define NOMINMAX // windows.h is very old and for compatibilty reasons includes a very badly written
                 // definition of min and max. It clashes with the STL std::min and std::max.
                 // Use this line before windows.h to remove the problematic legacy definitions
#include <windows.h>
#include <d3d11.h>
#include <string>

#include "CVector3.h"
#include "CMatrix4x4.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
// Make global Variables from various files available to other files. "extern" means
// this variable is defined in another file somewhere. We should use classes and avoid
// use of globals, but done this way to keep code simpler so the DirectX content is
// clearer. However, try to architect your own code in a better way.

// Windows variables
extern HWND gHWnd;

// Viewport size
extern int gViewportWidth;
extern int gViewportHeight;


// Important DirectX variables
extern ID3D11Device*           gD3DDevice;
extern ID3D11DeviceContext*    gD3DContext;
extern IDXGISwapChain*         gSwapChain;
extern ID3D11RenderTargetView* gBackBufferRenderTarget;  // Back buffer is where we render to
extern ID3D11DepthStencilView* gDepthStencil;            // The depth buffer contains a depth for each back buffer pixel
extern ID3D11ShaderResourceView* gTextureA_SRV;
extern ID3D11ShaderResourceView* gTextureB_SRV;

// Input constsnts
extern const float ROTATION_SPEED;
extern const float MOVEMENT_SPEED;


// A global error message to help track down fatal errors - set it to a useful message
// when a serious error occurs
extern std::string gLastError;



//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
// Variables sent over to the GPU each frame

// Data that remains constant for an entire frame, updated from C++ to the GPU shaders *once per frame*
// We hold them together in a structure and send the whole thing to a "constant buffer" on the GPU each frame when
// we have finished updating the scene. There is a structure in the shader code that exactly matches this one
struct PerFrameConstants
{
    // These are the matrices used to position the camera
    CMatrix4x4 viewMatrix;
    CMatrix4x4 projectionMatrix;
    CMatrix4x4 viewProjectionMatrix; // The above two matrices multiplied together to combine their effects

    CVector3   light1Position; // 3 floats: x, y z
    float      padding1;      // IMPORTANT technical point: shaders work with float4 values. If constant buffer variables don't align
                              // to the size of a float4 then HLSL (GPU) will insert padding, which can cause problems matching 
                              // structure between C++ and GPU. So add these unused padding variables to both HLSL and C++ structures.
    CVector3   light1Colour;
    float      padding2;

    CVector3   light2Position;
    float      padding3;

    CVector3   light2Colour;
    float      padding4;

    CVector3   light3Position;
    float      padding5;

    CVector3   light3Colour;
    float      padding6;

    CVector3   light4Position;
    float      padding7;

    CVector3   light4Colour;
    float      padding8;

    CVector3   light5Position; // 3 floats: x, y z
    float      padding9;       // Pad above variable to float4 (HLSL requirement - which we must duplicate in this the C++ version of the structure)

    CVector3   light5Colour;
    float      padding10;

    CVector3   light5Facing;           // Spotlight facing direction (normal)
    float      light5CosHalfAngle;     // cos(Spot light cone angle / 2). Precalculate in C++ the spotlight angle in this form to save doing in the shader
    CMatrix4x4 light5ViewMatrix;       // For shadow mapping we treat lights like cameras so we need camera matrices for them (prepared on the C++ side)
    CMatrix4x4 light5ProjectionMatrix; // --"--

    CVector3   light6Position;
    float      padding11;

    CVector3   light6Colour;
    float      padding12;

    CVector3   light6Facing;
    float      light6CosHalfAngle;
    CMatrix4x4 light6ViewMatrix;
    CMatrix4x4 light6ProjectionMatrix;

    CVector3   light7Position;
    float      padding13;

    CVector3   light7Colour;
    float      padding14;

    CVector3   light8Position;
    float      padding15;

    CVector3   light8Colour;
    float      padding16;

    CVector3   light8Facing;
    float      light8CosHalfAngle;
    CMatrix4x4 light8ViewMatrix;
    CMatrix4x4 light8ProjectionMatrix;

    CVector3   light9Position;
    float      padding17;

    CVector3   light9Colour;
    float      padding18;

    CVector3   light10Position;
    float      padding19;

    CVector3   light10Colour;
    float      padding20;

    CVector3   ambientColour;
    float      specularPower;  // In this case we actually have a useful float variable that we can use to pad to a float4

    CVector3   cameraPosition;
    float      padding21;

    CVector3   outlineColour;    // Cell shading outline colour
    float      padding23;

    float      outlineThickness; // Controls thickness of outlines for cell shading
    float      padding24;

    float      wiggle;
    float      padding25;

    float      transitionFactor;
    float      padding26;

    float      parallaxDepth;  // Depth of the parallax mapping effect
    float      padding27;

    float      colorVariation[3]; // B color variation
    float      padding28;
};

extern PerFrameConstants gPerFrameConstants;      // This variable holds the CPU-side constant buffer described above
extern ID3D11Buffer*     gPerFrameConstantBuffer; // This variable controls the GPU-side constant buffer matching to the above structure



// This is the matrix that positions the next thing to be rendered in the scene. Unlike the structure above this data can be
// updated and sent to the GPU several times every frame (once per model). However, apart from that it works in the same way.
struct PerModelConstants
{
    CMatrix4x4 worldMatrix;
    CVector3   objectColour; // Allows each light model to be tinted to match the light colour they cast
    float      padding22;
};
extern PerModelConstants gPerModelConstants;      // This variable holds the CPU-side constant buffer described above
extern ID3D11Buffer*     gPerModelConstantBuffer; // This variable controls the GPU-side constant buffer related to the above structure

#endif //_COMMON_H_INCLUDED_
