//--------------------------------------------------------------------------------------
// Class encapsulating a model
//--------------------------------------------------------------------------------------
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.

#include "Model.h"

#include "Common.h"
#include "GraphicsHelpers.h"
#include "Mesh.h"

void Model::Render()
{
	UpdateWorldMatrix();

	gPerModelConstants.worldMatrix = mWorldMatrix; // Update C++ side constant buffer
	UpdateConstantBuffer(gPerModelConstantBuffer, gPerModelConstants); // Send to GPU

	// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
	gD3DContext->VSSetConstantBuffers(1, 1, &gPerModelConstantBuffer); // First parameter must match constant buffer number in the shader
	gD3DContext->PSSetConstantBuffers(1, 1, &gPerModelConstantBuffer);

	mMesh->Render();
}



// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
void Model::Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
	KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward)
{
	UpdateWorldMatrix();

	if (KeyHeld(turnDown))
	{
		mRotation.x += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnUp))
	{
		mRotation.x -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnRight))
	{
		mRotation.y += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnLeft))
	{
		mRotation.y -= ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnCW))
	{
		mRotation.z += ROTATION_SPEED * frameTime;
	}
	if (KeyHeld(turnCCW))
	{
		mRotation.z -= ROTATION_SPEED * frameTime;
	}

	// Local Z movement - move in the direction of the Z axis, get axis from world matrix
	CVector3 localZDir = Normalise({ mWorldMatrix.e20, mWorldMatrix.e21, mWorldMatrix.e22 }); // normalise axis in case world matrix has scaling
	if (KeyHeld(moveForward))
	{
		mPosition.x += localZDir.x * MOVEMENT_SPEED * frameTime;
		mPosition.y += localZDir.y * MOVEMENT_SPEED * frameTime;
		mPosition.z += localZDir.z * MOVEMENT_SPEED * frameTime;
	}
	if (KeyHeld(moveBackward))
	{
		mPosition.x -= localZDir.x * MOVEMENT_SPEED * frameTime;
		mPosition.y -= localZDir.y * MOVEMENT_SPEED * frameTime;
		mPosition.z -= localZDir.z * MOVEMENT_SPEED * frameTime;
	}
}


void Model::UpdateWorldMatrix()
{
	mWorldMatrix = MatrixScaling(mScale) * MatrixRotationZ(mRotation.z) * MatrixRotationX(mRotation.x) * MatrixRotationY(mRotation.y) * MatrixTranslation(mPosition);
}