#include "TriangleComponent.h"

TriangleComponent::TriangleComponent() {
	positions = nullptr;
	colors = nullptr;
	indeces = nullptr;
}

TriangleComponent::TriangleComponent(DirectX::XMFLOAT4* positionsArr, DirectX::XMFLOAT4* colorsArr, int* indecesArr) {
	positions = positionsArr;
	colors = colorsArr;
	indeces = indecesArr;
}

void TriangleComponent::Initialize(){

}

void TriangleComponent::Update() {

}


int TriangleComponent::PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) {
	HRESULT res;

	
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

			std::cout << "Missing Shader File";
			//MessageBox(Display.get_hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
		}
		return 0;
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr }; // (�������: ��������, �����������)
	
	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(L"MiniTri.fx", Shader_Macros, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
	if (FAILED(res)) {
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			std::cout << "Missing Shader File";
			//MessageBox(Display.get_hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
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

	
	device->CreateInputLayout(
		inputElements,
		2, // ���������� ������� ���������
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);



	D3D11_BUFFER_DESC dataBufDesc = {}; // ��������� �������� ������ ������
	dataBufDesc.Usage = D3D11_USAGE_DEFAULT; // ��������� ������ ��� ������ � ������ �� ������� ������������ ����������
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �������� ������ � �������� ������ ������
	dataBufDesc.CPUAccessFlags = 0; // ������ � �� �� ���������
	dataBufDesc.MiscFlags = 0; // �������������� �����
	dataBufDesc.StructureByteStride = 0; // ������ ������� �������� � ��������� � ������ (������ ����?)
	dataBufDesc.ByteWidth = sizeof(XMFLOAT4) * 4;// std::size(positions); // ������ ������ � ������

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
	indexBufDesc.ByteWidth = sizeof(int) * 6;//std::size(indeces);

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&indexBufDesc, &indexData, &indBuf);


	vBuffers[0] = pb;
	vBuffers[1] = cb;
	strides[0] = 16;
	offsets[1] = 0;
	strides[0] = 16;
	offsets[1] = 0;



	CD3D11_RASTERIZER_DESC rastDesc = {}; // ���������� �������������
	rastDesc.CullMode = D3D11_CULL_NONE; // ������������, ���������� � ��������� �����������, �� ������������ (������ ������������)
	rastDesc.FillMode = D3D11_FILL_SOLID; // ����� ������� (����������)

	res = device->CreateRasterizerState(&rastDesc, &rastState); ZCHECK(res);

	return 1;
}

void TriangleComponent::DestroyResourses() {

	vertexShader->Release();
	pixelShader->Release();
}

void TriangleComponent::Draw(ID3D11DeviceContext* context) {


	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �������������: �������� ������� ������� ������������
	// ������ ��� - ����� �������������, ����� ��������� ���������� �� �������� 0-1-2, 1-2-3, 2-3-4
	context->IASetIndexBuffer(indBuf, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(
		0, // ������ ����
		2, // ���������� �������
		vBuffers, // ������ ������
		strides,
		offsets);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->RSSetState(rastState);



	context->DrawIndexed(
		6, // ���������� �������������� �������� �� ������ ��������
		0, // ������ ������ ��� ���������
		0);// ��������, ����������� � ������� ������� ����� ������� ������� �� ������ ������
}
