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
	Components.push_back(new TriangleComponent(positions,colors,indeces));
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



	//D3D11_VIEWPORT viewport = {}; // размеры области просмотра
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
		if (result == 0)
			return 0;
	}



	return 1;
}

void Game::DestroyResources() {
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

	context->RSSetViewports(1, &viewport); // первый параметр - количество окон


	for (int i = 0; i < Components.size(); i++)
		Components[i]->Draw(context);

	annotation->EndEvent();
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0); // замена переднего буфера на задний после отрисовки в задний
}