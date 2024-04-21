//--------------------------------------------------------------------------------------
// Class encapsulating a model
//--------------------------------------------------------------------------------------
// Holds a pointer to a mesh as well as position, rotation and scaling, which are converted to a world matrix when required
// This is more of a convenience class, the Mesh class does most of the difficult work.

#include "Common.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "Input.h"

#ifndef _MODEL_H_INCLUDED_
#define _MODEL_H_INCLUDED_

class Mesh;

class Model
{
public:
	//-------------------------------------
	// Construction / Usage
	//-------------------------------------

	Model(Mesh* mesh, CVector3 position = { 0,0,0 }, CVector3 rotation = { 0,0,0 }, float scale = 1)
		: mMesh(mesh), mPosition(position), mRotation(rotation), mScale({ scale, scale, scale })
	{
	}

	// The render function sets the world matrix in the per-frame constant buffer and makes that buffer available
	// to vertex & pixel shader. Then it calls Mesh:Render, which renders the geometry with current GPU settings.
	// So all other per-frame constants must have been set already along with shaders, textures, samplers, states etc.
	void Render();


	// Control the model's position and rotation using keys provided. Amount of motion performed depends on frame time
	void Control(float frameTime, KeyCode turnUp, KeyCode turnDown, KeyCode turnLeft, KeyCode turnRight,
		KeyCode turnCW, KeyCode turnCCW, KeyCode moveForward, KeyCode moveBackward);


	void FaceTarget(CVector3 target)
	{
		UpdateWorldMatrix();
		mWorldMatrix.FaceTarget(target);
		mRotation = mWorldMatrix.GetEulerAngles();
	}

	//-------------------------------------
	// Data access
	//-------------------------------------

	// Getters / setters
	CVector3 Position() { return mPosition; }
	CVector3 Rotation() { return mRotation; }
	CVector3 Scale() { return mScale; }

	void SetPosition(CVector3 position) { mPosition = position; }
	void SetRotation(CVector3 rotation) { mRotation = rotation; }

	// Two ways to set scale: x,y,z separately, or all to the same value
	void SetScale(CVector3 scale) { mScale = scale; }
	void SetScale(float scale) { mScale = { scale, scale, scale }; }

	// Read only access to model world matrix, updated on request
	CMatrix4x4 WorldMatrix() { UpdateWorldMatrix();  return mWorldMatrix; }


	//-------------------------------------
	// Private data / members
	//-------------------------------------
private:
	void UpdateWorldMatrix();

	Mesh* mMesh;

	// Position, rotation and scaling for the model
	CVector3 mPosition;
	CVector3 mRotation;
	CVector3 mScale;

	// World matrix for the model - built from the above
	CMatrix4x4 mWorldMatrix;
};


#endif //_MODEL_H_INCLUDED_
