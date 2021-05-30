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
		ID3DBlob* errorVertexCode; // сообщения об ошибках
		res = D3DCompileFromFile( // Компиляция вертексного шейдера из файла
			L"MiniTri.fx", // название файла с шейдером
			nullptr, // макросы шейдера
			nullptr, // include в файле с шейдерами
			"VSMain", // точка входа
			"vs_5_0", // цель шейдера (?)
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // флаги шейдера (добавление информации об отладочном файле + пропуск оптимизации при отладке)
			0, // флаги эффекта 
			&vertexBC,
			&errorVertexCode);
		if (FAILED(res)) {
			if (errorVertexCode) { // Если шейдер не был скомпилирован, что-то должно быть написано в сообщении об ошибке
				char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
				std::cout << compileErrors << std::endl;
				return ERROR_VBC;
			}
			else // Если в сообщении об ошибке ничего не было, значит, он просто не смог найти сам файл шейдера
				return MISSING_VS;
		}

		D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr }; // (попарно: название, определение)
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

		D3D11_INPUT_ELEMENT_DESC inputElements[] = { // Описание формата данных о точках
			D3D11_INPUT_ELEMENT_DESC {
				"POSITION",
				0, // семантический индекс элемента
				DXGI_FORMAT_R32G32B32A32_FLOAT, // почему такая константа на позицию?
				0, // индекс слота ввода
				0, // смещение от начала
				D3D11_INPUT_PER_VERTEX_DATA, // класс входных данных - данные для каждой вершины
				0}, // кол-во эл-тов для рисования с исп. одних и тех же данных перед продвижением в буфере на один элемент
				// Это значение должно быть 0 для элемента, который содержит данные для каждой вершины
			D3D11_INPUT_ELEMENT_DESC {
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT, // RGBA по 32 бита на канал
				1,
				D3D11_APPEND_ALIGNED_ELEMENT, // смещение: определить текущий элемент сразу после предыдущего
				D3D11_INPUT_PER_VERTEX_DATA,
				0}
		};
		res = device->CreateInputLayout(
			inputElements,
			2, // количество входных элементов
			vertexBC->GetBufferPointer(),
			vertexBC->GetBufferSize(),
			&layout);
		if (FAILED(res))
			return ERROR_CREATING_LAYOUT;

		D3D11_BUFFER_DESC dataBufDesc = {}; // структура описания буфера данных
		dataBufDesc.Usage = D3D11_USAGE_DEFAULT; // требуется доступ для чтения и записи со стороны графического процессора
		dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // привязка буфера в качестве буфера вершин
		dataBufDesc.CPUAccessFlags = 0; // доступ к ЦП не требуется
		dataBufDesc.MiscFlags = 0; // дополнительные флаги
		dataBufDesc.StructureByteStride = 0; // размер каждого элемента в структуре в байтах (почему ноль?)
		dataBufDesc.ByteWidth = sizeof(XMFLOAT4) * parameters.numPoints; // размер буфера в байтах

		D3D11_SUBRESOURCE_DATA positionsData = {}; // настройки для инициализации подресурса
		positionsData.pSysMem = parameters.positions; // данные
		positionsData.SysMemPitch = 0; // расстояние (в байтах) от начала одной строки до следующей строки
		positionsData.SysMemSlicePitch = 0; // расстояние (в байтах) от начала одного уровня глубины до следующего (для трехмерных текстур)
		D3D11_SUBRESOURCE_DATA colorsData = {};
		colorsData.pSysMem = parameters.colors;
		colorsData.SysMemPitch = 0;
		colorsData.SysMemSlicePitch = 0;

		ID3D11Buffer* pb; // буфер позиций
		ID3D11Buffer* cb; // буфер цветов

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

		D3D11_BLEND_DESC blendStateDesc; // дескриптор смешивания
		blendStateDesc.AlphaToCoverageEnable = false;
		blendStateDesc.IndependentBlendEnable = false;
		blendStateDesc.RenderTarget[0].BlendEnable = true;
		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // операция со значением RGB в пиксельном шейдере
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // операция с RGB в целевом объекте рендеринга
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // определяет, как комбинировать операции SrcBlend и DestBlend
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // операция со значением альфа в пиксельном шейдере
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // операция со значением альфа в целевом объекте рендеринга
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // определяет, как комбинировать операции SrcBlendAlpha и DestBlendAlpha
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
		constBufDesc.Usage = D3D11_USAGE_DYNAMIC; // ресурс, доступный как для GPU (только для чтения), так и для CPU (только для записи)
		constBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // привязка буфера в качестве константного
		constBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // доступ для записи
		constBufDesc.MiscFlags = 0;
		constBufDesc.StructureByteStride = 0; // размер каждого элемента в структуре буфера
		constBufDesc.ByteWidth = sizeof(DirectX::SimpleMath::Matrix);
		res = device->CreateBuffer(&constBufDesc, nullptr, &constBuf);
		if (FAILED(res))
			return ERROR_CREATING_CONSTBUF;

		CD3D11_RASTERIZER_DESC rastDesc = {}; // дескриптор растеризатора
		rastDesc.CullMode = D3D11_CULL_BACK; // треугольники, обращенные в указанном направлении, не отображаются
		// D3D11_CULL_NONE - всегда отображаются
		// D3D11_CULL_FRONT - не рисовать треугольники, повернутые лицом
		// D3D11_CULL_BACK - не рисовать треугольники, повернутые задом
		rastDesc.FillMode = D3D11_FILL_SOLID; // режим заливки 
		// D3D11_FILL_SOLID - заполнение
		// D3D11_FILL_WIREFRAME - только линии
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
	auto proj = DirectX::SimpleMath::Matrix::CreateTranslation(compPosition) * camera->ViewMatrix * camera->ProjectionMatrix; // получение проекции
	proj = proj.Transpose();

	D3D11_MAPPED_SUBRESOURCE subresourse = {};
	context->Map( // получение указателя на ресурс и запрет доступа GPU к этому ресурсу
		constBuf,
		0,  // номер подресурса
		D3D11_MAP_WRITE_DISCARD, // получение ресурса для записи
		0, // D3D11_MAP_FLAG_DO_NOT_WAIT
		&subresourse);

	memcpy(
		reinterpret_cast<float*>(subresourse.pData), // куда
		&proj, // откуда
		sizeof(DirectX::SimpleMath::Matrix)); // сколько байт

	context->Unmap(constBuf, 0); // вернуть доступ GPU
}

void TriangleComponent::Draw(ID3D11DeviceContext* context) {
	if (parameters.numIndeces != 0)
	{
		context->IASetInputLayout(layout);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // список треугольников: задаются вершины каждого треугольника
		// другой вид - лента треугольников (STRIP), когда отрисовка происходит по индексам 0-1-2, 1-2-3, 2-3-4
		context->IASetIndexBuffer(indBuf, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(
			0, // первый слот
			2, // количество буферов
			vBuffers, // буферы вершин
			strides, // шаг вершин для каждого буфера
			offsets); // смещение от начала для каждого буфера
		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);
		context->OMSetBlendState(blend, blendFactor, sampleMask);
		context->VSSetConstantBuffers(0, 1, &constBuf);
		context->RSSetState(rastState);
		context->DrawIndexed(
			parameters.numIndeces, // количество отрисовываемых индексов из буфера индексов
			0, // первый индекс для отрисовки
			0);// значение, добавляемое к каждому индексу перед чтением вершины из буфера вершин
	}
}
