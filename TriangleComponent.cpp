#include "pch.h"
#include "TriangleComponent.h"

TriangleComponent::TriangleComponent() {
	parameters.positions = nullptr;
	parameters.colors = nullptr;
	parameters.indeces = nullptr;
	parameters.numPoints = 0;
	parameters.numIndeces = 0;
	vertexShader = nullptr;
	pixelShader = nullptr;
	compPosition = DirectX::SimpleMath::Vector3(0, 0.5 ,0);
}

TriangleComponent::TriangleComponent(TriangleComponentParameters param) {
	parameters.positions = param.positions;
	parameters.colors = param.colors;
	parameters.indeces = param.indeces;
	parameters.numPoints = param.numPoints;
	parameters.numIndeces = param.numIndeces;
	vertexShader = nullptr;
	pixelShader = nullptr;
	compPosition = DirectX::SimpleMath::Vector3(0, 0.5, 0);
}

int TriangleComponent::PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) {
	if (parameters.numIndeces != 0)
	{
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
				return ERROR_VBC;
			}
			else // ���� � ��������� �� ������ ������ �� ����, ������, �� ������ �� ���� ����� ��� ���� �������
				return MISSING_VS;
		}

		D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr }; // (�������: ��������, �����������)
		ID3DBlob* errorPixelCode;
		res = D3DCompileFromFile(L"MiniTri.fx", nullptr /*Shader_Macros*/, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
		if (FAILED(res)) {
			if (errorPixelCode) {
				char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
				std::cout << compileErrors << std::endl;
				return ERROR_PBC;
			}
			else
				return MISSING_PS;
		}

		res = device->CreateVertexShader(
			vertexBC->GetBufferPointer(),
			vertexBC->GetBufferSize(),
			nullptr, &vertexShader);
		if (FAILED(res))
			return ERROR_CREATING_VS;
		res = device->CreatePixelShader(
			pixelBC->GetBufferPointer(),
			pixelBC->GetBufferSize(),
			nullptr, &pixelShader);
		if (FAILED(res))
			return ERROR_CREATING_PS;

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
		res = device->CreateInputLayout(
			inputElements,
			2, // ���������� ������� ���������
			vertexBC->GetBufferPointer(),
			vertexBC->GetBufferSize(),
			&layout);
		if (FAILED(res))
			return ERROR_CREATING_LAYOUT;

		D3D11_BUFFER_DESC dataBufDesc = {}; // ��������� �������� ������ ������
		dataBufDesc.Usage = D3D11_USAGE_DEFAULT; // ��������� ������ ��� ������ � ������ �� ������� ������������ ����������
		dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // �������� ������ � �������� ������ ������
		dataBufDesc.CPUAccessFlags = 0; // ������ � �� �� ���������
		dataBufDesc.MiscFlags = 0; // �������������� �����
		dataBufDesc.StructureByteStride = 0; // ������ ������� �������� � ��������� � ������ (������ ����?)
		dataBufDesc.ByteWidth = sizeof(XMFLOAT4) * parameters.numPoints; // ������ ������ � ������

		D3D11_SUBRESOURCE_DATA positionsData = {}; // ��������� ��� ������������� ����������
		positionsData.pSysMem = parameters.positions; // ������
		positionsData.SysMemPitch = 0; // ���������� (� ������) �� ������ ����� ������ �� ��������� ������
		positionsData.SysMemSlicePitch = 0; // ���������� (� ������) �� ������ ������ ������ ������� �� ���������� (��� ���������� �������)
		D3D11_SUBRESOURCE_DATA colorsData = {};
		colorsData.pSysMem = parameters.colors;
		colorsData.SysMemPitch = 0;
		colorsData.SysMemSlicePitch = 0;

		ID3D11Buffer* pb; // ����� �������
		ID3D11Buffer* cb; // ����� ������

		res = device->CreateBuffer(&dataBufDesc, &positionsData, &pb);
		if (FAILED(res))
			return ERROR_CREATING_POSBUF;
		res = device->CreateBuffer(&dataBufDesc, &colorsData, &cb);
		if (FAILED(res))
			return ERROR_CREATING_COLBUF;

		D3D11_BUFFER_DESC indexBufDesc = {};
		indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufDesc.CPUAccessFlags = 0;
		indexBufDesc.MiscFlags = 0;
		indexBufDesc.StructureByteStride = 0;
		indexBufDesc.ByteWidth = sizeof(int) * parameters.numIndeces;

		D3D11_SUBRESOURCE_DATA indexData = {};
		indexData.pSysMem = parameters.indeces;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		res = device->CreateBuffer(&indexBufDesc, &indexData, &indBuf);
		if (FAILED(res))
			return ERROR_CREATING_INDBUF;

		vBuffers[0] = pb;
		vBuffers[1] = cb;
		strides[0] = 16;
		strides[1] = 16;
		offsets[0] = 0;
		offsets[1] = 0;

		D3D11_BLEND_DESC blendStateDesc; // ���������� ����������
		blendStateDesc.AlphaToCoverageEnable = false;
		blendStateDesc.IndependentBlendEnable = false;
		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // �������� �� ��������� RGB � ���������� �������
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // �������� � RGB � ������� ������� ����������
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // ����������, ��� ������������� �������� SrcBlend � DestBlend
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // �������� �� ��������� ����� � ���������� �������
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // �������� �� ��������� ����� � ������� ������� ����������
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // ����������, ��� ������������� �������� SrcBlendAlpha � DestBlendAlpha
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		res = device->CreateBlendState(&blendStateDesc, &blend); 
		if (FAILED(res))
			return ERROR_CREATING_BLENDSTATE;

		blendFactor[0] = 0;
		blendFactor[1] = 0;
		blendFactor[2] = 0;
		blendFactor[3] = 0;
		sampleMask = 0xffffffff;


		D3D11_BUFFER_DESC constBufDesc = {};
		constBufDesc.Usage = D3D11_USAGE_DYNAMIC; // ������, ��������� ��� ��� GPU (������ ��� ������), ��� � ��� CPU (������ ��� ������)
		constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // �������� ������ � �������� ������������
		constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // ������ ��� ������
		constBufDesc.MiscFlags = 0;
		constBufDesc.StructureByteStride = 0; // ������ ������� �������� � ��������� ������
		constBufDesc.ByteWidth = sizeof(DirectX::SimpleMath::Matrix);
		res = device->CreateBuffer(&constBufDesc, nullptr, &constBuf);
		if (FAILED(res))
			return ERROR_CREATING_CONSTBUF;

		CD3D11_RASTERIZER_DESC rastDesc = {}; // ���������� �������������
		rastDesc.CullMode = D3D11_CULL_BACK; // ������������, ���������� � ��������� �����������, �� ������������
		// D3D11_CULL_NONE - ������ ������������
		// D3D11_CULL_FRONT - �� �������� ������������, ���������� �����
		// D3D11_CULL_BACK - �� �������� ������������, ���������� �����
		rastDesc.FillMode = D3D11_FILL_SOLID; // ����� ������� 
		// D3D11_FILL_SOLID - ����������
		// D3D11_FILL_WIREFRAME - ������ �����
		res = device->CreateRasterizerState(&rastDesc, &rastState);
		if (FAILED(res))
			return ERROR_CREATING_RASTSTATE;

		return SUCCESS;
	}
	return NOTHING_TO_DRAW;
}

void TriangleComponent::DestroyResourses() {
	//if (layout) layout->Release();
	if (vertexShader != nullptr)
		vertexShader->Release();
	if (pixelShader != nullptr)
		pixelShader->Release();
}

void TriangleComponent::Update(ID3D11DeviceContext* context, Camera* camera) {
	auto proj = DirectX::SimpleMath::Matrix::CreateTranslation(compPosition) * camera->ViewMatrix * camera->ProjectionMatrix; // ��������� ��������
	proj = proj.Transpose();

	D3D11_MAPPED_SUBRESOURCE subresourse = {};
	context->Map( // ��������� ��������� �� ������ � ������ ������� GPU � ����� �������
		constBuf,
		0,  // ����� ����������
		D3D11_MAP_WRITE_DISCARD, // ��������� ������� ��� ������
		0, // D3D11_MAP_FLAG_DO_NOT_WAIT
		&subresourse);

	memcpy(
		reinterpret_cast<float*>(subresourse.pData), // ����
		&proj, // ������
		sizeof(DirectX::SimpleMath::Matrix)); // ������� ����

	context->Unmap(constBuf, 0); // ������� ������ GPU
}

void TriangleComponent::Draw(ID3D11DeviceContext* context) {
	if (parameters.numIndeces != 0)
	{
		context->IASetInputLayout(layout);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �������������: �������� ������� ������� ������������
		// ������ ��� - ����� ������������� (STRIP), ����� ��������� ���������� �� �������� 0-1-2, 1-2-3, 2-3-4
		context->IASetIndexBuffer(indBuf, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(
			0, // ������ ����
			2, // ���������� �������
			vBuffers, // ������ ������
			strides, // ��� ������ ��� ������� ������
			offsets); // �������� �� ������ ��� ������� ������
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->OMSetBlendState(blend, blendFactor, sampleMask);
		context->VSSetConstantBuffers(0, 1, &constBuf);
		context->RSSetState(rastState);
		context->DrawIndexed(
			parameters.numIndeces, // ���������� �������������� �������� �� ������ ��������
			0, // ������ ������ ��� ���������
			0);// ��������, ����������� � ������� ������� ����� ������� ������� �� ������ ������
	}
}
