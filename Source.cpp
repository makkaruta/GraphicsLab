#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <iostream>
#include "Game.h"

int main() {
	Game test;
	int res;
	res = test.Run();
	if (res == 1)
		std::cout << "Error";
	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) { // ���� �� ��������� � ������ �� ���� ��� ������������
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // ������������� ����
			TranslateMessage(&msg); // ������� ������� ������ � �������
			DispatchMessage(&msg); // ��������� ���������
		}
		if (msg.message == WM_QUIT) { // ���� ���� �������� ��������� � ������, � ���� ������ ������� �� ����
			isExitRequested = true;
		}
		test.Draw();
	}
	test.DestroyResources();
	return 0;
}
