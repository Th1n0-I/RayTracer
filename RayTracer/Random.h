#pragma once
#include <cstdint>
#include <DirectXMath.h>

namespace RayTracer {
	uint32_t Hash(uint32_t x) {
		x ^= x >> 16;
		x *= 0x7feb352du;
		x ^= x >> 15;
		x *= 0x846ca68bu;
		x ^= x >> 16;
		return x;
	}

	uint32_t RandUint(uint32_t& state){
		state ^= state << 13;
		state ^= state >> 17;
		state ^= state << 5;
		return state;
	}

	float RandFloat(uint32_t& state) {

		return(RandUint(state) >> 8) * (1.0f / 16777216.0f);
	}

	DirectX::XMVECTOR RandomUnitVector(uint32_t& rng) {
		const float z = RandFloat(rng) * 2.0f - 1.0f;
		const float a = RandFloat(rng) * 6.283185531f;
		const float r = sqrt(1.0f - z * z);
		return DirectX::XMVectorSet(r * cosf(a), r * sinf(a), z, 0.0f);
	}
}