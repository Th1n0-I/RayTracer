#include "BoundingVolume.h"

using namespace DirectX; 

namespace RayTracer {
	BoundingVolume::BoundingVolume(std::vector<Triangle>& tris, DirectX::XMFLOAT3 boundMin, DirectX::XMFLOAT3 boundMax) : 
	m_boundMax(boundMax), m_boundMin(boundMin), m_isLeaf(false){
		std::vector<int> trianglesInVolume;
		for (int indx = 0; indx < tris.size(); indx++) {
			XMVECTOR center = XMVectorScale(XMLoadFloat3(&tris[indx].v0) * 
				XMLoadFloat3(&tris[indx].v1) * 
				XMLoadFloat3(&tris[indx].v2), 1.0f / 3.0f);
			XMFLOAT3 centerFloat3; XMStoreFloat3(&centerFloat3, center);

			int count = 0;
			if (centerFloat3.x > boundMin.x && centerFloat3.x < boundMax.x &&
				centerFloat3.y > boundMin.y && centerFloat3.y < boundMax.y &&
				centerFloat3.z > boundMin.z && centerFloat3.z < boundMax.z) {
				count++;
				m_tris.push_back(indx);
			}

			if (count <= 6) {
				m_isLeaf = true;
			}
			else {
				XMFLOAT3 size{ boundMax.x - boundMin.x, boundMax.y - boundMin.y, boundMax.z - boundMin.z };
				if (size.x > size.y && size.x > size.z) {
					m_children.push_back({
						tris,
						{boundMin},
						{boundMin.x + size.x / 2, boundMax.y, boundMax.z}
						});
					m_children.push_back({
						tris,
						{boundMin.x + size.x / 2, boundMin.y, boundMin.z},
						{boundMax}
						});
				}
				else if (size.y > size.x && size.y > size.z) {
					m_children.push_back({
						tris,
						{boundMin},
						{boundMax.x, boundMin.y + size.y / 2, boundMax.z}
						});
					m_children.push_back({
						tris,
						{boundMin.x, boundMin.y + size.y / 2, boundMin.z},
						{boundMax}
						});
				}
				else if (size.z > size.y && size.z > size.x) {
					m_children.push_back({
						tris,
						{boundMin},
						{boundMax.x, boundMax.y, boundMin.z + size.z / 2}
						});
					m_children.push_back({
						tris,
						{boundMin.x, boundMin.y, boundMin.z + size.z / 2},
						{boundMax}
						});
				}
			}
		}
	}

	bool BoundingVolume::RayBoundVolumeIntersect(const Ray& ray, const std::vector<Triangle>& triangles, HitData& data, std::vector<Material>& materials) {
		bool hitAnything = false;
		if (!m_isLeaf) {
			for (auto& child : m_children) {
				if (child.RayBoundVolumeIntersect(ray, triangles, data, materials)) hitAnything = true;
			}
			return hitAnything;
		}

		for (auto& tri : triangles) {
			if (RayTriangle(ray, tri, 0.01f, data.t, data, materials)) hitAnything = true;
			return hitAnything;
		}
	}
}