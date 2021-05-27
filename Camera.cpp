#include "Pch.h"
#include "Camera.h"
#define PI 3,1415926535

void Camera::Initialize(int screenWidth, int screenHeight) {
	ViewMatrix = DirectX::SimpleMath::Matrix::Identity;
	ProjectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		(float)(PI / 2.0f),
		(float)(screenWidth / screenHeight),
		0.1f,
		10000.0f);
}
