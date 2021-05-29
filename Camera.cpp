#include "Pch.h"
#include "Camera.h"
#define PI 3.1415926535

void Camera::Initialize(int screenWidth, int screenHeight, InputDevice* inpDev) {
	inputDevice = inpDev;
	ViewMatrix = DirectX::SimpleMath::Matrix::Identity;
	std::cout << "Width: " << screenWidth << " Height: " << screenHeight << std::endl;
	ProjectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		((float)PI) / 2.0f, // ���� ���� ������ �� ��� y (� ��������)
		(float)screenWidth / screenHeight, // ����������� ������ �����������(������ / ������)
		0.1f, // ���������� �� ������� ��������� �������
		10000.0f); // ���������� �� ������� ��������� �������
	Yaw = 0;
	Pitch = 0;
	position = {1, 1, 1};
}

void Camera::Update(float deltaTime, int screenWidth, int screenHeight) {
	std::cout << "Yaw: " << Yaw << " Pitch: " << Pitch << std::endl;
	auto rotation = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(Yaw, Pitch, 0);
	auto velDirection = DirectX::SimpleMath::Vector3::Zero;
	if (inputDevice->IsKeyDown(Keys::W)) velDirection += DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	if (inputDevice->IsKeyDown(Keys::S)) velDirection += DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
	if (inputDevice->IsKeyDown(Keys::A)) velDirection += DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
	if (inputDevice->IsKeyDown(Keys::D)) velDirection += DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

	if (inputDevice->IsKeyDown(Keys::Space)) velDirection += DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	if (inputDevice->IsKeyDown(Keys::C)) velDirection += DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);

	velDirection.Normalize();

	auto velDir = rotation.Forward() * velDirection.x + DirectX::SimpleMath::Vector3::Up * velDirection.y + rotation.Right() * velDirection.z;
	if (velDir.Length() != 0) {
		velDir.Normalize();
	}
	position = position + velDir * VelocityMagnitude * deltaTime;
	ViewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(position, position + rotation.Forward(), rotation.Up());
	//std::cout << "Width: " << screenWidth << " Height: " << screenHeight << std::endl;
	ProjectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		(float)PI / 2.0f, // ���� ���� ������ �� ��� y (� ��������)
		(float)screenWidth / screenHeight, // ����������� ������ �����������(������ / ������)
		0.1f, // ���������� �� ������� ��������� �������
		10000.0f); // ���������� �� ������� ��������� �������
}

void Camera::OnMouseMove(MouseMoveEventArgs args) {
	if (inputDevice->IsKeyDown(Keys::LeftShift)) return;

	std::cout << "X: " << args.Offset.x << " Y: " << args.Offset.y << std::endl;

	Yaw -= args.Offset.x * 0.003f * MouseSensetivity;
	Pitch -= args.Offset.y * 0.003f * MouseSensetivity;

	if (args.WheelDelta == 0) return;
	if (args.WheelDelta > 0) VelocityMagnitude += 1;
	if (args.WheelDelta < 0) VelocityMagnitude -= 1;
}