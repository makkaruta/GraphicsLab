#include "Game.h"

using namespace DirectX;

void Game::Initialize() {
	DirectX::XMFLOAT4 positions[] = {
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
		XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
		XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
	};
	DirectX::XMFLOAT4 colors[] = {
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	int indeces[] = { 0,1,2, 1,0,3 };
	Components.push_back(new TriangleComponent(positions,colors,indeces));
}

int Game::Run() {

	Display.CreateDisplay();
	Initialize();

	HRESULT res; // ���������� � ���������� ������ (����� �������)

	DXGI_SWAP_CHAIN_DESC swapDesc = {}; // ���������� ��������� (��������� � �����������)
	swapDesc.BufferCount = 2; // ���������� �������
	swapDesc.BufferDesc.Width = Display.get_screenWidth(); // ���������� ������: ������ ������
	swapDesc.BufferDesc.Height = Display.get_screenHeight(); // ���������� ������: ������ ������
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // ���������� ������: ������ RGBA, ��� �� ������ ����� �� 8 ���
	swapDesc.BufferDesc.RefreshRate.Numerator = 60; // ������� ���������� � ������: ������� ����� ������������� ����� (�����)
	swapDesc.BufferDesc.RefreshRate.Denominator = 1; // ������� ���������� � ������: ������ ����� �����
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �������� ������ ��������� ����� ��������� (����� ���� ������������� ��� �������������)
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // �����, �����������, ��� ����������� ������������� ��� ������������ ���������� ������� �������� 
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ��������� ������������� ��������� ������� � �� ��� ������� ������ (������ �� �����)
	swapDesc.OutputWindow = Display.get_hWnd(); // ���������� ���� ������
	swapDesc.Windowed = true; 
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // ��������� ��������� �������� ����� ������ ��������� (����������� ���������� ������� ������ ����� ���������)
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // �����: ��������� ���������� ����������� ������ �� �������� � ������������� (���������� ����� �������� � ������������ � ��������� ���� ����������)
	swapDesc.SampleDesc.Count = 1; // ���������� ������������� �� ������� (��� �����������)
	swapDesc.SampleDesc.Quality = 0; // ������� �������� �����������

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 }; // ����� ������� Direct3D
	res = D3D11CreateDeviceAndSwapChain(
		nullptr, // ������������ (�� ���������)
		D3D_DRIVER_TYPE_HARDWARE, // ������� Direct3D ����� ������������� ���������� ������� 
		nullptr, // ���������� ������������ ������������� 
		D3D11_CREATE_DEVICE_DEBUG, // ������� ����������, �������������� ������� �������
		featureLevel, // ������� ������� �������
		1, // ���������� ��������� � ������ �������
		D3D11_SDK_VERSION, // ������ SDK
		&swapDesc, // ���������� ���������
		&swapChain,
		&device,
		nullptr, // ���������� ���������, ������� ������������ ������ ������� � ������� ������� ������� (null, ���� �� ����� ����������)
		&context); // ���������� ����� ��������� �� ������, �������������� �������� ���������� 
	ZCHECK(res); //�������� ��� ��������� �������

	ID3D11Texture2D* backTex; // ��������� 2D-�������� (��������� ������� �������� (����������� ������� �������� ���������� �������))
	res = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backTex);	ZCHECK(res); // ������ � ������ �� ������ ������� ���������
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);			ZCHECK(res); // �������� ������������� �������� ������� ����������

	context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)&annotation); // ������ ���������� ���������
	device->QueryInterface(IID_ID3D11Debug, (void**)&debug); // ������ ���������� �������



	//D3D11_VIEWPORT viewport = {}; // ������� ������� ���������
	viewport.Width = static_cast<float>(Display.get_screenWidth());
	viewport.Height = static_cast<float>(Display.get_screenHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	int result;
	for (int i = 0; i < Components.size(); i++)
	{
		result = Components[i]->PrepareResourses(device);
		if (result == 0)
			return 0;
	}



	return 1;
}

void Game::DestroyResources() {
	device->Release();
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void Game::Draw() {

	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;
	totalTime += deltaTime;
	frameCount++;
	if (totalTime > 1.0f) { // ������� ������ � �������
		float fps = frameCount / totalTime;
		totalTime = 0.0f;
		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(Display.get_hWnd(), text);
		frameCount = 0;
	}

	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	context->OMSetRenderTargets(1, &rtv, nullptr); // �������� ������ ������� � ������� ������, ��������� �������� - ������� ��������
	context->ClearRenderTargetView(rtv, color);
	annotation->BeginEvent(L"BeginDraw");

	context->RSSetViewports(1, &viewport); // ������ �������� - ���������� ����


	for (int i = 0; i < Components.size(); i++)
		Components[i]->Draw(context);

	annotation->EndEvent();
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // ������ ��������� ������ �� ������ ����� ��������� � ������
}