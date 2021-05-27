#pragma once

#include "pch.h"

#include "DisplayWin32.h"
#include "Camera.h"
#include "GameComponent.h"
#include "TriangleComponent.h"

#define ERROR_DEV_SC 13
#define ERROR_SCBUF 14
#define ERROR_RTV 15

class Game {
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	DisplayWin32 Display;
	D3D11_VIEWPORT viewport; // размеры области просмотра
	std::vector <GameComponent*> Components; // вектор компонент (акторов)
	ID3D11DeviceContext* context; // структура, содержащая сведения об атрибутах рисования устройства, таких как экран или принтер
	// Все вызовы рисования выполняются через объект контекста устройства, который инкапсулирует интерфейсы.
	IDXGISwapChain* swapChain; // свапчейн (цепочка подкачки)
	ID3D11RenderTargetView* rtv; // целевой объект рендеринга
	ID3DUserDefinedAnnotation* annotation; // Описание концептуальных разделов и маркеров в потоке кода приложения
	// Позволяет переходить к интересующим частям временной шкалы или понимать, какой набор вызовов Direct3D производится какими разделами кода приложения
	ID3D11Debug* debug; // Интерфейс отладки управляет настройками отладки и проверяет состояние конвейера
	InputDevice inputDevice;
	Camera camera;

	std::chrono::time_point<std::chrono::steady_clock> prevTime;
	float totalTime = 0;
	unsigned int frameCount = 0;
	
	void Initialize();
	int PrepareResources();
	void DestroyResources();
	void PrepareFrame();
	void EndFrame();
	void Update();
	void Draw();
	void ErrorsOutput(int ErrorCode);

public:
	void Run();
};