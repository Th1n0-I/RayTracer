#pragma once

#include <DirectXMath.h>

namespace RayTracer {
	struct Ray {
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR direction;
	};
}