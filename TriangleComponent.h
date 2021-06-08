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
	LPCWSTR textureFileName; // название файла с текстурой
	DirectX::SimpleMath::Vector3 compPosition;
};

struct constData
{
	DirectX::SimpleMath::Matrix WorldViewProj;
	DirectX::SimpleMath::Matrix World;
};

struct lightData {
	DirectX::SimpleMath::Vector4 Direction;
	DirectX::SimpleMath::Vector4 Color;
	DirectX::SimpleMath::Vector4 ViewerPos;
};

class TriangleComponent : public GameComponent {
private:
	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC; // вертекс байт код, результат компил€ции текстового файла с вертексным шейдером
	ID3DBlob* pixelBC; // пиксель байт код, результат компил€ции текстового файла с пиксельным шейдером
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout; // определение того, как передавать данные вершин, которые размещены в пам€ти, на этап сборщика ввода графического конвейера
	ID3D11Buffer* vBuffers[4];
	UINT strides[4]; // шаг вершин дл€ каждого буфера
	UINT offsets[4]; // смещение от начала дл€ каждого буфера
	ID3D11Buffer* indBuf;
	ID3D11BlendState* blend;
	ID3D11Buffer* constBuf;
	ID3D11Buffer* lightBuf;
	ID3D11RasterizerState* rastState;
	float blendFactor[4];
	UINT sampleMask;
	IWICImagingFactory2* factory; // интерфейс загрузчика текстур
	ID3D11Texture2D* texture; // текстура
	ID3D11ShaderResourceView* textureView;
	ID3D11SamplerState* sampler;
	DirectX::SimpleMath::Vector4* normals; // вектор нормалей
	void NormalsCalc();
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