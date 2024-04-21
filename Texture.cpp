#include "Texture.h"
#include "GraphicsHelpers.h"

Texture::Texture(const std::string& filename) 
{
    // Attempt to load the texture. Throws an error if loading fails.
    if (!LoadTexture(filename, &mTexture, &mTextureSRV)) {
        throw std::runtime_error("Failed to load texture: " + filename);
    }

    // Clean up the temporary resource holder if it's not needed anymore
    if (mTexture) {
        mTexture->Release();
        mTexture = nullptr;
    }
}

Texture::~Texture() 
{
    // Release Direct3D resources
    if (mTexture) mTexture->Release();
    mTexture = nullptr;

    if (mTextureSRV) mTextureSRV->Release();
    mTextureSRV = nullptr;
}

ID3D11Resource*& Texture::GetTexture() 
{
    return mTexture; // Returns a reference to the texture resource
}

ID3D11ShaderResourceView*& Texture::GetTextureSRV()
{
    return mTextureSRV; // Returns a reference to the shader resource view
}
