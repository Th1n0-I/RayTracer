#pragma once

#include <DirectXMath.h>
#include <vector>


#include "Triangle.h"

namespace RayTracer {
	class Mesh {
	public:
		Mesh(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rot, int mat,
			std::vector<Triangle>& tris, const char* path);
	};
}