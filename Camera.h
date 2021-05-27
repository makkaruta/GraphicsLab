#pragma once
#include "pch.h"

class Camera {
private:
public:
	DirectX::SimpleMath::Matrix ViewMatrix;
	DirectX::SimpleMath::Matrix ProjectionMatrix;
	void Initialize(int screenWidth, int screenHeight);
};