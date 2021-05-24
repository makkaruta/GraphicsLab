#pragma once

#include "pch.h"

#include "DisplayWin32.h"

using namespace DirectX;

class GameComponent {
public:
	virtual int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) { return 0; };
	virtual void DestroyResourses() {};
	virtual void Update() {};
	virtual void Draw(ID3D11DeviceContext* context) {};
};
