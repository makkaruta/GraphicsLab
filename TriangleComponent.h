#pragma once

#include "GameComponent.h"

#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

using namespace DirectX;

class TriangleComponent : public GameComponent {
public:
	DirectX::XMFLOAT4* positions;
	DirectX::XMFLOAT4* colors;
	int* indeces;
	TriangleComponent();
	TriangleComponent(DirectX::XMFLOAT4* positionsArr, DirectX::XMFLOAT4* colorsArr, int* indecesArr);
	void Initialize();
	void Update();
	void Draw();
	void DestroyResources();
};