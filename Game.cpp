#include "Game.h"

void Game::Run() {
	Initialize();
	Display.CreateDisplay();
	ErrorsOutput(PrepareResources());

	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) { // ���� �� ��������� � ������ �� ���� ��� ������������
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ������������� ����
			TranslateMessage(&msg); // ������� ������� ������ � �������
			DispatchMessage(&msg); // ��������� ���������
			std::cout << msg.message << std::endl;
		}
		if (msg.message == WM_QUIT) { // ���� ���� �������� ��������� � ������, � ���� ������ ������� �� ����
			isExitRequested = true;
		}
		PrepareFrame();
		Draw();
		EndFrame();
	}
	DestroyResources();
}

void Game::Initialize() {
	TriangleComponentParameters temp;
	temp.positions = new DirectX::XMFLOAT4[4];
	temp.positions[0] = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	temp.positions[1] = XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f);
	temp.positions[2] = XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f);
	temp.positions[3] = XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f);
	temp.colors = new DirectX::XMFLOAT4[4];
	temp.colors[0] = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	temp.colors[1] = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	temp.colors[2] = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	temp.colors[3] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	temp.indeces = new int[6];
	temp.indeces[0] = 0;
	temp.indeces[1] = 1;
	temp.indeces[2] = 2;
	temp.indeces[3] = 1;
	temp.indeces[4] = 0;
	temp.indeces[5] = 3;
	temp.numPoints = 4;
	temp.numIndeces = 6;
	Components.push_back(new TriangleComponent(temp));
}

int Game::PrepareResources() {
	HRESULT res;

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
	if (FAILED(res))
		return ERROR_DEV_SC;

	ID3D11Texture2D* backTex; // ��������� 2D-�������� (��������� ������� �������� (����������� ������� �������� ���������� �������))
	res = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backTex); // ������ � ������ �� ������ ������� ���������
	if (FAILED(res))
		return ERROR_SCBUF;
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv); // �������� ������������� �������� ������� ����������
	if (FAILED(res))
		return ERROR_RTV;

	context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)&annotation); // ������ ���������� ���������
	device->QueryInterface(IID_ID3D11Debug, (void**)&debug); // ������ ���������� �������

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
		if (result != SUCCESS && result != NOTHING_TO_DRAW)
			return result;
	}
	auto prevTime = std::chrono::steady_clock::now();
	return SUCCESS;
}

void Game::DestroyResources() {
	for (int i = 0; i < Components.size(); i++)
		Components[i]->DestroyResourses();
	device->Release();
	debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void Game::PrepareFrame() {
	auto curTime = std::chrono::steady_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
	prevTime = curTime;
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
	context->ClearState();
	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	context->OMSetRenderTargets(1, &rtv, nullptr); // �������� ������ ������� � ������� ������, ��������� �������� - ������� ��������
	context->ClearRenderTargetView(rtv, color);
	context->RSSetViewports(1, &viewport); // ������ �������� - ���������� ����
}

void Game::EndFrame() {
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // ������ ��������� ������ �� ������ ����� ��������� � ������
}

void Game::Update() {

}

void Game::Draw() {
	annotation->BeginEvent(L"BeginDraw");
	for (int i = 0; i < Components.size(); i++)
		Components[i]->Draw(context);
	annotation->EndEvent();
}

void Game::ErrorsOutput(int ErrorCode) {
	switch (ErrorCode) {
	case ERROR_VBC:
		std::cout << "Error compiling Vertex Byte Code";
		break;
	case MISSING_VS:
		std::cout << "Missing Vertex Shader file";
		break;
	case ERROR_PBC:
		std::cout << "Error compiling Pixel Byte Code";
		break;
	case MISSING_PS:
		std::cout << "Missing Pixel Shader file";
		break;
	case ERROR_CREATING_VS:
		std::cout << "Error creating Vertex Shader";
		break;
	case ERROR_CREATING_PS:
		std::cout << "Error creating Pixel Shader";
		break;
	case ERROR_CREATING_LAYOUT:
		std::cout << "Error creating Layout";
		break;
	case ERROR_CREATING_POSBUF:
		std::cout << "Error creating Position Buffer";
		break;
	case ERROR_CREATING_COLBUF:
		std::cout << "Error creating Color Buffer";
		break;
	case ERROR_CREATING_INDBUF:
		std::cout << "Error creating Index Buffer";
		break;
	case ERROR_CREATING_RASTSTATE:
		std::cout << "Error creating Rasterizer State";
		break;
	case ERROR_DEV_SC:
		std::cout << "Error creating Device and SwapChain";
		break;
	case ERROR_SCBUF:
		std::cout << "Error SwapChain get buffer";
		break;
	case ERROR_RTV:
		std::cout << "Error creating RenderTargetView";
		break;
	case SUCCESS:
		break;
	default:
		std::cout << "Unidentified error";
		break;
	}
}

