#pragma once

#include "pch.h"

#include "GameComponent.h"

struct TriangleComponentParameters {
	DirectX::SimpleMath::Vector4 *positions;
	DirectX::SimpleMath::Vector4 *colors;
	DirectX::SimpleMath::Vector4* texcoords;
	int *indeces;
	int numPoints;
	int numIndeces;
	LPCWSTR textureFileName; // �������� ����� � ���������
	DirectX::SimpleMath::Vector3 compPosition;
};

class TriangleComponent : public GameComponent {
private:
	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC; // ������� ���� ���, ��������� ���������� ���������� ����� � ���������� ��������
	ID3DBlob* pixelBC; // ������� ���� ���, ��������� ���������� ���������� ����� � ���������� ��������
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout; // ����������� ����, ��� ���������� ������ ������, ������� ��������� � ������, �� ���� �������� ����� ������������ ���������
	ID3D11Buffer* vBuffers[3];
	UINT strides[3]; // ��� ������ ��� ������� ������
	UINT offsets[3]; // �������� �� ������ ��� ������� ������
	ID3D11Buffer* indBuf;
	ID3D11BlendState* blend;
	ID3D11Buffer* constBuf;
	ID3D11RasterizerState* rastState;
	float blendFactor[4];
	UINT sampleMask;
	IWICImagingFactory2* factory; // ��������� ���������� �������
	ID3D11Texture2D* texture; // ��������
	ID3D11ShaderResourceView* textureView;
	ID3D11SamplerState* sampler;
public:
	DirectX::SimpleMath::Vector3 compPosition;
	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);
	int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device);
	int LoadTextureFromFile(Microsoft::WRL::ComPtr<ID3D11Device> device, ID3D11DeviceContext* context, bool generateMips, bool useSrgb, UINT frameIndex);
	void DestroyResourses();
	void Update(ID3D11DeviceContext* context, Camera* camera);
	void Draw(ID3D11DeviceContext* context);
};