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

	HRESULT res; // информация о результате вызова (флаги статуса)

	DXGI_SWAP_CHAIN_DESC swapDesc = {}; // дескриптор свапчейна (структура с настройками)
	swapDesc.BufferCount = 2; // количество буферов
	swapDesc.BufferDesc.Width = Display.get_screenWidth(); // дескриптор буфера: ширина буфера
	swapDesc.BufferDesc.Height = Display.get_screenHeight(); // дескриптор буфера: высота буфера
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // дескриптор буфера: формат RGBA, где на каждый канал по 8 бит
	swapDesc.BufferDesc.RefreshRate.Numerator = 60; // частота обновления в герцах: верхняя часть рационального числа (дроби)
	swapDesc.BufferDesc.RefreshRate.Denominator = 1; // частота обновления в герцах: нижняя часть дроби
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // описание режима рисования строк развертки (может быть прогрессивным или чересстрочным)
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // флаги, указывающие, как изображение растягивается для соответствия разрешению данного монитора 
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // описывает использование параметры доступа к ЦП для заднего буфера (рендер на экран)
	swapDesc.OutputWindow = Display.get_hWnd(); // дескриптор окна вывода
	swapDesc.Windowed = true; 
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // параметры обработки пикселей после вызова свапчейна (отбрасывает содержимое заднего буфера после свапчейна)
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // флаги: разрешить приложению переключать режимы из оконного в полноэкранный (разрешение будет изменено в соответствии с размерами окна приложения)
	swapDesc.SampleDesc.Count = 1; // количество мультисэмплов на пиксель (для сглаживания)
	swapDesc.SampleDesc.Quality = 0; // уровень качества изображения

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 }; // набор функций Direct3D
	res = D3D11CreateDeviceAndSwapChain(
		nullptr, // видеоадаптер (по умолчанию)
		D3D_DRIVER_TYPE_HARDWARE, // функции Direct3D будет реализовывать аппаратный драйвер 
		nullptr, // дескриптор программного растеризатора 
		D3D11_CREATE_DEVICE_DEBUG, // создает устройство, поддерживающее уровень отладки
		featureLevel, // порядок уровней функций
		1, // количество элементов в уровне функций
		D3D11_SDK_VERSION, // версия SDK
		&swapDesc, // дескриптор свапчейна
		&swapChain,
		&device,
		nullptr, // возвращает указатель, который представляет первый элемент в массиве уровней функций (null, если не нужно определять)
		&context); // возвращает адрес указателя на объект, представляющий контекст устройства 
	ZCHECK(res); //проверка что создалось успешно

	ID3D11Texture2D* backTex; // Интерфейс 2D-текстуры (управляет данными текселей (минимальная единица текстуры трёхмерного объекта))
	res = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backTex);	ZCHECK(res); // Доступ к одному из задних буферов свапчейна
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);			ZCHECK(res); // Создание представления целевого объекта рендеринга

	context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)&annotation); // Запрос интерфейса аннотации
	device->QueryInterface(IID_ID3D11Debug, (void**)&debug); // Запрос интерфейса отладки

	ID3DBlob* vertexBC; // вертекс байт код, результат компиляции текстового файла с вертексным шейдером
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
		}
		else // Если в сообщении об ошибке ничего не было, значит, он просто не смог найти сам файл шейдера
		{
			MessageBox(Display.get_hWnd(), L"MiniTri.fx", L"Missing Shader File", MB_OK);
		}
		return 0;
	}

	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr }; // (попарно: название, определение)
	ID3DBlob* pixelBC; // пиксель байт код, результат компиляции текстового файла с пиксельным шейдером
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

	ID3D11InputLayout* layout; // определение того, как передавать данные вершин, которые размещены в памяти, на этап сборщика ввода графического конвейера
	device->CreateInputLayout(
		inputElements,
		2, // количество входных элементов
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

	D3D11_BUFFER_DESC dataBufDesc = {}; // структура описания буфера данных
	dataBufDesc.Usage = D3D11_USAGE_DEFAULT; // требуется доступ для чтения и записи со стороны графического процессора
	dataBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // привязка буфера в качестве буфера вершин
	dataBufDesc.CPUAccessFlags = 0; // доступ к ЦП не требуется
	dataBufDesc.MiscFlags = 0; // дополнительные флаги
	dataBufDesc.StructureByteStride = 0; // размер каждого элемента в структуре в байтах (почему ноль?)
	dataBufDesc.ByteWidth = sizeof(XMFLOAT4) * std::size(positions); // размер буфера в байтах

	D3D11_SUBRESOURCE_DATA positionsData = {}; // настройки для инициализации подресурса
	positionsData.pSysMem = positions; // данные
	positionsData.SysMemPitch = 0; // расстояние (в байтах) от начала одной строки до следующей строки
	positionsData.SysMemSlicePitch = 0; // расстояние (в байтах) от начала одного уровня глубины до следующего (для трехмерных текстур)
	D3D11_SUBRESOURCE_DATA colorsData = {};
	colorsData.pSysMem = colors;
	colorsData.SysMemPitch = 0;
	colorsData.SysMemSlicePitch = 0;

	ID3D11Buffer* pb; // буфер позиций
	ID3D11Buffer* cb; // буфер цветов

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
	UINT strides[] = { 16, 16 }; // шаг вершин для каждого буфера
	UINT offsets[] = { 0, 0 }; // смещение от начала для каждого буфера

	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // список треугольников: задаются вершины каждого треугольника
	// другой вид - лента треугольников, когда отрисовка происходит по индексам 0-1-2, 1-2-3, 2-3-4
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(
		0, // первый слот
		2, // количество буферов
		vBuffers, // буферы вершин
		strides, 
		offsets); 
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	CD3D11_RASTERIZER_DESC rastDesc = {}; // дескриптор растеризатора
	rastDesc.CullMode = D3D11_CULL_NONE; // треугольники, обращенные в указанном направлении, не отображаются (всегда отображаются)
	rastDesc.FillMode = D3D11_FILL_SOLID; // режим заливки (заполнение)
	ID3D11RasterizerState* rastState;
	res = device->CreateRasterizerState(&rastDesc, &rastState); ZCHECK(res);
	context->RSSetState(rastState);

	D3D11_VIEWPORT viewport = {}; // размеры области просмотра
	viewport.Width = static_cast<float>(Display.get_screenWidth());
	viewport.Height = static_cast<float>(Display.get_screenHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport); // первый параметр - количество окон
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
	if (totalTime > 1.0f) { // подсчет кадров в секунду
		float fps = frameCount / totalTime;
		totalTime = 0.0f;
		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(Display.get_hWnd(), text);
		frameCount = 0;
	}

	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	context->OMSetRenderTargets(1, &rtv, nullptr); // привязка рендер таргета к заднему буферу, последний параметр - глубина привязки
	context->ClearRenderTargetView(rtv, color);
	annotation->BeginEvent(L"BeginDraw");
	context->DrawIndexed(
		6, // количество отрисовываемых индексов из буфера индексов
		0, // первый индекс для отрисовки
		0);// значение, добавляемое к каждому индексу перед чтением вершины из буфера вершин
	annotation->EndEvent();
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // замена переднего буфера на задний после отрисовки в задний
}