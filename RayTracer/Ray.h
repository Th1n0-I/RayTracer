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
		DirectX::XMVECTOR color{};
		DirectX::XMVECTOR emission{};
		DirectX::XMVECTOR specularColor{};
		float specularChance{};
		float roughness = 0.0f;
	};
}