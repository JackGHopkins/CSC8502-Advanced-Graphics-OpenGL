#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <vector>
class CameraAngle {
public:
	CameraAngle(float yaw, float pitch) {
		this->yaw = yaw;
		this->pitch = pitch;
	}

	float yaw;
	float pitch;
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

		this->cameraPositions.emplace_back(Vector3(0,0,0));
		this->cameraPositions.emplace_back(Vector3(100,1000,100));
		this->cameraPositions.emplace_back(Vector3(1000,0,1000));

		this->cameraAngles.emplace_back(CameraAngle(0.0f,0.0f));
		this->cameraAngles.emplace_back(CameraAngle(270.0f, 90.0f));
		this->cameraAngles.emplace_back(CameraAngle(20.0f, -90.0f));
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

protected:
	std::vector<Vector3> cameraPositions;
	std::vector<CameraAngle> cameraAngles;
	float yaw;
	float pitch;
	float speed;
	Vector3 position;
	int index;
};
