//#include "pch.h"
#include "InputDevice.h"
#include <iostream>
#include "Game.h"


using namespace DirectX::SimpleMath;

InputDevice::InputDevice() {

}

void InputDevice::Initialize(DisplayWin32 Display) {
	keys = new std::unordered_set<Keys>();

	RAWINPUTDEVICE RID[2];

	RID[0].usUsagePage = 0x01;
	RID[0].usUsage = 0x02;
	RID[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	RID[0].hwndTarget = Display.get_hWnd();

	RID[1].usUsagePage = 0x01;
	RID[1].usUsage = 0x06;
	RID[1].dwFlags = 0;   // adds HID keyboard and also ignores legacy keyboard messages
	RID[1].hwndTarget = Display.get_hWnd();

	if (RegisterRawInputDevices(RID, 2, sizeof(RID[0])) == FALSE)
	{
		auto errorCode = GetLastError();
		std::cout << "ERROR: " << errorCode << std::endl;
	}
}

InputDevice::~InputDevice() {
	delete keys;
}

void InputDevice::OnKeyDown(KeyboardInputEventArgs args) {
	bool Break = args.Flags & 0x01;

	auto key = (Keys)args.VKey;

	if (args.MakeCode == 42) key = Keys::LeftShift;
	if (args.MakeCode == 54) key = Keys::RightShift;

	if (Break) {
		if (keys->count(key))	RemovePressedKey(key);
	}
	else {
		if (!keys->count(key))	AddPressedKey(key);
	}
}

void InputDevice::OnMouseMove(RawMouseEventArgs args, DisplayWin32 Display) {
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonDown))
		AddPressedKey(Keys::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonUp))
		RemovePressedKey(Keys::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonDown))
		AddPressedKey(Keys::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonUp))
		RemovePressedKey(Keys::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonDown))
		AddPressedKey(Keys::MiddleButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonUp))
		RemovePressedKey(Keys::MiddleButton);

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(Display.get_hWnd(), &p);

	MousePosition = Vector2(p.x, p.y);
	MouseOffset = Vector2(args.X, args.Y);
	MouseWheelDelta = args.WheelDelta;

	const MouseMoveEventArgs moveArgs = { MousePosition, MouseOffset, MouseWheelDelta };

	//printf(" Mouse: posX=%04.4f posY:%04.4f offsetX:%04.4f offsetY:%04.4f, wheelDelta=%04d \n",
	//	MousePosition.x,
	//	MousePosition.y,
	//	MouseOffset.x,
	//	MouseOffset.y,
	//	MouseWheelDelta);

}

void InputDevice::AddPressedKey(Keys key) {
	//if (!game->isActive) {
	//	return;
	//}
	keys->insert(key);
}

void InputDevice::RemovePressedKey(Keys key) {
	keys->erase(key);
}

bool InputDevice::IsKeyDown(Keys key) {
	return keys->count(key);
}

