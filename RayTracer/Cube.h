#pragma once

#include <DirectXMath.h>
#include <vector>
#include "Triangle.h"
#include "Material.h"

namespace RayTracer {
	class Cube {
	public:
		Cube(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, Material mat, std::vector<Triangle>& tris);
		bool RayCubeIntersect(const Ray& ray, const Cube& cube, float tMin, float tMax, HitData& data, std::vector<Triangle>& tris);

		Material material{};
		std::vector<int> indices;
	};
}