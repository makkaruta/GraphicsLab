#include "DisplayWin32.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY: // закрытие окна
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_SIZE: // изменение размера окна
		{
			std::cout << "Width: " << LOWORD(lparam) << ", Height: " << HIWORD(lparam) << std::endl;
			return 0;
		}
	case WM_KEYDOWN: // нажатие клавиши
		{
			// Если была нажата клавиша, необходимо отправить её в объект ввода для обработки
			std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;
			if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
			return 0;
		}
	case WM_KEYUP: // отжатие клавиши
		{
			// Если была отжата клавиша, необъодимо отправить это в объект ввода для прекращения обработки
			return 0;
		}
	default: // Все остальные сообщения передаются обработчику сообщений в системном классе
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}

void DisplayWin32::CreateDisplay() {
	applicationName = L"Game";

	// Настройка класса окна начальными значениями
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // перерисовка окна при изменении ширины или высоты области + уникальный контекст устройства для каждого окна
	wc.lpfnWndProc = WndProc; // указатель на оконную процедуру
	wc.cbClsExtra = 0; // число дополнительных байт после структуры
	wc.cbWndExtra = 0; // число дополнительных байт после экземпляра окна
	wc.hInstance = hInstance; // дескриптор приложения
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO); // дескриптор иконки приложения
	wc.hIconSm = wc.hIcon; // дескриптор мини иконки
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // дескриптор курсора
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)); // дескриптор фона
	wc.lpszMenuName = nullptr; // название меню
	wc.lpszClassName = applicationName; // название класса окна
	wc.cbSize = sizeof(WNDCLASSEX); // размер этой структуры в байтах

	RegisterClassEx(&wc); // Регистрация созданного класса окна
	screenWidth = 800;
	screenHeight = 800;
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) }; // Определение левого верхнего и правого нижнего углов прямоугольника
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE); // вычисление фактического размера окна
	hWnd = CreateWindowEx( // Создание окна с настройками и получение дескриптора
		WS_EX_APPWINDOW, // окно верхнего уровня на панели задач
		applicationName, // имя класса
		applicationName, // имя окна
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME, // имеет меню окна + область заголовка + кнопку сворачивания + рамку изменения размера
		posX, posY, // положение окна на экране
		windowRect.right - windowRect.left, // фактическая ширина окна
		windowRect.bottom - windowRect.top, // фактическая высота окна
		nullptr, // дескриптор родительского окна
		nullptr, // дексриптор меню
		hInstance, // дескриптор приложения
		nullptr); // указатель на параметры при создании

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd); // Переводит поток, который создал окно, в приоритетный режим
	SetFocus(hWnd); // Устанавливает фокус клавиатуры на окно
	ShowCursor(true);
};