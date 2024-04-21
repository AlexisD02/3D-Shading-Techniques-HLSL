#pragma once

#include <d3d11.h>
#include <string>
#include <stdexcept> // Included for exception handling

// Use the standard namespace to avoid prefixing STD::
using namespace std;

class Texture {
public:
    // Direct3D texture and its shader resource view
    ID3D11Resource* mTexture = nullptr;
    ID3D11ShaderResourceView* mTextureSRV = nullptr;

    // Constructor to load a texture from file
    Texture(const std::string& filename);

    // Destructor to clean up resources
    ~Texture();

    // Accessors for texture and its shader resource view
    ID3D11Resource*& GetTexture();
    ID3D11ShaderResourceView*& GetTextureSRV();
};
