#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Triangle.h"

namespace RayTracer {

	struct Bounds {
		DirectX::XMFLOAT3 min{  FLT_MAX, FLT_MAX, FLT_MAX };
		DirectX::XMFLOAT3 max{ -FLT_MAX,-FLT_MAX,-FLT_MAX };

		void Grow(const DirectX::XMFLOAT3& p) {
			min.x = fminf(min.x, p.x); max.x = fmaxf(max.x, p.x);
			min.y = fminf(min.y, p.y); max.y = fmaxf(max.y, p.y);
			min.z = fminf(min.z, p.z); max.z = fmaxf(max.z, p.z);
		}
		void Grow(const Bounds& b) { Grow(b.min); Grow(b.max); }

		float Area() const {
			float w = max.x - min.x, h = max.y - min.y, d = max.z - min.z;
			if (w < 0.0f) return 0.0f;
			return 2.0f * (w * h + w * d + d * h);
		}
	};

	inline float Axis(const DirectX::XMFLOAT3& v, int a) { return(&v.x)[a]; }

	class BoundingVolume {
	public:
		
		bool RayBoundVolumeIntersect(const Ray& ray,
			const std::vector<Triangle>& triangles,
			const std::vector<int>& indices,
			HitData& data,
			std::vector<Material>& materials,
			DirectX::XMFLOAT3& divRayDir,
			std::vector<BoundingVolume>& nodes) const;

		// When not leaf, child 1 / 2 index. When leaf, triangle vector start / end index
		int m_index = 0;
		int m_count = 0;
		Bounds bounds;
	};

	int Build(std::vector<Triangle>& tris, std::vector<DirectX::XMFLOAT3>& centroids, std::vector<int>& indices, int start, int count, std::vector<BoundingVolume>& nodes, int self);
}