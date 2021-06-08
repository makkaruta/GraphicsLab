#pragma once

#include "pch.h"

#include "DisplayWin32.h"
#include "Camera.h"

#define NOTHING_TO_DRAW -1
#define SUCCESS 0
#define ERROR_VERTEX_BC 1
#define MISSING_VERTEX_SHADER 2
#define ERROR_PIXEL_BC 3
#define MISSING_PIXEL_SHADER 4
#define ERROR_CREATING_VERTEX_SHADER 5
#define ERROR_CREATING_PIXEL_SHADER 6
#define ERROR_CREATING_COM_OBJ 7
#define ERROR_CREATING_LAYOUT 8
#define ERROR_CREATING_POSBUF 9
#define ERROR_CREATING_COLBUF 10
#define ERROR_CREATING_TEXBUF 11
#define ERROR_CREATING_INDBUF 12
#define ERROR_CREATING_BLENDSTATE 13
#define ERROR_CREATING_CONSTBUF 14
#define ERROR_CREATING_RASTSTATE 15
#define ERROR_CREATING_DECODER 16
#define ERROR_GET_FRAME 17
#define ERROR_CREATING_CONVERTER 18
#define ERROR_INITIALIZE_CONVERTER 19
#define ERROR_COPY_PIXELS 20
#define ERROR_CREATING_TEXTURE 21
#define ERROR_CREATING_SHADER_RV 22
#define ERROR_RESOURCES_NOT_PREPARED 23
#define ERROR_TEXTURE_FILE_MISSING 24
#define ERROR_LOAD_TEXTURE_ORDER 25
#define ERROR_CREATING_SAMPLER_STATE 26

using namespace DirectX;

class GameComponent {
public:
	virtual int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device) { return 0; };
	virtual int LoadTextureFromFile(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11DeviceContext* context, bool generateMips, bool useSrgb, UINT frameIndex) { return 0; };
	virtual void DestroyResourses() {};
	virtual void Update(ID3D11DeviceContext* context, Camera* camera) {};
	virtual void Draw(ID3D11DeviceContext* context) {};
};
