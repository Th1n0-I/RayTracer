#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Triangle.h"

namespace RayTracer {
	class BoundingVolume {
	public:
		BoundingVolume(std::vector<Triangle>& tris, DirectX::XMFLOAT3 boundMin, DirectX::XMFLOAT3 boundMax);
		bool RayBoundVolumeIntersect(const Ray& ray, const std::vector<Triangle>& triangles, HitData& data, std::vector<Material>& materials);
	private:
		std::vector<BoundingVolume> m_children;
		std::vector<int> m_tris;
		DirectX::XMFLOAT3 m_boundMin;
		DirectX::XMFLOAT3 m_boundMax;
		bool m_isLeaf;
	};
}