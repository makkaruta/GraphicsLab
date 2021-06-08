#include "pch.h"
#include "Game.h"

void Game::Run() {
	Initialize();
	Display.CreateDisplay(&inputDevice);
	inputDevice.Initialize(Display.get_hWnd());
	camera.Initialize(Display.get_screenWidth(), Display.get_screenHeight(), &inputDevice);
	int res = PrepareResources();
	ErrorsOutput(res);

	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) { // ���� �� ��������� � ������ �� ���� ��� ������������
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ������������� ����
			TranslateMessage(&msg); // ������� ������� ������ � �������
			DispatchMessage(&msg); // ��������� ���������
		}
		if (msg.message == WM_QUIT) { // ���� ���� �������� ��������� � ������, � ���� ������ ������� �� ����
			isExitRequested = true;
		}
		if (res == 0)
		{
			PrepareFrame();
			Update();
			Draw();
			EndFrame();
		}
		
	}
	DestroyResources();
}

void Game::Initialize() {
	// ����� ����
	LineComponentParameters grid;
	int numDots = 200;
	float density = 2.0f;
	float offsetX = (((float)numDots) / (density)) / 1.35f;
	float offsetY = 25.0f;
	float distance = 100.0f;
	grid.positions = new DirectX::SimpleMath::Vector4[numDots];
	for (int i = 0; i < numDots / 2; i += 2)
	{
		grid.positions[i] = DirectX::SimpleMath::Vector4(-distance, 0.0f, offsetY - ((float)i) / density, 1.0f);
		grid.positions[i + 1] = DirectX::SimpleMath::Vector4(distance, 0.0f, offsetY - ((float)i) / density, 1.0f);
	}
	for (int i = numDots / 2; i < numDots; i += 2)
	{
		grid.positions[i] = DirectX::SimpleMath::Vector4(offsetX - ((float)i) / density, 0.0f, -distance, 1.0f);
		grid.positions[i + 1] = DirectX::SimpleMath::Vector4(offsetX - ((float)i) / density, 0.0f, distance, 1.0f);
	}
	grid.colors = new DirectX::SimpleMath::Vector4[numDots];
	for (int i = 0; i < numDots; i++)
		grid.colors[i] = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f); 
	grid.numPoints = numDots;
	grid.compPosition = DirectX::SimpleMath::Vector3::Zero;
	Components.push_back(new LineComponent(grid));

	// ���������
	TriangleComponentParameters pyramid;
	pyramid.positions = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f)};
	pyramid.colors = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f)};
	pyramid.texcoords = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 1.0f, 1.0f) };
	pyramid.indeces = new int[] {
		0, 1, 2, // 1 ����� ���������
		2, 3, 0, // 2 ����� ���������
		0, 4, 1, // 1 ������� �����
		1, 4, 2, // 2 ������� �����
		2, 4, 3, // 3 ������� �����
		3, 4, 0}; // 4 ������� �����
	pyramid.numPoints = 5;
	pyramid.numIndeces = 18;
	pyramid.textureFileName = L"textures/colorful.png";
	pyramid.compPosition = DirectX::SimpleMath::Vector3(1.5, 0, 1);
	Components.push_back(new TriangleComponent(pyramid));
	
	// ���
	TriangleComponentParameters cube;
	cube.positions = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f), // ������ �������
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, 0.5f, 1.0f), // ������� �������
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, -0.5f, 1.0f)};
	cube.colors = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f) };
	cube.texcoords = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(0.666f, 0.55f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 0.55f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 0.55f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.333f, 0.55f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.666f, 0.25f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 0.25f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 0.25f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.333f, 0.25f, 1.0f, 1.0f) };
	cube.indeces = new int[] {
		0, 1, 2, // ������ �������
		2, 3, 0,
		4, 6, 5, // ������� ��������
		6, 4, 7,
		0, 4, 1, // 1 ������� �����
		1, 4, 5, 
		1, 5, 2, // 2 ������� �����
		2, 5, 6, 
		2, 7, 3, // 3 ������� �����
		2, 6, 7, 
		0, 3, 4, // 4 ������� �����
		3, 7, 4};
	cube.numPoints = 8;
	cube.numIndeces = 12*3; 
	cube.textureFileName = nullptr; // L"textures/pion.png";
	cube.compPosition = DirectX::SimpleMath::Vector3(2, 0, 3);
	Components.push_back(new TriangleComponent(cube));
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
		return ERROR_DEV_SWAPCHAIN;

	ID3D11Texture2D* backTex; // ��������� 2D-�������� (��������� ������� �������� (����������� ������� �������� ���������� �������))
	res = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backTex); // ������ � ������ �� ������ ������� ���������
	if (FAILED(res))
		return ERROR_SWAPCHAIN_BUF;

	res = device->CreateRenderTargetView(backTex, nullptr, &rtv); // �������� ������������� �������� ������� ����������
	if (FAILED(res))
		return ERROR_RENDER_TARGER;

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.ArraySize = 1; // ���������� ������� � ������� �������
	depthTexDesc.MipLevels = 1; // ������������ ���������� ������� MIP - ����� � ��������
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS; // ���������������� 32-������ ���������� ������, �������������� 32 ���� ��� �������� ������
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL; // �������� � ������� � ������ ������
	depthTexDesc.CPUAccessFlags = 0; // ������ � �� �� ���������
	depthTexDesc.MiscFlags = 0; // �������������� �����
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT; // ������ ������ � ������ ��������
	depthTexDesc.Width = Display.get_screenWidth();
	depthTexDesc.Height = Display.get_screenHeight();
	depthTexDesc.SampleDesc = {1, 0}; // ���������, ������������ ��������� ��������������� ��� �������� (���������� ������������� �� �������, �������� �����������)
	res = device->CreateTexture2D(&depthTexDesc, nullptr, &depthBuffer);
	if (FAILED(res)) 
		return ERROR_DEPTH_BUF;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStenDesc = {};
	depthStenDesc.Format = DXGI_FORMAT_D32_FLOAT; // ���������������� 32-������ ������ � ��������� �������, �������������� 32-������ �������
	depthStenDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // ��� ����� �������������� ������ � ������� ��������� �������
	depthStenDesc.Flags = 0;
	res = device->CreateDepthStencilView(depthBuffer, &depthStenDesc, &depthView); 
	if (FAILED(res)) 
		return ERROR_DEPTH_STENSIL;

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
		result = Components[i]->LoadTextureFromFile(device, context, false, false, 0);
		if (result != SUCCESS && result != ERROR_TEXTURE_FILE_MISSING)
			return result;
	}

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

	/*
	if (Context) Context->ClearState();
	if (_constantBuffer) _constantBuffer->Release();
	if (_vertexBuffer) _vertexBuffer->Release();
	if (_indexBuffer) _indexBuffer->Release();
	for (auto c : Components)
	{
		c->DestroyResources();
	}
	if (RenderView) RenderView->Release();
	if (Context) Context->Release();
	if (Device) Device->Release();*/
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
	context->OMSetRenderTargets(1, &rtv, depthView); // �������� ������ ������� � ������ ������ � ������� ������
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport); // ������ �������� - ���������� ����
}

void Game::EndFrame() {
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // ������ ��������� ������ �� ������ ����� ��������� � ������
}

void Game::Update() {
	annotation->BeginEvent(L"BeginUpdate");
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
	case ERROR_VERTEX_BC:
		std::cout << "Error compiling Vertex Byte Code" << std::endl;
		break;
	case MISSING_VERTEX_SHADER:
		std::cout << "Missing Vertex Shader file" << std::endl;
		break;
	case ERROR_PIXEL_BC:
		std::cout << "Error compiling Pixel Byte Code" << std::endl;
		break;
	case MISSING_PIXEL_SHADER:
		std::cout << "Missing Pixel Shader file" << std::endl;
		break;
	case ERROR_CREATING_VERTEX_SHADER:
		std::cout << "Error creating Vertex Shader" << std::endl;
		break;
	case ERROR_CREATING_PIXEL_SHADER:
		std::cout << "Error creating Pixel Shader" << std::endl;
		break;
	case ERROR_CREATING_COM_OBJ:
		std::cout << "Error creating COM object" << std::endl;
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
	case ERROR_CREATING_TEXBUF:
		std::cout << "Error creating Texture Coord Buffer" << std::endl;
		break;
	case ERROR_CREATING_INDBUF:
		std::cout << "Error creating Index Buffer" << std::endl;
		break;
	case ERROR_CREATING_BLENDSTATE:
		std::cout << "Error creating Blend State" << std::endl;
		break;
	case ERROR_CREATING_CONSTBUF:
		std::cout << "Error creating Constant Buffer" << std::endl;
		break;
	case ERROR_CREATING_RASTSTATE:
		std::cout << "Error creating Rasterizer State" << std::endl;
		break;
	case ERROR_CREATING_DECODER:
		std::cout << "Error creating Decoder from texture file" << std::endl;
		break;
	case ERROR_GET_FRAME:
		std::cout << "Error getting frame from texture" << std::endl;
		break;
	case ERROR_CREATING_CONVERTER:
		std::cout << "Error creating format Converter" << std::endl;
		break;
	case ERROR_INITIALIZE_CONVERTER:
		std::cout << "Error initializing Converter" << std::endl;
		break;
	case ERROR_COPY_PIXELS:
		std::cout << "Error Copy Pixels" << std::endl;
		break;
	case ERROR_CREATING_TEXTURE:
		std::cout << "Error creating Texture" << std::endl;
		break;
	case ERROR_CREATING_SHADER_RV:
		std::cout << "Error creating Shader ResourceView" << std::endl;
		break;
	case ERROR_RESOURCES_NOT_PREPARED:
		std::cout << "Error resources not prepared" << std::endl;
		break;
	case ERROR_DEV_SWAPCHAIN:
		std::cout << "Error creating Device and Swap Chain" << std::endl;
		break;
	case ERROR_SWAPCHAIN_BUF:
		std::cout << "Error creating SwapChain Buffer" << std::endl;
		break;
	case ERROR_RENDER_TARGER:
		std::cout << "Error creating Render Target View" << std::endl;
		break;
	case ERROR_DEPTH_BUF:
		std::cout << "Error creating Depth Buffer" << std::endl;
		break;
	case ERROR_DEPTH_STENSIL:
		std::cout << "Error creating Depth Stensil View" << std::endl;
		break;
	case ERROR_TEXTURE_FILE_MISSING:
		std::cout << "Error Texture File missing" << std::endl;
		break;
	case ERROR_LOAD_TEXTURE_ORDER:
		std::cout << "Error: you should load texture before preparing resources" << std::endl;
		break;
	case ERROR_CREATING_SAMPLER_STATE:
		std::cout << "Error creatinf Sampler State" << std::endl;
		break;
	case SUCCESS:
		break;
	default:
		std::cout << "Unidentified error" << std::endl;
		break;
	}
}

