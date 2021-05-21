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
	Components.push_back(TriangleComponent(positions,colors,indeces));
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

	ID3DBlob* vertexBC; // ������� ���� ���, ��������� ���������� ���������� ����� � ���������� ��������
	ID3DBlob* errorVertexCode; // ��������� �� �������
	res = D3DCompileFromFile( // ���������� ����������� ������� �� �����
		L"MiniTri.fx", // �������� ����� � ��������
		nullptr, // ������� �������
		nullptr, // include � ����� � ���������
		"VSMain", // ����� �����
		"vs_5_0", // ���� ������� (?)
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // ����� ������� (���������� ���������� �� ���������� ����� + ������� ����������� ��� �������)
		0, // ����� ������� 
		&vertexBC,
		&errorVertexCode);
	if (FAILED(res)) {
		if (errorVertexCode) { // ���� ������ �� ��� �������������, ���-�� ������ ���� �������� � ��������� �� ������
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else // ���� � ��������� �� ������ ������ �� ����, ������, �� ������ �� ���� ����� ��� ���� �������
		{
			MessageBox(Display.get_hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
		}
		return 0;
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr }; // (�������: ��������, �����������)
	ID3DBlob* pixelBC; // ������� ���� ���, ��������� ���������� ���������� ����� � ���������� ��������
	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(L"MiniTri.fx", Shader_Macros, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
	if (FAILED(res)) {
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			MessageBox(Display.get_hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
		}
		return 0;
	}

	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = { // �������� ������� ������ � ������
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0, // ������������� ������ ��������
			DXGI_FORMAT_R32G32B32A32_FLOAT, // ������ ����� ��������� �� �������?
			0, // ������ ����� �����
			0, // �������� �� ������
			D3D11_INPUT_PER_VERTEX_DATA, // ����� ������� ������ - ������ ��� ������ �������
			0}, // ���-�� ��-��� ��� ��������� � ���. ����� � ��� �� ������ ����� ������������ � ������ �� ���� �������
			// ��� �������� ������ ���� 0 ��� ��������, ������� �������� ������ ��� ������ �������
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT, // RGBA �� 32 ���� �� �����
			1,
			D3D11_APPEND_ALIGNED_ELEMENT, // ��������: ���������� ������� ������� ����� ����� �����������
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	ID3D11InputLayout* layout; // ����������� ����, ��� ���������� ������ ������, ������� ��������� � ������, �� ���� �������� ����� ������������ ���������
	device->CreateInputLayout(
		inputElements,
		2, // ���������� ������� ���������
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);
	
	
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

	D3D11_BUFFER_DESC dataBufDesc = {}; // ��������� �������� ������ ������
	dataBufDesc.Usage = D3D11_USAGE_DEFAULT; // ��������� ������ ��� ������ � ������ �� ������� ������������ ����������
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �������� ������ � �������� ������ ������
	dataBufDesc.CPUAccessFlags = 0; // ������ � �� �� ���������
	dataBufDesc.MiscFlags = 0; // �������������� �����
	dataBufDesc.StructureByteStride = 0; // ������ ������� �������� � ��������� � ������ (������ ����?)
	dataBufDesc.ByteWidth = sizeof(XMFLOAT4) * std::size(positions); // ������ ������ � ������

	D3D11_SUBRESOURCE_DATA positionsData = {}; // ��������� ��� ������������� ����������
	positionsData.pSysMem = positions; // ������
	positionsData.SysMemPitch = 0; // ���������� (� ������) �� ������ ����� ������ �� ��������� ������
	positionsData.SysMemSlicePitch = 0; // ���������� (� ������) �� ������ ������ ������ ������� �� ���������� (��� ���������� �������)
	D3D11_SUBRESOURCE_DATA colorsData = {};
	colorsData.pSysMem = colors;
	colorsData.SysMemPitch = 0;
	colorsData.SysMemSlicePitch = 0;

	ID3D11Buffer* pb; // ����� �������
	ID3D11Buffer* cb; // ����� ������

	device->CreateBuffer(&dataBufDesc, &positionsData, &pb);
	device->CreateBuffer(&dataBufDesc, &colorsData, &cb);

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	ID3D11Buffer* ib;
	device->CreateBuffer(&indexBufDesc, &indexData, &ib);


	ID3D11Buffer* vBuffers[] = { pb, cb };
	UINT strides[] = { 16, 16 }; // ��� ������ ��� ������� ������
	UINT offsets[] = { 0, 0 }; // �������� �� ������ ��� ������� ������

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �������������: �������� ������� ������� ������������
	// ������ ��� - ����� �������������, ����� ��������� ���������� �� �������� 0-1-2, 1-2-3, 2-3-4
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(
		0, // ������ ����
		2, // ���������� �������
		vBuffers, // ������ ������
		strides, 
		offsets); 
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	CD3D11_RASTERIZER_DESC rastDesc = {}; // ���������� �������������
	rastDesc.CullMode = D3D11_CULL_NONE; // ������������, ���������� � ��������� �����������, �� ������������ (������ ������������)
	rastDesc.FillMode = D3D11_FILL_SOLID; // ����� ������� (����������)
	ID3D11RasterizerState* rastState;
	res = device->CreateRasterizerState(&rastDesc, &rastState); ZCHECK(res);
	context->RSSetState(rastState);

	D3D11_VIEWPORT viewport = {}; // ������� ������� ���������
	viewport.Width = static_cast<float>(Display.get_screenWidth());
	viewport.Height = static_cast<float>(Display.get_screenHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport); // ������ �������� - ���������� ����
	return 0;
}

void Game::DestroyResources() {
	vertexShader->Release();
	pixelShader->Release();
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
	context->DrawIndexed(
		6, // ���������� �������������� �������� �� ������ ��������
		0, // ������ ������ ��� ���������
		0);// ��������, ����������� � ������� ������� ����� ������� ������� �� ������ ������
	annotation->EndEvent();
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // ������ ��������� ������ �� ������ ����� ��������� � ������
}