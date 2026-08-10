#pragma once

#include <DirectXMath.h>
#include <vector>
#include "Triangle.h"
#include "Material.h"

namespace RayTracer {
	class Cube {
	public:
		Cube(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, int mat, std::vector<Triangle>& tris);

		Material material{};
		std::vector<int> indices;
	};
}