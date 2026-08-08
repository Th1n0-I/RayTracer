#pragma once

#include <DirectXMath.h>
#include "Ray.h"
#include "Material.h"

namespace RayTracer {
	struct Sphere {
		Material material{};
		DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
		float radius = 1.0f;
	};

	bool RaySphere(const Ray& ray, const Sphere& sphere,
		float tMin, float tMax, HitData& data);
}