#include "Camera.h"

using namespace DirectX;

namespace RayTracer {

	Ray Camera::GetRay(float s, float t) const {
		XMVECTOR origin = XMLoadFloat3(&position);

		XMVECTOR target = XMVectorMultiplyAdd(
			XMLoadFloat3(&horizontal), XMVectorReplicate(s), XMLoadFloat3(&lowerLeft));
		target = XMVectorMultiplyAdd(
			XMLoadFloat3(&vertical), XMVectorReplicate(t), target);

		return { origin, XMVector3Normalize(XMVectorSubtract(target, origin)) };
		
	}

	XMVECTOR Camera::GetForward() const {
		float cosPitch = cosf(pitch);
		XMVECTOR forward = XMVectorSet(
			cosPitch * sinf(yaw),
			sinf(pitch),
			cosPitch * cosf(yaw),
			0.0f);
		return XMVector3Normalize(forward);
	}

	XMVECTOR Camera::GetRight() const {
		XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		return XMVector3Normalize(XMVector3Cross(worldUp, GetForward()));
	}

	void Camera::Update(const Input& input, float deltaTime) {
		XMVECTOR pos = XMLoadFloat3(&position);
		XMVECTOR forward = GetForward();
		XMVECTOR right = GetRight();

		float distance = moveSpeed * deltaTime;

		if (input.IsKeyDown('W')) pos = XMVectorAdd(pos, XMVectorScale(forward, distance));
		if (input.IsKeyDown('S')) pos = XMVectorSubtract(pos, XMVectorScale(forward, distance));
		if (input.IsKeyDown('D')) pos = XMVectorAdd(pos, XMVectorScale(right, distance));
		if (input.IsKeyDown('A')) pos = XMVectorSubtract(pos, XMVectorScale(right, distance));

		XMStoreFloat3(&position, pos);
	}

	void Camera::Rotate(float deltaYaw, float deltaPitch) {
		yaw += deltaYaw * lookSensitivity;
		pitch -= deltaPitch * lookSensitivity;

		float limit = XMConvertToRadians(89.0f);
		if (pitch > limit) pitch = limit;
		if (pitch < -limit) pitch = -limit;
	}
}