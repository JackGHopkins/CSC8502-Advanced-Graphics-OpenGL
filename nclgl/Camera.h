#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <vector>
class CameraAngle {
public:
	CameraAngle(float pitch, float yaw) {
		this->pitch = pitch;
		this->yaw = yaw;
	}

	float pitch;
	float yaw;
};

class Camera {
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		index = 0;
	};

	Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;

		this->cameraPositions.emplace_back(Vector3(1825,180,2300));
		this->cameraPositions.emplace_back(Vector3(2500, 750, 9500));
		this->cameraPositions.emplace_back(Vector3(9900, 1600, 11500));
		this->cameraPositions.emplace_back(Vector3(13000, 2880, 5500));

		this->cameraAngles.emplace_back(CameraAngle(7,220));
		this->cameraAngles.emplace_back(CameraAngle(20, 275));
		this->cameraAngles.emplace_back(CameraAngle(-70, -17));
		this->cameraAngles.emplace_back(CameraAngle(70, 117));
		this->index = 0;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);
	void UpdateAutomaticCamera(float dt);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

	std::vector<Vector3> cameraPositions;
	std::vector<CameraAngle> cameraAngles;
	int index;
protected:
	float yaw;
	float pitch;
	float speed;
	Vector3 position;
};
