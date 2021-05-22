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
	while (!isExitRequested) { // Цикл до сообщения о выходе от окна или пользователя
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) { // Неблокирующий ввод
			TranslateMessage(&msg); // Перевод нажатия клавиш в символы
			DispatchMessage(&msg); // Обработка сообщения
		}
		if (msg.message == WM_QUIT) { // Если было получено сообщение о выходе, в цикл больше входить не надо
			isExitRequested = true;
		}
		test.Draw();
	}
	test.DestroyResources();
	return 0;
}
