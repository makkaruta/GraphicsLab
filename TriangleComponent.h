#pragma once

#include "pch.h"

#include "GameComponent.h"

struct TriangleComponentParameters {
	DirectX::SimpleMath::Vector4 *positions;
	DirectX::SimpleMath::Vector4 *colors;
	int *indeces;
	int numPoints;
	int numIndeces;
	DirectX::SimpleMath::Vector3 compPosition;
};

class TriangleComponent : public GameComponent {
private:
	TriangleComponentParameters parameters;
	ID3DBlob* vertexBC; // вертекс байт код, результат компил€ции текстового файла с вертексным шейдером
	ID3DBlob* pixelBC; // пиксель байт код, результат компил€ции текстового файла с пиксельным шейдером
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout; // определение того, как передавать данные вершин, которые размещены в пам€ти, на этап сборщика ввода графического конвейера
	ID3D11Buffer* vBuffers[2];
	UINT strides[2]; // шаг вершин дл€ каждого буфера
	UINT offsets[2]; // смещение от начала дл€ каждого буфера
	ID3D11Buffer* indBuf;
	ID3D11BlendState* blend;
	ID3D11Buffer* constBuf;
	ID3D11RasterizerState* rastState;
	float blendFactor[4];
	UINT sampleMask;
public:
	DirectX::SimpleMath::Vector3 compPosition;
	TriangleComponent();
	TriangleComponent(TriangleComponentParameters param);
	int PrepareResourses(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void DestroyResourses();
	void Update(ID3D11DeviceContext* context, Camera* camera);
	void Draw(ID3D11DeviceContext* context);
};