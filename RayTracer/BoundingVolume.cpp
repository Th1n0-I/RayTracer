#include "BoundingVolume.h"


using namespace DirectX;

namespace RayTracer {

	bool AABBIntersect(const Ray& ray, XMFLOAT3 min, XMFLOAT3 max, HitData& data, XMFLOAT3 divDir, float& hitDist) {
		XMFLOAT3 tLow = { (min.x - XMVectorGetX(ray.position)) * divDir.x,
		(min.y - XMVectorGetY(ray.position))* divDir.y,
		(min.z - XMVectorGetZ(ray.position))* divDir.z };

		XMFLOAT3 tHigh = { (max.x - XMVectorGetX(ray.position)) * divDir.x,
		(max.y - XMVectorGetY(ray.position))* divDir.y,
		(max.z - XMVectorGetZ(ray.position))* divDir.z };

		XMFLOAT3 tClose = { fminf(tLow.x, tHigh.x), fminf(tLow.y, tHigh.y), fminf(tLow.z, tHigh.z) };
		XMFLOAT3 tFar = { fmaxf(tLow.x, tHigh.x), fmaxf(tLow.y, tHigh.y), fmaxf(tLow.z, tHigh.z) };

		float close = fmaxf(tClose.x, fmaxf(tClose.y, tClose.z));
		float far = fminf(tFar.x, fminf(tFar.y, tFar.z));

		hitDist = close;

		if (close <= far && close <= data.t && far >= 0.0f) return true;
		return false;
	}


	BoundingVolume::BoundingVolume(std::vector<Triangle>& tris, DirectX::XMFLOAT3 boundMin, DirectX::XMFLOAT3 boundMax) :
	m_boundMax(boundMax), m_boundMin(boundMin), m_isLeaf(false){
		std::vector<int> trianglesInVolume;
		int count = 0;
		for (int indx = 0; indx < tris.size(); indx++) {
			XMVECTOR center = XMVectorScale(XMLoadFloat3(&tris[indx].v0) +
				XMLoadFloat3(&tris[indx].v1) +
				XMLoadFloat3(&tris[indx].v2), 1.0f / 3.0f);
			XMFLOAT3 centerFloat3; XMStoreFloat3(&centerFloat3, center);


			if (centerFloat3.x >= boundMin.x && centerFloat3.x < boundMax.x &&
				centerFloat3.y >= boundMin.y && centerFloat3.y < boundMax.y &&
				centerFloat3.z >= boundMin.z && centerFloat3.z < boundMax.z) {
				count++;
				m_tris.push_back(indx);
			}
		}

		if (count == 0) { m_isLeaf = true; return; }

		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float minZ = FLT_MAX;
		float maxX = -FLT_MAX;
		float maxY = -FLT_MAX;
		float maxZ = -FLT_MAX;
		for (const auto& indx : m_tris) {
			const auto& tri = tris[indx];
			if (tri.v0.x < minX) minX = tri.v0.x;
			if (tri.v0.y < minY) minY = tri.v0.y;
			if (tri.v0.z < minZ) minZ = tri.v0.z;
			if (tri.v0.x > maxX) maxX = tri.v0.x;
			if (tri.v0.y > maxY) maxY = tri.v0.y;
			if (tri.v0.z > maxZ) maxZ = tri.v0.z;
			if (tri.v1.x < minX) minX = tri.v1.x;
			if (tri.v1.y < minY) minY = tri.v1.y;
			if (tri.v1.z < minZ) minZ = tri.v1.z;
			if (tri.v1.x > maxX) maxX = tri.v1.x;
			if (tri.v1.y > maxY) maxY = tri.v1.y;
			if (tri.v1.z > maxZ) maxZ = tri.v1.z;
			if (tri.v2.x < minX) minX = tri.v2.x;
			if (tri.v2.y < minY) minY = tri.v2.y;
			if (tri.v2.z < minZ) minZ = tri.v2.z;
			if (tri.v2.x > maxX) maxX = tri.v2.x;
			if (tri.v2.y > maxY) maxY = tri.v2.y;
			if (tri.v2.z > maxZ) maxZ = tri.v2.z;
		}
		m_boundMin = { minX - 0.01f, minY - 0.01f, minZ - 0.01f };
		m_boundMax = { maxX + 0.01f, maxY + 0.01f, maxZ + 0.01f };

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
			else{
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

	bool BoundingVolume::RayBoundVolumeIntersect(const Ray& ray, const std::vector<Triangle>& triangles, HitData& data, std::vector<Material>& materials, XMFLOAT3& divRayDir) {
		bool hitAnything = false;

		data.nodeCount++;
		
		if (!m_isLeaf) {
			float child1Dist = 0;
			float child2Dist = 0;
			bool child1Enter = AABBIntersect(ray, m_children[0].m_boundMin, m_children[0].m_boundMax, data, divRayDir, child1Dist);
			bool child2Enter = AABBIntersect(ray, m_children[1].m_boundMin, m_children[1].m_boundMax, data, divRayDir, child2Dist);
			if (child1Dist < child2Dist) {
				if (child1Enter) if (m_children[0].RayBoundVolumeIntersect(ray, triangles, data, materials, divRayDir)) hitAnything = true;
				if (child2Enter && child2Dist <= data.t) if (m_children[1].RayBoundVolumeIntersect(ray, triangles, data, materials, divRayDir)) hitAnything = true;
				}
			else { 
				if (child2Enter) if (m_children[1].RayBoundVolumeIntersect(ray, triangles, data, materials, divRayDir)) hitAnything = true; 
				if (child1Enter && child1Dist <= data.t) if (m_children[0].RayBoundVolumeIntersect(ray, triangles, data, materials, divRayDir)) hitAnything = true;
			}
			return hitAnything;
		}

		for (auto& indx : m_tris) {
			if (RayTriangle(ray, triangles[indx], 0.01f, data.t, data, materials)) hitAnything = true;
		}
		return hitAnything;
	}
}