#include "TriangleComponent.h"

TriangleComponent::TriangleComponent() {
	positions = nullptr;
	colors = nullptr;
	indeces = nullptr;
}

TriangleComponent::TriangleComponent(DirectX::XMFLOAT4* positionsArr, DirectX::XMFLOAT4* colorsArr, int* indecesArr) {
	positions = positionsArr;
	colors = colorsArr;
	indeces = indecesArr;
}

void TriangleComponent::Initialize(){

}

void TriangleComponent::Update() {

}

void TriangleComponent::Draw() {

}

void TriangleComponent::DestroyResources() {

}