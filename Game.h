#pragma once

#include "pch.h"

#include "DisplayWin32.h"
#include "Camera.h"
#include "GameComponent.h"
#include "TriangleComponent.h"

#define ERROR_DEV_SC 13
#define ERROR_SCBUF 14
#define ERROR_RTV 15

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
	ID3DUserDefinedAnnotation* annotation; // �������� �������������� �������� � �������� � ������ ���� ����������
	// ��������� ���������� � ������������ ������ ��������� ����� ��� ��������, ����� ����� ������� Direct3D ������������ ������ ��������� ���� ����������
	ID3D11Debug* debug; // ��������� ������� ��������� ����������� ������� � ��������� ��������� ���������
	InputDevice inputDevice;
	Camera camera;

	std::chrono::time_point<std::chrono::steady_clock> prevTime;
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

public:
	void Run();
};