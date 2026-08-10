#pragma once

#include <DirectXMath.h>
#include <vector>

#include "Material.h"
#include "Ray.h"

namespace RayTracer {
	struct Triangle {
		DirectX::XMFLOAT3 v0, v1, v2;
		int material;
	};

	bool RayTriangle(const Ray& ray, const Triangle& triangle, float tMin, float tMax, HitData& data, std::vector<Material>& materials);

}