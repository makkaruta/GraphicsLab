#include "pch.h"
#include "TriangleComponent.h"

TriangleComponent::TriangleComponent() {
	parameters.positions = nullptr;
	parameters.colors = nullptr;
	parameters.texcoords = nullptr;
	parameters.indeces = nullptr;
	parameters.numPoints = 0;
	parameters.numIndeces = 0;
	parameters.textureFileName = nullptr;
	normals = nullptr;
	compPosition = DirectX::SimpleMath::Vector3(0, 0 ,0);

	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	for (int i = 0; i < 4; i++)
		vBuffers[i] = nullptr;
	indBuf = nullptr;
	blend = nullptr;
	constBuf = nullptr;
	lightBuf = nullptr;
	rastState = nullptr;
	factory = nullptr;
	texture = nullptr;
	textureView = nullptr;
	sampler = nullptr;

	sampleMask = 0;
	for (int i = 0; i < 4; i++)
		strides[i] = 0;
	for (int i = 0; i < 4; i++)
		offsets[i] = 0;
	for (int i = 0; i < 4; i++)
		blendFactor[i] = 0;
}

TriangleComponent::TriangleComponent(TriangleComponentParameters param) {
	parameters.positions = param.positions;
	parameters.colors = param.colors;
	parameters.texcoords = param.texcoords;
	parameters.indeces = param.indeces;
	parameters.numPoints = param.numPoints;
	parameters.numIndeces = param.numIndeces;
	parameters.textureFileName = param.textureFileName;
	compPosition = param.compPosition;
	normals = new DirectX::SimpleMath::Vector4[parameters.numPoints];
	NormalsCalc(); // ������� ������� ��� ���������

	vertexBC = nullptr;
	pixelBC = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	layout = nullptr;
	for (int i = 0; i < 4; i++)
		vBuffers[i] = nullptr;
	indBuf = nullptr;
	blend = nullptr;
	constBuf = nullptr;
	lightBuf = nullptr;
	rastState = nullptr;
	factory = nullptr;
	texture = nullptr;
	textureView = nullptr;
	sampler = nullptr;

	sampleMask = 0;
	for (int i = 0; i < 4; i++)
		strides[i] = 0;
	for (int i = 0; i < 4; i++)
		offsets[i] = 0;
	for (int i = 0; i < 4; i++)
		blendFactor[i] = 0;

	// ��� ���������� �������
	CoInitialize(NULL); // �������������� ���������� COM � ������� ������
	CoCreateInstance( // ������� COM ������
		CLSID_WICImagingFactory2, // CLSID, ��������� � ������� � �����, ������� ����� �������������� ��� �������� �������
		NULL, // ���� NULL, ���������, ��� ������ �� ��������� ��� ����� ��������
		CLSCTX_INPROC_SERVER, // ����������� � ��� �� ��������, ��� � ���������� �������
		IID_PPV_ARGS(&factory));
}

int TriangleComponent::PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) {
	if (parameters.numIndeces == 0)
		return NOTHING_TO_DRAW;

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
			return ERROR_VERTEX_BC;
		}
		else // ���� � ��������� �� ������ ������ �� ����, ������, �� ������ �� ���� ����� ��� ���� �������
			return MISSING_VERTEX_SHADER;
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "HASTEXTURE", "1", nullptr, nullptr }; // (�������: ��������, �����������)
	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(L"MiniTri.fx", (parameters.textureFileName == nullptr) ? nullptr : Shader_Macros, nullptr, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
	if (FAILED(res)) {
		if (errorPixelCode) {
			char* compileErrors = (char*)(errorPixelCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
			return ERROR_PIXEL_BC;
		}
		else
			return MISSING_PIXEL_SHADER;
	}

	res = device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);
	if (FAILED(res))
		return ERROR_CREATING_VERTEX_SHADER;
	res = device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);
	if (FAILED(res))
		return ERROR_CREATING_PIXEL_SHADER;

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
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			2,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			3,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};
	res = device->CreateInputLayout(
		inputElements,
		4, // ���������� ������� ���������
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
	D3D11_SUBRESOURCE_DATA texcoordsData = {};
	texcoordsData.pSysMem = parameters.texcoords;
	texcoordsData.SysMemPitch = 0;
	texcoordsData.SysMemSlicePitch = 0;
	D3D11_SUBRESOURCE_DATA normalsData = {};
	normalsData.pSysMem = normals;
	normalsData.SysMemPitch = 0;
	normalsData.SysMemSlicePitch = 0;

	ID3D11Buffer* pb; // ����� �������
	ID3D11Buffer* cb; // ����� ������
	ID3D11Buffer* tb; // ����� ��������� ��� ��������
	ID3D11Buffer* nb; // ����� ��������

	res = device->CreateBuffer(&dataBufDesc, &positionsData, &pb);
	if (FAILED(res))
		return ERROR_CREATING_POSBUF;
	res = device->CreateBuffer(&dataBufDesc, &colorsData, &cb);
	if (FAILED(res))
		return ERROR_CREATING_COLBUF;
	res = device->CreateBuffer(&dataBufDesc, &texcoordsData, &tb);
	if (FAILED(res))
		return ERROR_CREATING_TEXBUF;
	res = device->CreateBuffer(&dataBufDesc, &normalsData, &nb);
	if (FAILED(res))
		return ERROR_CREATING_NORMBUF;

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
	vBuffers[2] = tb;
	vBuffers[3] = nb;
	strides[0] = 16;
	strides[1] = 16;
	strides[2] = 16;
	strides[3] = 16;
	offsets[0] = 0;
	offsets[1] = 0;
	offsets[2] = 0;
	offsets[3] = 0;

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
	constBufDesc.ByteWidth = sizeof(constData);
	res = device->CreateBuffer(&constBufDesc, nullptr, &constBuf);
	if (FAILED(res))
		return ERROR_CREATING_CONSTBUF;

	D3D11_BUFFER_DESC lightBufDesc = {};
	lightBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufDesc.MiscFlags = 0;
	lightBufDesc.StructureByteStride = 0;
	lightBufDesc.ByteWidth = sizeof(lightData);
	res = device->CreateBuffer(&lightBufDesc, nullptr, &lightBuf);
	if (FAILED(res))
		return ERROR_CREATING_LIGHTTBUF;

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

	if (parameters.textureFileName != nullptr)
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; // ���������� �������� ��� ��������� [0,0, 1,0] ��������������� ��� ����� �������� 0,0 ��� 1,0 ��������������. 
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // ��� ����������
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.MaxLOD = INT_MAX; // ������� ����� ��������� MIP-����� ��� ����������� �������
		res = device->CreateSamplerState(&samplerDesc, &sampler);
		if (FAILED(res))
			return ERROR_CREATING_SAMPLER_STATE;
	}

	return SUCCESS;
}

int TriangleComponent::LoadTextureFromFile(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11DeviceContext* context, bool generateMips, bool useSrgb, UINT frameIndex) {
	if (factory == nullptr)
		return ERROR_CREATING_COM_OBJ;
	if (vertexShader != nullptr)
		return ERROR_LOAD_TEXTURE_ORDER;
	HRESULT res;
	IWICBitmapDecoder* decoder;
	if (parameters.textureFileName == nullptr)
		return ERROR_TEXTURE_FILE_MISSING;
	res = factory->CreateDecoderFromFilename(parameters.textureFileName, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
	if (FAILED(res))
		return ERROR_CREATING_DECODER;
	IWICBitmapFrameDecode* frame;
	res = decoder->GetFrame(frameIndex, &frame); // �������� ���� �� ����� (��� ��������� ������ 0)
	if (FAILED(res))
		return ERROR_GET_FRAME;
	IWICFormatConverter* converter;
	res = factory->CreateFormatConverter(&converter);
	if (FAILED(res))
		return ERROR_CREATING_CONVERTER;
	res = converter->Initialize(
		frame, // ������� ��������� ����������� ��� ��������������
		GUID_WICPixelFormat32bppPRGBA, // 32 ���� �� �����, RGBA
		WICBitmapDitherTypeNone, // ��� ���������
		nullptr, // �������, ������������ ��� ��������������
		0, // �����-�����
		WICBitmapPaletteTypeCustom); // ������������ ������������� �������
	if (FAILED(res))
		return ERROR_INITIALIZE_CONVERTER;
	UINT width, height;
	converter->GetSize(&width, &height);
	UINT stride = width * 4; // 4 ������
	UINT bufSize = stride * height;
	BYTE* buf = new BYTE[bufSize];
	res = converter->CopyPixels(nullptr, stride, bufSize, buf);
	if (FAILED(res))
		return ERROR_COPY_PIXELS;

	D3D11_TEXTURE2D_DESC desc = {}; // ���������� ��������
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.Format = useSrgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.MipLevels = generateMips ? 0 : 1; // ��� ������������� MIP ������� �������� 0, ��� ��� ��� ���� �������������
	desc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	desc.SampleDesc.Count = 1; // ���������� ������������� �� �������
	desc.SampleDesc.Quality = 0; // ������� �������� �����������

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = buf;
	data.SysMemPitch = stride;
	data.SysMemSlicePitch = bufSize;

	res = device->CreateTexture2D(&desc, generateMips ? nullptr : &data, &texture);
	if (FAILED(res))
		return ERROR_CREATING_TEXTURE;
	res = device->CreateShaderResourceView(texture, nullptr, &textureView);
	if (FAILED(res))
		return ERROR_CREATING_SHADER_RV;

	if (generateMips) {
		context->UpdateSubresource(texture, 0, nullptr, buf, stride, bufSize);
		context->GenerateMips(textureView);
	}

	decoder->Release();
	frame->Release();
	converter->Release();
	delete[] buf;

	return SUCCESS;
}

void TriangleComponent::DestroyResourses() {
	if (vertexShader != nullptr)
		vertexShader->Release();
	if (pixelShader != nullptr)
		pixelShader->Release();
	if (layout != nullptr)
		layout->Release();
	for (int i = 0; i < 4; i++)
	{
		if (vBuffers[i] != nullptr)
			vBuffers[i]->Release();
	}
	if (indBuf != nullptr)
		indBuf->Release();
	if (blend != nullptr)
		blend->Release();
	if (constBuf != nullptr)
		constBuf->Release();
	if (lightBuf != nullptr)
		lightBuf->Release();
	if (rastState != nullptr)
		rastState->Release();
	if (factory != nullptr)
		factory->Release();
	if (texture != nullptr)
		texture->Release();
	if (textureView != nullptr)
		textureView->Release();
	if (sampler != nullptr)
		sampler->Release();
}

void TriangleComponent::Update(ID3D11DeviceContext* context, Camera* camera) {

	// ���������� ������������ ������
	constData data;
	data.WorldViewProj = DirectX::SimpleMath::Matrix::CreateTranslation(compPosition) * camera->ViewMatrix * camera->ProjectionMatrix; // ��������� ��������
	data.WorldViewProj = data.WorldViewProj.Transpose();
	data.World = DirectX::SimpleMath::Matrix::CreateTranslation(parameters.compPosition);
	D3D11_MAPPED_SUBRESOURCE subresourse = {};
	context->Map( // ��������� ��������� �� ������ � ������ ������� GPU � ����� �������
		constBuf,
		0,  // ����� ����������
		D3D11_MAP_WRITE_DISCARD, // ��������� ������� ��� ������
		0, // D3D11_MAP_FLAG_DO_NOT_WAIT
		&subresourse);
	memcpy(
		reinterpret_cast<float*>(subresourse.pData), // ����
		&data, // ������
		sizeof(constData)); // ������� ����
	context->Unmap(constBuf, 0); // ������� ������ GPU

	// ���������� ������������ ������ ��� �����
	lightData light;
	light.ViewerPos = DirectX::SimpleMath::Vector4(camera->position.x, camera->position.y, camera->position.z, 1.0f);
	light.Direction = DirectX::SimpleMath::Vector4(-10.0f, 10.0f, -10.0f, 1.0f);;
	light.Color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	D3D11_MAPPED_SUBRESOURCE subresourse2 = {};
	context->Map( // ��������� ��������� �� ������ � ������ ������� GPU � ����� �������
		lightBuf,
		0,  // ����� ����������
		D3D11_MAP_WRITE_DISCARD, // ��������� ������� ��� ������
		0, // D3D11_MAP_FLAG_DO_NOT_WAIT
		&subresourse2);
	memcpy(
		reinterpret_cast<float*>(subresourse2.pData), // ����
		&light, // ������
		sizeof(lightData)); // ������� ����
	context->Unmap(lightBuf, 0); // ������� ������ GPU
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
			4, // ���������� �������
			vBuffers, // ������ ������
			strides, // ��� ������ ��� ������� ������
			offsets); // �������� �� ������ ��� ������� ������
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->OMSetBlendState(blend, blendFactor, sampleMask);
		context->VSSetConstantBuffers(0, 1, &constBuf); 
		context->PSSetConstantBuffers(1, 1, &lightBuf);
		if (parameters.textureFileName != nullptr)
		{
			context->PSSetShaderResources(0, 1, &textureView);
			context->PSSetSamplers(0, 1, &sampler);
		}
		context->RSSetState(rastState);
		context->DrawIndexed(
			parameters.numIndeces, // ���������� �������������� �������� �� ������ ��������
			0, // ������ ������ ��� ���������
			0);// ��������, ����������� � ������� ������� ����� ������� ������� �� ������ ������
	}
}

void TriangleComponent::NormalsCalc() {
	int ind_a, ind_b, ind_c;
	DirectX::SimpleMath::Vector4 a, b, c, p, q, norm;
	for (int i = 0; i < parameters.numIndeces; i += 3)
	{
		ind_a = parameters.indeces[i];
		ind_b = parameters.indeces[i+1];
		ind_c = parameters.indeces[i+2];

		a = parameters.positions[ind_a];
		b = parameters.positions[ind_b];
		c = parameters.positions[ind_c];

		p = a - b;
		q = c - b;

		norm.x = p.y * q.z - p.z * q.y;
		norm.y = p.z * q.x - p.x * q.z; 
		norm.z = p.x * q.y - p.y * q.x;
		norm.w = 1.0f;

		normals[ind_a] = norm;
		normals[ind_b] = norm;
		normals[ind_c] = norm;
	}; 
}
