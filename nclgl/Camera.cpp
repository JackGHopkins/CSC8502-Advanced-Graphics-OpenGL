#include "Camera.h"
#include "Window.h"
#include <algorithm>
void Camera::UpdateCamera(float dt) {
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	speed = 1000.0f * dt;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y -= speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += speed;
	}
}

void Camera::UpdateAutomaticCamera(float dt) {
	speed = 1000.0f * dt;

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	if (cameraPositions.size() == 0) {
		std::cout << "ERROR ERROR ERROR ERROR: No Automatic Camera POSITIONS listed!\n";
		return;
	}
	if (cameraAngles.size() == 0) {
		std::cout << "ERROR ERROR ERROR ERROR: No Automatic Camera ANGLES listed!\n";
		return;
	}
	/*if (cameraAngles.size() == cameraPositions.size()) {
		std::cout << "ERROR ERROR ERROR ERROR: Number of Camera Positions and Angles are not Equal!\n";
		return;
	}*/
	Vector3 direction = cameraPositions[index] - position;
	float timeTweenPoints = direction.Length() / speed;
	direction = direction / direction.Length();
	position += direction * speed;

	if (!(yaw > cameraAngles[index].yaw - 0.1 && yaw < cameraAngles[index].yaw + 0.1)) {
		int cAGradYaw = (yaw - cameraAngles[index].yaw > 180 || yaw - cameraAngles[index].yaw < 0) ? 0.1 : -0.1;
		yaw += cAGradYaw;
	}

	if (!(pitch > cameraAngles[index].pitch - 1 && pitch < cameraAngles[index].pitch + 1)) {
		int cAGradPitch = (pitch < cameraAngles[index].pitch) ? 1 : -1;
		pitch += cAGradPitch;
	}

	if ((position.x > cameraPositions[index].x - 10 && position.x < cameraPositions[index].x + 10) &&
		(position.y > cameraPositions[index].y - 10 && position.y < cameraPositions[index].y + 10) &&
		(position.z > cameraPositions[index].z - 10 && position.z < cameraPositions[index].z + 10)) {
		if (index < cameraPositions.size() - 1) {
			index++;
		}
		else {
			index = 0;
		}
	}
}

Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);
};
