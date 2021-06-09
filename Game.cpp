#include "pch.h"
#include "Game.h"

Game::Game() {
	context = nullptr;
	swapChain = nullptr;
	rtv = nullptr;
	depthBuffer = nullptr;
	depthView = nullptr;
	annotation = nullptr;
	debug = nullptr;
}

void Game::Run() {
	Initialize();
	Display.CreateDisplay(&inputDevice);
	inputDevice.Initialize(Display.get_hWnd());
	camera.Initialize(Display.get_screenWidth(), Display.get_screenHeight(), &inputDevice);
	int errors = PrepareResources();
	ErrorsOutput(errors);

	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) { // ���� �� ��������� � ������ �� ���� ��� ������������
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ������������� ����
			TranslateMessage(&msg); // ������� ������� ������ � �������
			DispatchMessage(&msg); // ��������� ���������
			if (msg.message == WM_QUIT) { // ���� ���� �������� ��������� � ������, � ���� ������ ������� �� ����
				isExitRequested = true;
			}
		}
		if (errors == 0)
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
	CreateGrid();
	//CreateCube();
	//CreatePyramid();
	//CreateSphere();
	CreateCapsule();
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
	if (context != nullptr)
	{
		context->ClearState();
		context->Release();
	}
	if (swapChain != nullptr)
		swapChain->Release();
	if (rtv != nullptr)
		rtv->Release();
	if (depthBuffer)
		depthBuffer->Release();
	if (depthView != nullptr)
		depthView->Release();
	if (annotation != nullptr)
		annotation->Release();
	if (device != nullptr)
		device->Release();
	if (debug != nullptr)
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
	float color[] = { 0.08f, 0.0f, 0.24f, 1.0f };
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
	case ERROR_CREATING_NORMBUF:
		std::cout << "Error creating Normal Buffer" << std::endl;
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
	case ERROR_CREATING_LIGHTTBUF:
		std::cout << "Error creating Light Buffer" << std::endl;
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

void Game::CreateGrid() {
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
		grid.colors[i] = DirectX::SimpleMath::Vector4(1.0f, 0.88f, 1.0f, 1.0f);
	grid.numPoints = numDots;
	grid.compPosition = DirectX::SimpleMath::Vector3::Zero;
	Components.push_back(new LineComponent(grid));
}

void Game::CreateCube() {
	TriangleComponentParameters cube;
	cube.numPoints = 24;
	cube.numIndeces = 36;
	cube.positions = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f), // ������ �����
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, 0.5f, 1.0f), // ������� �����
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f), // �������� �����
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f), // ������ �����
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f), // ����� �����
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f), // ������ �����
	DirectX::SimpleMath::Vector4(-0.5f, 1.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 1.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f) };
	cube.colors = new DirectX::SimpleMath::Vector4[cube.numPoints];
	for (int i = 0; i < cube.numPoints; i++)
		cube.colors[i] = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	cube.texcoords = new DirectX::SimpleMath::Vector4[cube.numPoints];
	for (int i = 0; i < cube.numPoints; i += 4)
	{
		cube.texcoords[i] = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		cube.texcoords[i + 1] = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f);
		cube.texcoords[i + 2] = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		cube.texcoords[i + 3] = DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f);
	}
	cube.indeces = new int[] {
		0, 1, 2, // ������ �����
			2, 3, 0,
			6, 5, 4, // ������� �����
			4, 7, 6,
			8, 9, 10, // �������� �����
			10, 11, 8,
			14, 13, 12, // ������ �����
			12, 15, 14,
			18, 17, 16, // ����� �����
			16, 19, 18,
			20, 21, 22, // ������ �����
			22, 23, 20};
	cube.textureFileName = L"textures/gradient.png";
	cube.compPosition = DirectX::SimpleMath::Vector3(2, 0, 3);
	Components.push_back(new TriangleComponent(cube));
}

void Game::CreatePyramid() {
	TriangleComponentParameters pyramid;
	pyramid.numPoints = 16;
	pyramid.numIndeces = 18;
	pyramid.positions = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, -0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(-0.5f, 0.0f, 0.5f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, 1.0f) };
	pyramid.colors = new DirectX::SimpleMath::Vector4[pyramid.numPoints];
	for (int i = 0; i < pyramid.numPoints; i++)
		pyramid.colors[i] = DirectX::SimpleMath::Vector4(0.6f, 0.6f, 0.6f, 1.0f);
	pyramid.texcoords = new DirectX::SimpleMath::Vector4[]{
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f),
	DirectX::SimpleMath::Vector4(0.5f, 0.0f, 1.0f, 1.0f) };
	pyramid.indeces = new int[] {
		0, 1, 2, // 1 ����� ���������
			2, 3, 0, // 2 ����� ���������
			4, 6, 5, // 1 ������� �����
			7, 9, 8, // 2 ������� �����
			10, 12, 11, // 3 ������� �����
			13, 15, 14}; // 4 ������� �����
	pyramid.textureFileName = L"textures/colorful.png";
	pyramid.compPosition = DirectX::SimpleMath::Vector3(1.5, 0, 1);
	Components.push_back(new TriangleComponent(pyramid));
}

void Game::CreateSphere() {
	TriangleComponentParameters sphere;
	int parallels = 100; // ���������
	int meridians = 100; // ���������
	float r = 1; // ������ �����
	sphere.numPoints = parallels * meridians * 4 - 1 * 2 * meridians;
	sphere.numIndeces = parallels * meridians * 6 - 3 * 2 * meridians;
	sphere.positions = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	sphere.indeces = new int[sphere.numIndeces];
	int tempPos = 0;
	int tempInd = 0;
	DirectX::SimpleMath::Vector4 vertex1, vertex2, vertex3, vertex4;
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI;
		float teta2 = ((float)(i + 1) / parallels) * PI;
		for (int j = 0; j < meridians; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x, y, z;
			//� ������� �������� ���� �������� ������ ������� �������
			x = r * sin(teta1) * cos(fi1);
			y = r * sin(teta1) * sin(fi1);
			z = r * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			x = r * sin(teta1) * cos(fi2);
			y = r * sin(teta1) * sin(fi2);
			z = r * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			x = r * sin(teta2) * cos(fi2);
			y = r * sin(teta2) * sin(fi2);
			z = r * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			x = r * sin(teta2) * cos(fi1);
			y = r * sin(teta2) * sin(fi1);
			z = r * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);

			if (i == 0)
			{
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}	
			else if (i == parallels - 1)
			{
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4); 
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				sphere.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else
			{
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				tempPos++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				tempPos++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				tempPos++;
				sphere.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				tempPos++;

				sphere.indeces[tempInd] = tempPos - 1; // vertex4
				tempInd++; 
				sphere.indeces[tempInd] = tempPos - 4; // vertex1
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3; // vertex2
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 3; // vertex2
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 2; // vertex3
				tempInd++;
				sphere.indeces[tempInd] = tempPos - 1; // vertex4
				tempInd++;
			}
		}
	}
	sphere.colors = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	for (int i = 0; i < sphere.numPoints; i++)
		sphere.colors[i] = DirectX::SimpleMath::Vector4(0.59f, 0.0f, 0.25f, 1.0f);
	sphere.texcoords = new DirectX::SimpleMath::Vector4[sphere.numPoints];
	for (int i = 0; i < sphere.numPoints; i++)
		sphere.texcoords[i] = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	sphere.textureFileName = nullptr;
	sphere.compPosition = DirectX::SimpleMath::Vector3(1.5, 0, 1);
	Components.push_back(new TriangleComponent(sphere));
}

void Game::CreateCapsule() {
	TriangleComponentParameters capsule;
	int parallels = 100; // ���������
	int meridians = 100; // ���������
	float r = 1; // ������ �����
	float h = 2.0f; // ������ ��������
	capsule.numPoints = (parallels * meridians * 4 - 1 * 2 * meridians) + (parallels * meridians * 4);
	capsule.numIndeces = (parallels * meridians * 6 - 3 * 2 * meridians) + (parallels * meridians * 6);
	capsule.positions = new DirectX::SimpleMath::Vector4[capsule.numPoints];
	capsule.indeces = new int[capsule.numIndeces];
	int tempPos = 0;
	int tempInd = 0;
	DirectX::SimpleMath::Vector4 vertex1, vertex2, vertex3, vertex4;
	// ������� ���������
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI;
		float teta2 = ((float)(i + 1) / parallels) * PI;
		for (int j = 0; j < meridians / 2; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x, y, z;
			//� ������� �������� ���� �������� ������ ������� �������
			x = r * sin(teta1) * cos(fi1);
			y = r * sin(teta1) * sin(fi1);
			z = r * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y + h / 2, z, 1.0f);
			x = r * sin(teta1) * cos(fi2);
			y = r * sin(teta1) * sin(fi2);
			z = r * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y + h / 2, z, 1.0f);
			x = r * sin(teta2) * cos(fi2);
			y = r * sin(teta2) * sin(fi2);
			z = r * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y + h / 2, z, 1.0f);
			x = r * sin(teta2) * cos(fi1);
			y = r * sin(teta2) * sin(fi1);
			z = r * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y + h / 2, z, 1.0f);

			if (i == 0)
			{
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else if (i == parallels - 1)
			{
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else
			{
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				tempPos++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				tempPos++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				tempPos++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				tempPos++;

				capsule.indeces[tempInd] = tempPos - 1; // vertex4
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 4; // vertex1
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 3; // vertex2
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 3; // vertex2
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 2; // vertex3
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 1; // vertex4
				tempInd++;
			}
		}
	}
	
	// �������
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI * 2;
		float teta2 = ((float)(i + 1) / parallels) * PI * 2;
		for (int j = 0; j < meridians; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x, y, z;
			//� ������� �������� ���� �������� ������ ������� �������
			x = r * sin(teta1);
			y = sin(fi1);
			z = r * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			x = r * sin(teta1);
			y = sin(fi2);
			z = r * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			x = r * sin(teta2);
			y = sin(fi2);
			z = r * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);
			x = r * sin(teta2);
			y = sin(fi1);
			z = r * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y, z, 1.0f);

			capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
			tempPos++;
			capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
			tempPos++;
			capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
			tempPos++;
			capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
			tempPos++;

			capsule.indeces[tempInd] = tempPos - 1; // vertex4
			tempInd++;
			capsule.indeces[tempInd] = tempPos - 4; // vertex1
			tempInd++;
			capsule.indeces[tempInd] = tempPos - 3; // vertex2
			tempInd++;
			capsule.indeces[tempInd] = tempPos - 3; // vertex2
			tempInd++;
			capsule.indeces[tempInd] = tempPos - 2; // vertex3
			tempInd++;
			capsule.indeces[tempInd] = tempPos - 1; // vertex4
			tempInd++;
		}
	}

	// ������ ���������
	for (int i = 0; i < parallels; i++)
	{
		float teta1 = ((float)(i) / parallels) * PI;
		float teta2 = ((float)(i + 1) / parallels) * PI;
		for (int j = meridians / 2; j < meridians; j++)
		{
			float fi1 = ((float)(j) / meridians) * 2 * PI;
			float fi2 = ((float)(j + 1) / meridians) * 2 * PI;

			float x, y, z;
			//� ������� �������� ���� �������� ������ ������� �������
			x = r * sin(teta1) * cos(fi1);
			y = r * sin(teta1) * sin(fi1);
			z = r * cos(teta1);
			vertex1 = DirectX::SimpleMath::Vector4(x, y - h / 2, z, 1.0f);
			x = r * sin(teta1) * cos(fi2);
			y = r * sin(teta1) * sin(fi2);
			z = r * cos(teta1);
			vertex2 = DirectX::SimpleMath::Vector4(x, y - h / 2, z, 1.0f);
			x = r * sin(teta2) * cos(fi2);
			y = r * sin(teta2) * sin(fi2);
			z = r * cos(teta2);
			vertex3 = DirectX::SimpleMath::Vector4(x, y - h / 2, z, 1.0f);
			x = r * sin(teta2) * cos(fi1);
			y = r * sin(teta2) * sin(fi1);
			z = r * cos(teta2);
			vertex4 = DirectX::SimpleMath::Vector4(x, y - h / 2, z, 1.0f);

			if (i == 0)
			{
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else if (i == parallels - 1)
			{
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				capsule.indeces[tempInd] = tempPos;
				tempPos++;
				tempInd++;
			}
			else
			{
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex1);
				tempPos++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex2);
				tempPos++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex3);
				tempPos++;
				capsule.positions[tempPos] = DirectX::SimpleMath::Vector4(vertex4);
				tempPos++;

				capsule.indeces[tempInd] = tempPos - 1; // vertex4
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 4; // vertex1
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 3; // vertex2
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 3; // vertex2
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 2; // vertex3
				tempInd++;
				capsule.indeces[tempInd] = tempPos - 1; // vertex4
				tempInd++;
			}
		}
	}

	capsule.colors = new DirectX::SimpleMath::Vector4[capsule.numPoints];
	for (int i = 0; i < capsule.numPoints; i++)
		capsule.colors[i] = DirectX::SimpleMath::Vector4(0.59f, 0.0f, 0.25f, 1.0f);
	capsule.texcoords = new DirectX::SimpleMath::Vector4[capsule.numPoints];
	for (int i = 0; i < capsule.numPoints; i++)
		capsule.texcoords[i] = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	capsule.textureFileName = nullptr;
	capsule.compPosition = DirectX::SimpleMath::Vector3(5.0f, 2.0f, 0.0f);
	Components.push_back(new TriangleComponent(capsule));
}