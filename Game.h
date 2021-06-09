#pragma once

#include "pch.h"

#include "DisplayWin32.h"
#include "GameComponent.h"
#include "TriangleComponent.h"
#include "LineComponent.h"

#define ERROR_DEV_SWAPCHAIN 101
#define ERROR_SWAPCHAIN_BUF 102
#define ERROR_RENDER_TARGER 103
#define ERROR_DEPTH_BUF 104
#define ERROR_DEPTH_STENSIL 105

class Game {
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	DisplayWin32 Display;
	D3D11_VIEWPORT viewport; // ������� ������� ���������
	std::vector <GameComponent*> Components; // ������ ��������� (�������)
	ID3D11DeviceContext* context; // ���������, ���������� �������� �� ��������� ��������� ����������, ����� ��� ����� ��� �������
	// ��� ������ ��������� ����������� ����� ������ ��������� ����������, ������� ������������� ����������.
	IDXGISwapChain* swapChain; // �������� (������� ��������)
	ID3D11RenderTargetView* rtv; // ������� ������ ����������
	ID3D11Texture2D* depthBuffer;
	ID3D11DepthStencilView* depthView;
	ID3DUserDefinedAnnotation* annotation; // �������� �������������� �������� � �������� � ������ ���� ����������
	// ��������� ���������� � ������������ ������ ��������� ����� ��� ��������, ����� ����� ������� Direct3D ������������ ������ ��������� ���� ����������
	ID3D11Debug* debug; // ��������� ������� ��������� ����������� ������� � ��������� ��������� ���������
	InputDevice inputDevice;
	Camera camera;

	std::chrono::time_point<std::chrono::steady_clock> prevTime;
	float deltaTime;
	float totalTime = 0;
	unsigned int frameCount = 0;
	
	void Initialize();
	int PrepareResources();
	void DestroyResources();
	void PrepareFrame();
	void EndFrame();
	void Update();
	void Draw();
	void ErrorsOutput(int ErrorCode);

	void CreateGrid();
	void CreateCube();
	void CreatePyramid();
	void CreateSphere();
	void CreateCapsule();

public:
	Game();
	void Run();
};