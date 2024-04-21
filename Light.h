#pragma once
#include "CVector3.h" // Include the header for vector operations, assuming you have one
#include "Model.h"    // Model class for visual representation

class Light {
public:
    // Constructor with default parameters for position, color, and light strength
    Light(const CVector3& position = { 0.0f, 0.0f, 0.0f }, const CVector3& color = { 1.0f, 1.0f, 1.0f }, float strength = 1.0f);

    // Setter for the light's position
    void SetPosition(const CVector3& position);
    // Getter for the light's position
    CVector3 GetPosition();

    // Setter for the light's color
    void SetColor(const CVector3& color);
    // Getter for the light's color
    CVector3 GetColor();

    // Setter for the light's strength
    void SetStrength(float strength);
    // Getter for the light's strength
    float GetStrength();

    // Setter for associating a model with this light
    void SetModel(Model* model);
    // Getter for the model associated with this light
    Model* GetModel();

private:
    // Private member variables for position, color, strength, and the associated model
    CVector3 mPosition;
    CVector3 mColor;
    float mStrength;
    Model* mModel; // Pointer to the model representing this light visually in the scene
};
