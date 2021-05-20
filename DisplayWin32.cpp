#include "DisplayWin32.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY: // �������� ����
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	case WM_SIZE: // ��������� ������� ����
		{
			std::cout << "Width: " << LOWORD(lparam) << ", Height: " << HIWORD(lparam) << std::endl;
			return 0;
		}
	case WM_KEYDOWN: // ������� �������
		{
			// ���� ���� ������ �������, ���������� ��������� � � ������ ����� ��� ���������
			std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;
			if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
			return 0;
		}
	case WM_KEYUP: // ������� �������
		{
			// ���� ���� ������ �������, ���������� ��������� ��� � ������ ����� ��� ����������� ���������
			return 0;
		}
	default: // ��� ��������� ��������� ���������� ����������� ��������� � ��������� ������
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}

void DisplayWin32::CreateDisplay() {
	applicationName = L"Game";

	// ��������� ������ ���� ���������� ����������
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // ����������� ���� ��� ��������� ������ ��� ������ ������� + ���������� �������� ���������� ��� ������� ����
	wc.lpfnWndProc = WndProc; // ��������� �� ������� ���������
	wc.cbClsExtra = 0; // ����� �������������� ���� ����� ���������
	wc.cbWndExtra = 0; // ����� �������������� ���� ����� ���������� ����
	wc.hInstance = hInstance; // ���������� ����������
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO); // ���������� ������ ����������
	wc.hIconSm = wc.hIcon; // ���������� ���� ������
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // ���������� �������
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)); // ���������� ����
	wc.lpszMenuName = nullptr; // �������� ����
	wc.lpszClassName = applicationName; // �������� ������ ����
	wc.cbSize = sizeof(WNDCLASSEX); // ������ ���� ��������� � ������

	RegisterClassEx(&wc); // ����������� ���������� ������ ����
	screenWidth = 800;
	screenHeight = 800;
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) }; // ����������� ������ �������� � ������� ������� ����� ��������������
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE); // ���������� ������������ ������� ����
	hWnd = CreateWindowEx( // �������� ���� � ����������� � ��������� �����������
		WS_EX_APPWINDOW, // ���� �������� ������ �� ������ �����
		applicationName, // ��� ������
		applicationName, // ��� ����
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME, // ����� ���� ���� + ������� ��������� + ������ ������������ + ����� ��������� �������
		posX, posY, // ��������� ���� �� ������
		windowRect.right - windowRect.left, // ����������� ������ ����
		windowRect.bottom - windowRect.top, // ����������� ������ ����
		nullptr, // ���������� ������������� ����
		nullptr, // ���������� ����
		hInstance, // ���������� ����������
		nullptr); // ��������� �� ��������� ��� ��������

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd); // ��������� �����, ������� ������ ����, � ������������ �����
	SetFocus(hWnd); // ������������� ����� ���������� �� ����
	ShowCursor(true);
};