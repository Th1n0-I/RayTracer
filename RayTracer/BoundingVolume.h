#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Triangle.h"

namespace RayTracer {
	class BoundingVolume {
	public:
		BoundingVolume(std::vector<Triangle>& tris, DirectX::XMFLOAT3 boundMin, DirectX::XMFLOAT3 boundMax, std::vector<BoundingVolume>& nodes);
		bool RayBoundVolumeIntersect(const Ray& ray, const std::vector<Triangle>& triangles, HitData& data, std::vector<Material>& materials, DirectX::XMFLOAT3& divRayDir, std::vector<BoundingVolume>& nodes);
	private:
		int m_child1;
		int m_child2;
		std::vector<int> m_tris;
		DirectX::XMFLOAT3 m_boundMin;
		DirectX::XMFLOAT3 m_boundMax;
		bool m_isLeaf;
	};
}