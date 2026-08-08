#pragma once

#include <DirectXMath.h>

namespace RayTracer {
	struct Material {
		DirectX::XMFLOAT3 color = { 0.7f, 0.7f, 0.7f };
	};
}