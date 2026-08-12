#pragma once 

#include <vector>
#include <DirectXMath.h>

#include "Triangle.h"

namespace RayTracer {
	struct Quad {
		Quad(DirectX::XMFLOAT3 corner0, DirectX::XMFLOAT3 corner1, DirectX::XMFLOAT3 corner2, DirectX::XMFLOAT3 corner3, int mat,
			std::vector<Triangle>& tris, int subDivX, int subDivY);
	};
}