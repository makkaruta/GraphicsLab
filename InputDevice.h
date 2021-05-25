#pragma once

#include "pch.h"

#include <unordered_set>
#include "Keys.h"

struct MouseMoveEventArgs
{
	DirectX::SimpleMath::Vector2 Position;
	DirectX::SimpleMath::Vector2 Offset;
	int WheelDelta;
};

struct KeyboardInputEventArgs {
	UINT   Message;
	USHORT VKey;
	USHORT MakeCode;
	USHORT Flags; 
	// 0 ���: �������� �� ������� ����������� ��������, �������� ������� ��������� ALT � CTRL. 1 - ����������� ����; � ��������� ������ - 0.
	// 1-4 ����: ���������������
	// 5 ���: ��� ��������� (������ 0 ��� WM_KEYDOWN � WM_KEYUP)
	// 6 ���: 
	// ��� WM_KEYDOWN - ���������� �������� ���������. 1 - ������� ��� ���� ������ �� �������� ���������; 0 - ������� ������ ��� ������ (�� ����� ������).
	// ��� WM_KEYUP - ������ 1.
	// 7 ���:
	// ��� WM_KEYDOWN - ������ 0.
	// ��� WM_KEYUP - ������ 1.
};

struct RawMouseEventArgs
{
	int X;
	int Y;
	SHORT WheelDelta;
	USHORT ButtonFlags;
};

enum class MouseButtonFlags
{
	LeftButtonDown = 1, // 0x0001
	RightButtonDown = 2, // 0x0002
	MiddleButtonDown = 16, // 0x0010
	FirstXButton = 32, // 0x0020
	SecondXButton = 64, //0x0040

	None = 0,
};

class InputDevice{
private:
	HWND hWnd;
	std::unordered_set<Keys>* keys;
	void AddPressedKey(Keys key);
	void RemovePressedKey(Keys key);
public:
	MouseMoveEventArgs MouseParam;
	InputDevice();
	~InputDevice();
	void Initialize(HWND handle);
	void OnKeyDown(KeyboardInputEventArgs args);
	void OnMouseMove(RawMouseEventArgs args);
	bool IsKeyDown(Keys key);
	MouseMoveEventArgs getMouseParam() { return MouseParam; };
};

