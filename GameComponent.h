#pragma once

#include "pch.h"

#include "DisplayWin32.h"
#include "Camera.h"

#define NOTHING_TO_DRAW -1
#define SUCCESS 0
#define ERROR_VBC 1
#define MISSING_VS 2
#define ERROR_PBC 3
#define MISSING_PS 4
#define ERROR_CREATING_VS 5
#define ERROR_CREATING_PS 6
#define ERROR_CREATING_LAYOUT 7
#define ERROR_CREATING_POSBUF 8
#define ERROR_CREATING_COLBUF 9
#define ERROR_CREATING_INDBUF 10
#define ERROR_CREATING_BLENDSTATE 11
#define ERROR_CREATING_CONSTBUF 12
#define ERROR_CREATING_RASTSTATE 13

using namespace DirectX;

class GameComponent {
public:
	virtual int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) { return 0; };
	virtual void DestroyResourses() {};
	virtual void Update(ID3D11DeviceContext* context, Camera* camera) {};
	virtual void Draw(ID3D11DeviceContext* context) {};
};
