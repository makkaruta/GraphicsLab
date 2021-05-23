#pragma once

#include "DisplayWin32.h"
#include "windows.h"
#include <vector>
#include <wrl.h>
#include <chrono>
#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace DirectX;

class GameComponent {
public:
	virtual int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) { return 0; };
	virtual void DestroyResourses() {};
	virtual void Update() {};
	virtual void Draw(ID3D11DeviceContext* context) {};
};
