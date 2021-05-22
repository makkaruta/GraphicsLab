#pragma once

#include "GameComponent.h"

#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>

using namespace DirectX;


#define ZCHECK(exp) if(FAILED(exp)) { printf("Check failed at file: %s at line %i", __FILE__, __LINE__); return 0; }

class TriangleComponent : public GameComponent {
public:
	DirectX::XMFLOAT4* positions;
	DirectX::XMFLOAT4* colors;
	int* indeces;

	ID3DBlob* vertexBC; // ������� ���� ���, ��������� ���������� ���������� ����� � ���������� ��������
	ID3DBlob* pixelBC; // ������� ���� ���, ��������� ���������� ���������� ����� � ���������� ��������
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout; // ����������� ����, ��� ���������� ������ ������, ������� ��������� � ������, �� ���� �������� ����� ������������ ���������

	ID3D11Buffer* vBuffers[2];
	UINT strides[2]; // ��� ������ ��� ������� ������
	UINT offsets[2]; // �������� �� ������ ��� ������� ������

	ID3D11Buffer* indBuf;

	ID3D11RasterizerState* rastState;

	TriangleComponent();
	TriangleComponent(DirectX::XMFLOAT4* positionsArr, DirectX::XMFLOAT4* colorsArr, int* indecesArr);
	void Initialize();
	void Update();
	void Draw(ID3D11DeviceContext* context);
	int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void DestroyResourses();
};