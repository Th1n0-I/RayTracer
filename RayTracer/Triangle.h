#pragma once

#include <DirectXMath.h>

#include "Material.h"
#include "Ray.h"

namespace RayTracer {
	struct Triangle {
		DirectX::XMFLOAT3 v0, v1, v2;
		Material material{};
	};

	bool RayTriangle(const Ray& ray, const Triangle& triangle, float tMin, float tMax, HitData& data);

}