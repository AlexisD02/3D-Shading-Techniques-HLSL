//--------------------------------------------------------------------------------------
// Depth-Only Pixel Shader
//--------------------------------------------------------------------------------------
// Pixel shader that only outputs the depth of a pixel - used to render the scene from the point
// of view of the lights in preparation for shadow mapping

#include "Common.hlsli" // Shaders can also use include files - note the extension


//--------------------------------------------------------------------------------------
// Shader code
//--------------------------------------------------------------------------------------

// Shader used when rendering the shadow map depths. The pixel shader doesn't really need to calculate a pixel colour
// since we are only writing to the depth buffer. However, one of the exercises displays the content on the screen.
float4 main(DepthOnlyInput input) : SV_Target
{
	// Output the value that would go in the depth buffer to the pixel colour (greyscale)
    // The pow just rescales the values so they are easier to visualise. Although depth values range from 0 to 1 most
    // values are close to 1, which might give the (mistaken) impression that the depth buffer is empty
    return pow(input.depth, 20);
}

// NOTE: The correct way to write a pixel shader that doesn't output a colour is this:
// You can use this line when not interested in visualising the depth buffer.
//     void main(SimplePixelShaderInput input) {}
// If the depth buffer is enabled, this will output depth but no colour (C++ side render target should be nullptr)
