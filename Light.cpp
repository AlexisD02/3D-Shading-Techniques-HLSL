#include "Light.h"

// Constructor initializing the light with given position, color, and strength
Light::Light(const CVector3& position, const CVector3& color, float strength) : mPosition(position), mColor(color), mStrength(strength), mModel(nullptr) { }

// Returns the current position of the light
CVector3 Light::GetPosition()
{
    return mPosition;
}

// Sets a new position for the light and updates the model's position if one is associated
void Light::SetPosition(const CVector3& position)
{
    mPosition = position;
    if (mModel) mModel->SetPosition(mPosition); // Synchronize light's position with its visual representation
}

// Returns the current color of the light
CVector3 Light::GetColor()
{
    return mColor;
}

// Sets a new color for the light
void Light::SetColor(const CVector3& color)
{
    mColor = color;
}

// Returns the current strength of the light
float Light::GetStrength()
{
    return mStrength;
}

// Sets a new strength value for the light
void Light::SetStrength(float strength)
{
    mStrength = strength;
}

// Associates a model with the light for visual representation in the scene
void Light::SetModel(Model* model)
{
    mModel = model;
    if (mModel) mModel->SetPosition(mPosition); // Automatically updates the model's position to match the light
}

// Returns the model associated with the light, if any
Model* Light::GetModel()
{
    return mModel;
}
