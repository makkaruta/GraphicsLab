#pragma once

#include "DisplayWin32.h"
#include "GameComponent.h"
#include "TriangleComponent.h"
#include "windows.h"
#include <vector>
#include <wrl.h>
#include <chrono>
/*#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")*/

#define ZCHECK(exp) if(FAILED(exp)) { printf("Check failed at file: %s at line %i", __FILE__, __LINE__); return 0; }

class Game {
private:
	ID3D11Debug* debug; // ��������� ������� ��������� ����������� ������� � ��������� ��������� ���������
	std::chrono::time_point<std::chrono::steady_clock> PrevTime;
	float totalTime = 0;
	unsigned int frameCount = 0;
public:
	D3D11_VIEWPORT viewport; // ������� ������� ���������

	std::vector <GameComponent*> Components; // ������ ��������� (�������)
	ID3D11DeviceContext* context; // ���������, ���������� �������� �� ��������� ��������� ����������, ����� ��� ����� ��� �������
	// ��� ������ ��������� ����������� ����� ������ ��������� ����������, ������� ������������� ����������.
	IDXGISwapChain* swapChain; // �������� (������� ��������)
	ID3D11RenderTargetView* rtv; // ������� ������ ����������
	ID3DUserDefinedAnnotation* annotation; // �������� �������������� �������� � �������� � ������ ���� ����������
	// ��������� ���������� � ������������ ������ ��������� ����� ��� ��������, ����� ����� ������� Direct3D ������������ ������ ��������� ���� ����������
	DisplayWin32 Display;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	int Run();
	void Initialize();
	void DestroyResources();
	void Draw();
};