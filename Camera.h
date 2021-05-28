#pragma once
#include "pch.h"

#include "InputDevice.h"

class Camera {
private:
	InputDevice* inputDevice;
public:
	DirectX::SimpleMath::Matrix ViewMatrix;
	DirectX::SimpleMath::Matrix ProjectionMatrix;

	float Yaw;
	float Pitch;
	DirectX::SimpleMath::Vector3 position;
	float VelocityMagnitude = 5.0f; 
	float MouseSensetivity = 1.0f;

	void Initialize(int screenWidth, int screenHeight, InputDevice* inpDev);
	void Update(float deltaTime, int screenWidth, int screenHeight);
	void OnMouseMove(MouseMoveEventArgs args);
};