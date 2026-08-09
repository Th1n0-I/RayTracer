#pragma once

#include <DirectXMath.h>
#include <vector>
#include "Triangle.h"
#include "Material.h"

namespace RayTracer {
	struct Cube {

		Cube(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, Material mat);;

		Material material{};
		std::vector<Triangle> tris;
	};
}