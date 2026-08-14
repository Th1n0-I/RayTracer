#pragma once

#include <DirectXMath.h>

namespace RayTracer {
	struct Ray {
		DirectX::XMVECTOR position;
		DirectX::XMVECTOR direction;
	};

	struct HitData {
		float t = 10000.0f;
		DirectX::XMVECTOR point{};
		DirectX::XMVECTOR normal{};
		int material = 0;
		int nodeCount = 0;
	};
}