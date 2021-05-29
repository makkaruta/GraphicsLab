#include "pch.h"
#include "Game.h"

void Game::Run() {
	Initialize();
	Display.CreateDisplay(&inputDevice);
	inputDevice.Initialize(Display.get_hWnd());
	camera.Initialize(Display.get_screenWidth(), Display.get_screenHeight(), &inputDevice);
	//std::cout << "Width: " << Display.get_screenWidth() << " Height: " << Display.get_screenHeight() << std::endl;
	ErrorsOutput(PrepareResources());

	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) { // ���� �� ��������� � ������ �� ���� ��� ������������
		//inputDevice.MouseParam
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ������������� ����
			TranslateMessage(&msg); // ������� ������� ������ � �������
			DispatchMessage(&msg); // ��������� ���������
			//std::cout << msg.message << std::endl;
		}
		if (msg.message == WM_QUIT) { // ���� ���� �������� ��������� � ������, � ���� ������ ������� �� ����
			isExitRequested = true;
		}
		PrepareFrame();
		Update();
		Draw();
		EndFrame();
	}
	DestroyResources();
}

void Game::Initialize() {
	TriangleComponentParameters temp;
	temp.positions = new DirectX::SimpleMath::Vector4[5];
	temp.positions[0] = DirectX::SimpleMath::Vector4(-0.5f, -0.5f, 0.8f, 1.0f);
	temp.positions[1] = DirectX::SimpleMath::Vector4(0.5f, -0.5f, 0.8f, 1.0f);
	temp.positions[2] = DirectX::SimpleMath::Vector4(0.5f, -0.5f, 0.4f, 1.0f);
	temp.positions[3] = DirectX::SimpleMath::Vector4(-0.5f, -0.5f, 0.4f, 1.0f);
	temp.positions[4] = DirectX::SimpleMath::Vector4(0.0f, 0.5f, 0.6f, 1.0f);
	temp.colors = new DirectX::SimpleMath::Vector4[5];
	temp.colors[0] = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 0.0f, 0.5f);
	temp.colors[1] = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 0.5f);
	temp.colors[2] = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 0.5f);
	temp.colors[3] = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 0.5f);
	temp.colors[4] = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 0.5f);
	temp.indeces = new int[18];
	temp.indeces[0] = 0; // 1 ����� ���������
	temp.indeces[1] = 1;
	temp.indeces[2] = 2;
	temp.indeces[3] = 2; // 2 ����� ���������
	temp.indeces[4] = 3;
	temp.indeces[5] = 0;
	temp.indeces[6] = 0; // 1 ������� �����
	temp.indeces[7] = 1;
	temp.indeces[8] = 4;
	temp.indeces[9] = 1; // 2 ������� �����
	temp.indeces[10] = 2;
	temp.indeces[11] = 4;
	temp.indeces[12] = 2; // 3 ������� �����
	temp.indeces[13] = 3; 
	temp.indeces[14] = 4;
	temp.indeces[15] = 3; // 4 ������� �����
	temp.indeces[16] = 0;
	temp.indeces[17] = 4;
	temp.numPoints = 5;
	temp.numIndeces = 18;
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
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
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
	//float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	float color[] = { 0.2f, 0.2f, 0.2f, 0.5f };
	context->OMSetRenderTargets(1, &rtv, nullptr); // �������� ������ ������� � ������� ������, ��������� �������� - ������� ��������
	context->ClearRenderTargetView(rtv, color);
	context->RSSetViewports(1, &viewport); // ������ �������� - ���������� ����
}

void Game::EndFrame() {
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // ������ ��������� ������ �� ������ ����� ��������� � ������
}

void Game::Update() {
	annotation->BeginEvent(L"BeginUpdate");
	camera.OnMouseMove(inputDevice.getMouseParam());

	//std::cout << "Width: " << Display.get_screenWidth() << " Height: " << Display.get_screenHeight() << std::endl;
	camera.Update(deltaTime, Display.get_screenWidth(), Display.get_screenHeight());
	for (int i = 0; i < Components.size(); i++)
		Components[i]->Update(context, &camera); 
	annotation->EndEvent();
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
		std::cout << "Error compiling Vertex Byte Code" << std::endl;
		break;
	case MISSING_VS:
		std::cout << "Missing Vertex Shader file" << std::endl;
		break;
	case ERROR_PBC:
		std::cout << "Error compiling Pixel Byte Code" << std::endl;
		break;
	case MISSING_PS:
		std::cout << "Missing Pixel Shader file" << std::endl;
		break;
	case ERROR_CREATING_VS:
		std::cout << "Error creating Vertex Shader" << std::endl;
		break;
	case ERROR_CREATING_PS:
		std::cout << "Error creating Pixel Shader" << std::endl;
		break;
	case ERROR_CREATING_LAYOUT:
		std::cout << "Error creating Layout" << std::endl;
		break;
	case ERROR_CREATING_POSBUF:
		std::cout << "Error creating Position Buffer" << std::endl;
		break;
	case ERROR_CREATING_COLBUF:
		std::cout << "Error creating Color Buffer" << std::endl;
		break;
	case ERROR_CREATING_INDBUF:
		std::cout << "Error creating Index Buffer" << std::endl;
		break;
	case ERROR_CREATING_BLENDSTATE:
		std::cout << "Error creating Blend State" << std::endl;
		break;
	case ERROR_CREATING_CONSTBUF:
		std::cout << "Error creating Constant Bufer" << std::endl;
		break;
	case ERROR_CREATING_RASTSTATE:
		std::cout << "Error creating Rasterizer State" << std::endl;
		break;
	case ERROR_DEV_SC:
		std::cout << "Error creating Device and SwapChain" << std::endl;
		break;
	case ERROR_SCBUF:
		std::cout << "Error SwapChain get buffer" << std::endl;
		break;
	case ERROR_RTV:
		std::cout << "Error creating RenderTargetView" << std::endl;
		break;
	case SUCCESS:
		break;
	default:
		std::cout << "Unidentified error" << std::endl;
		break;
	}
}

