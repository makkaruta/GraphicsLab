#pragma once

#include "DisplayWin32.h"
#include "GameComponent.h"
#include "TriangleComponent.h"
#include "windows.h"
#include <vector>
#include <wrl.h>
#include <chrono>
/*#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")*/

#define ZCHECK(exp) if(FAILED(exp)) { printf("Check failed at file: %s at line %i", __FILE__, __LINE__); return 0; }

class Game {
private:
	ID3D11Debug* debug; // Интерфейс отладки управляет настройками отладки и проверяет состояние конвейера
	std::chrono::time_point<std::chrono::steady_clock> PrevTime;
	float totalTime = 0;
	unsigned int frameCount = 0;
public:
	D3D11_VIEWPORT viewport; // размеры области просмотра

	std::vector <GameComponent*> Components; // вектор компонент (акторов)
	ID3D11DeviceContext* context; // структура, содержащая сведения об атрибутах рисования устройства, таких как экран или принтер
	// Все вызовы рисования выполняются через объект контекста устройства, который инкапсулирует интерфейсы.
	IDXGISwapChain* swapChain; // свапчейн (цепочка подкачки)
	ID3D11RenderTargetView* rtv; // целевой объект рендеринга
	ID3DUserDefinedAnnotation* annotation; // Описание концептуальных разделов и маркеров в потоке кода приложения
	// Позволяет переходить к интересующим частям временной шкалы или понимать, какой набор вызовов Direct3D производится какими разделами кода приложения
	DisplayWin32 Display;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	int Run();
	void Initialize();
	void DestroyResources();
	void Draw();
};