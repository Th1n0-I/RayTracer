#pragma once

#include <DirectXMath.h>
#include "Ray.h"

namespace RayTracer {
	struct Sphere {
		DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
		float radius = 1.0f;
	};

	struct HitData {
		float t = 0.0f;
		DirectX::XMVECTOR point{};
		DirectX::XMVECTOR normal{};
	};

	bool RaySphere(const Ray& ray, const Sphere& sphere,
		float tMin, float tMax, HitData& data);
}