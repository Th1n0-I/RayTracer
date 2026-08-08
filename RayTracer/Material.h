#pragma once

#include <DirectXMath.h>

namespace RayTracer {
	struct Material {
		DirectX::XMFLOAT3 color = { 0.7f, 0.7f, 0.7f };
		DirectX::XMFLOAT3 emissionColor = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 specularColor = { 1.0f, 1.0f, 1.0f };
		float specularChance = 0.0f;
		float roughness = 0.0f;
	};
}