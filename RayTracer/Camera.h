#pragma once
#include <DirectXMath.h>
#include "Input.h"
#include "Ray.h"

namespace RayTracer {
	

	class Camera {
	public:
		Ray GetRay(float s, float t) const;

		void CalculateViewPlane() { 
			float halfHeight = tanf(fov / 2.0f);
			float halfWidth = halfHeight * aspect;
			DirectX::XMStoreFloat3(&vertical, DirectX::XMVectorScale( GetUp(), 2.0f * halfHeight));
			DirectX::XMStoreFloat3(&horizontal, DirectX::XMVectorScale(GetRight(), 2.0f * halfWidth));
			DirectX::XMStoreFloat3(&lowerLeft, DirectX::XMVectorSubtract(DirectX::XMVectorAdd(DirectX::XMVectorSet(position.x, position.y, position.z, 0.0f), GetForward()),
				DirectX::XMVectorAdd(DirectX::XMVectorScale( GetRight(), halfWidth), DirectX::XMVectorScale(GetUp(), halfHeight))));
		};
		void SetAspect(int width, int height) { aspect = (float)width / (float)height; };
		inline DirectX::XMVECTOR GetUp() const { return DirectX::XMVector3Cross(GetForward(), GetRight()); };
		DirectX::XMVECTOR GetForward() const;
		DirectX::XMVECTOR GetRight() const;

		void Update(const Input& input, float deltaTime);

		void Rotate(float deltaYaw, float deltaPitch);

		DirectX::XMFLOAT3 position = { 0.0f, 0.0f, -3.0f };
		DirectX::XMFLOAT3 horizontal = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 vertical = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 lowerLeft = { 0.0f, 0.0f, 0.0f };
		float fov = DirectX::XMConvertToRadians(60.0f);
		float aspect = 0.0f;
		float yaw = 0.0f;
		float pitch = 0.0f;
		float lookSensitivity = 0.005f;
		float moveSpeed = 3.0f;
	};
}