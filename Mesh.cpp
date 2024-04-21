//--------------------------------------------------------------------------------------
// Class encapsulating a mesh
//--------------------------------------------------------------------------------------
// The mesh class splits the mesh into sub-meshes that only use one texture each.
// ** THIS VERSION WILL ONLY KEEP THE FIRST SUB-MESH OTHER PARTS WILL BE MISSING **
// ** DO NOT USE THIS VERSION FOR PROJECTS **
// The class also doesn't load textures, filters or shaders as the outer code is
// expected to select these things. A later lab will introduce a more robust loader.

#include "Mesh.h"
#include "Shader.h" // Needed for helper function CreateSignatureForVertexLayout
#include "CVector2.h" 
#include "CVector3.h" 
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <memory>


// Pass the name of the mesh file to load. Uses assimp (http://www.assimp.org/) to support many file types
// Optionally request tangents to be calculated (for normal and parallax mapping - see later lab)
// Will throw a std::runtime_error exception on failure (since constructors can't return errors).
Mesh::Mesh(const std::string& fileName, bool requireTangents /*= false*/)
{
    Assimp::Importer importer;

    // Flags for processing the mesh. Assimp provides a huge amount of control - right click any of these
    // and "Peek Definition" to see documention above each constant
    unsigned int assimpFlags = aiProcess_MakeLeftHanded |
        aiProcess_GenSmoothNormals |
        aiProcess_FixInfacingNormals |
        aiProcess_GenUVCoords |
        aiProcess_TransformUVCoords |
        aiProcess_FlipUVs |
        aiProcess_FlipWindingOrder |
        aiProcess_Triangulate |
        aiProcess_PreTransformVertices |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType |
        aiProcess_FindInvalidData |
        aiProcess_OptimizeMeshes |
        aiProcess_FindInstances |
        aiProcess_FindDegenerates |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_Debone |
        aiProcess_RemoveComponent;

    // Flags to specify what mesh data to ignore
    int removeComponents = aiComponent_LIGHTS | aiComponent_CAMERAS | aiComponent_TEXTURES | aiComponent_COLORS |
        aiComponent_BONEWEIGHTS | aiComponent_ANIMATIONS | aiComponent_MATERIALS;

    // Add / remove tangents as required by user
    if (requireTangents)
    {
        assimpFlags |= aiProcess_CalcTangentSpace;
    }
    else
    {
        removeComponents |= aiComponent_TANGENTS_AND_BITANGENTS;
    }

    // Other miscellaneous settings
    importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f); // Smoothing angle for normals
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);  // Remove points and lines (keep triangles only)
    importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);                 // Remove degenerate triangles
    importer.SetPropertyBool(AI_CONFIG_PP_DB_ALL_OR_NONE, true);            // Default to removing bones/weights from meshes that don't need skinning

    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, removeComponents);

    // Import mesh with assimp given above requirements - log output
    Assimp::DefaultLogger::create("", Assimp::DefaultLogger::VERBOSE);
    const aiScene* scene = importer.ReadFile(fileName, assimpFlags);
    Assimp::DefaultLogger::kill();
    if (scene == nullptr)  throw std::runtime_error("Error loading mesh (" + fileName + "). " + importer.GetErrorString());
    if (scene->mNumMeshes == 0)  throw std::runtime_error("No usable geometry in mesh: " + fileName);


    //-----------------------------------

    // Only importing first submesh - significant limitation - do not use this importer for your own projects
    aiMesh* assimpMesh = scene->mMeshes[0];
    std::string subMeshName = assimpMesh->mName.C_Str();


    //-----------------------------------

    // Check for presence of position and normal data. Tangents and UVs are optional.
    std::vector<D3D11_INPUT_ELEMENT_DESC> vertexElements;
    unsigned int offset = 0;

    if (!assimpMesh->HasPositions())  throw std::runtime_error("No position data for sub-mesh " + subMeshName + " in " + fileName);
    unsigned int positionOffset = offset;
    vertexElements.push_back({ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, positionOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    offset += 12;

    if (!assimpMesh->HasNormals())  throw std::runtime_error("No normal data for sub-mesh " + subMeshName + " in " + fileName);
    unsigned int normalOffset = offset;
    vertexElements.push_back({ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, normalOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    offset += 12;

    unsigned int tangentOffset = offset;
    if (requireTangents)
    {
        if (!assimpMesh->HasTangentsAndBitangents())  throw std::runtime_error("No tangent data for sub-mesh " + subMeshName + " in " + fileName);
        vertexElements.push_back({ "Tangent", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, tangentOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        offset += 12;
    }

    unsigned int uvOffset = offset;
    if (assimpMesh->GetNumUVChannels() > 0 && assimpMesh->HasTextureCoords(0))
    {
        if (assimpMesh->mNumUVComponents[0] != 2)  throw std::runtime_error("Unsupported texture coordinates in " + subMeshName + " in " + fileName);
        vertexElements.push_back({ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uvOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        offset += 8;
    }

    mVertexSize = offset;


    // Create a "vertex layout" to describe to DirectX what is data in each vertex of this mesh
    auto shaderSignature = CreateSignatureForVertexLayout(vertexElements.data(), static_cast<int>(vertexElements.size()));
    HRESULT hr = gD3DDevice->CreateInputLayout(vertexElements.data(), static_cast<UINT>(vertexElements.size()),
        shaderSignature->GetBufferPointer(), shaderSignature->GetBufferSize(),
        &mVertexLayout);
    if (shaderSignature)  shaderSignature->Release();
    if (FAILED(hr))  throw std::runtime_error("Failure creating input layout for " + fileName);



    //-----------------------------------

    // Create CPU-side buffers to hold current mesh data - exact content is flexible so can't use a structure for a vertex - so just a block of bytes
    // Note: for large arrays a unique_ptr is better than a vector because vectors default-initialise all the values which is a waste of time.
    mNumVertices = assimpMesh->mNumVertices;
    mNumIndices = assimpMesh->mNumFaces * 3;
    auto vertices = std::make_unique<unsigned char[]>(mNumVertices * mVertexSize);
    auto indices = std::make_unique<unsigned char[]>(mNumIndices * 4); // Using 32 bit indexes (4 bytes) for each indeex


    //-----------------------------------

    // Copy mesh data from assimp to our CPU-side vertex buffer

    CVector3* assimpPosition = reinterpret_cast<CVector3*>(assimpMesh->mVertices);
    unsigned char* position = vertices.get() + positionOffset;
    unsigned char* positionEnd = position + mNumVertices * mVertexSize;
    while (position != positionEnd)
    {
        *(CVector3*)position = *assimpPosition;
        position += mVertexSize;
        ++assimpPosition;
    }

    CVector3* assimpNormal = reinterpret_cast<CVector3*>(assimpMesh->mNormals);
    unsigned char* normal = vertices.get() + normalOffset;
    unsigned char* normalEnd = normal + mNumVertices * mVertexSize;
    while (normal != normalEnd)
    {
        *(CVector3*)normal = *assimpNormal;
        normal += mVertexSize;
        ++assimpNormal;
    }

    if (requireTangents)
    {
        CVector3* assimpTangent = reinterpret_cast<CVector3*>(assimpMesh->mTangents);
        unsigned char* tangent = vertices.get() + tangentOffset;
        unsigned char* tangentEnd = tangent + mNumVertices * mVertexSize;
        while (tangent != tangentEnd)
        {
            *(CVector3*)tangent = *assimpTangent;
            tangent += mVertexSize;
            ++assimpTangent;
        }
    }

    if (assimpMesh->GetNumUVChannels() > 0 && assimpMesh->HasTextureCoords(0))
    {
        aiVector3D* assimpUV = assimpMesh->mTextureCoords[0];
        unsigned char* uv = vertices.get() + uvOffset;
        unsigned char* uvEnd = uv + mNumVertices * mVertexSize;
        while (uv != uvEnd)
        {
            *(CVector2*)uv = CVector2(assimpUV->x, assimpUV->y);
            uv += mVertexSize;
            ++assimpUV;
        }
    }


    //-----------------------------------

    // Copy face data from assimp to our CPU-side index buffer
    if (!assimpMesh->HasFaces())  throw std::runtime_error("No face data in " + subMeshName + " in " + fileName);

    DWORD* index = reinterpret_cast<DWORD*>(indices.get());
    for (unsigned int face = 0; face < assimpMesh->mNumFaces; ++face)
    {
        *index++ = assimpMesh->mFaces[face].mIndices[0];
        *index++ = assimpMesh->mFaces[face].mIndices[1];
        *index++ = assimpMesh->mFaces[face].mIndices[2];
    }


    //-----------------------------------

    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA initData;

    // Create GPU-side vertex buffer and copy the vertices imported by assimp into it
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Indicate it is a vertex buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;          // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = mNumVertices * mVertexSize; // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    initData.pSysMem = vertices.get(); // Fill the new vertex buffer with data loaded by assimp

    hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &mVertexBuffer);
    if (FAILED(hr))  throw std::runtime_error("Failure creating vertex buffer for " + fileName);


    // Create GPU-side index buffer and copy the vertices imported by assimp into it
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // Indicate it is an index buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;         // Default usage for this buffer - we'll see other usages later
    bufferDesc.ByteWidth = mNumIndices * sizeof(DWORD); // Size of the buffer in bytes
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    initData.pSysMem = indices.get(); // Fill the new index buffer with data loaded by assimp

    hr = gD3DDevice->CreateBuffer(&bufferDesc, &initData, &mIndexBuffer);
    if (FAILED(hr))  throw std::runtime_error("Failure creating index buffer for " + fileName);
}


Mesh::~Mesh()
{
    if (mIndexBuffer)   mIndexBuffer->Release();
    if (mVertexBuffer)  mVertexBuffer->Release();
    if (mVertexLayout)  mVertexLayout->Release();
}


// The render function assumes shaders, matrices, textures, samplers etc. have been set up already.
// It simply draws this mesh with whatever settings the GPU is currently using.
void Mesh::Render()
{
    // Set vertex buffer as next data source for GPU
    UINT stride = mVertexSize;
    UINT offset = 0;
    gD3DContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

    // Indicate the layout of vertex buffer
    gD3DContext->IASetInputLayout(mVertexLayout);

    // Set index buffer as next data source for GPU, indicate it uses 32-bit integers
    gD3DContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Using triangle lists only in this class
    gD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Render mesh
    gD3DContext->DrawIndexed(mNumIndices, 0, 0);
}
