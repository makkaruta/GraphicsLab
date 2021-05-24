#pragma once

#include "GameComponent.h"

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
#define ERROR_CREATING_RASTSTATE 11

struct TriangleComponentParameters {
	DirectX::XMFLOAT4 *positions;
	DirectX::XMFLOAT4 *colors;
	int *indeces;
	int numPoints;
	int numIndeces;
};

class TriangleComponent : public GameComponent {
private:
	TriangleComponentParameters parameters;
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
public:
	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);
	int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void DestroyResourses();
	void Update();
	void Draw(ID3D11DeviceContext* context);
};