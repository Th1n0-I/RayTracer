#include "BoundingVolume.h"
#include <algorithm>

using namespace DirectX;
using namespace RayTracer;

static constexpr int kBins = 12;
static constexpr float C_TRAV = 4.0f;
static constexpr float C_ISECT = 1.0f;

struct Bin { AABB bounds; int count = 0; };

float FindBestSplit(const std::vector<Triangle>& tris,
	const std::vector<XMFLOAT3>& centroids,
	const std::vector<int>& indices,
	int start, int count,
	int& bestAxis, int& bestBin) {

	bestAxis = -1;
	bestBin = -1;
	float bestCost = FLT_MAX;

	AABB cb;
	for (int i = start; i < start + count; i++) 
		cb.Grow(centroids[indices[i]]);
	
	for (int axis = 0; axis < 3; axis++) {
		float cmin = Axis(cb.min, axis);
		float cmax = Axis(cb.max, axis);
		if (cmax - cmin < 1e-8f) continue;

		float scale = kBins / (cmax - cmin);

		Bin bins[kBins];
		for (int i = start; i < start + count; i++) {
			int idx = indices[i];
			int b = (int)((Axis(centroids[idx], axis) - cmin) * scale);
			if (b >= kBins) b = kBins - 1;
			bins[b].count++;
			bins[b].bounds.Grow(tris[idx].v0);
			bins[b].bounds.Grow(tris[idx].v1);
			bins[b].bounds.Grow(tris[idx].v2);
		}

		float rightTerm[kBins - 1];
		AABB rb; int rc = 0;
		for (int i = kBins - 1; i > 0; i--) {
			rb.Grow(bins[i].bounds);
			rc += bins[i].count;
			rightTerm[i - 1] = rc ? rb.Area() * rc : 0.0f;
		}

		AABB lb; int lc = 0;
		for (int i = 0; i < kBins; i++) {
			lb.Grow(bins[i].bounds);
			lc += bins[i].count;
			if (lc == 0 || lc == count) continue;
			
			float cost = lb.Area() * lc + rightTerm[i];
			if (cost < bestCost) {
				bestCost = cost;
				bestAxis = axis;
				bestBin = i + 1;	
			}
		}
	}
	return bestCost;
}

namespace RayTracer {

	bool AABBIntersect(const Ray& ray, AABB bounds, HitData& data, XMFLOAT3 divDir, float& hitDist) {
		XMFLOAT3 tLow = { (bounds.min.x - XMVectorGetX(ray.position)) * divDir.x,
		(bounds.min.y - XMVectorGetY(ray.position))* divDir.y,
		(bounds.min.z - XMVectorGetZ(ray.position))* divDir.z };

		XMFLOAT3 tHigh = { (bounds.max.x - XMVectorGetX(ray.position)) * divDir.x,
		(bounds.max.y - XMVectorGetY(ray.position))* divDir.y,
		(bounds.max.z - XMVectorGetZ(ray.position))* divDir.z };

		XMFLOAT3 tClose = { fminf(tLow.x, tHigh.x), fminf(tLow.y, tHigh.y), fminf(tLow.z, tHigh.z) };
		XMFLOAT3 tFar = { fmaxf(tLow.x, tHigh.x), fmaxf(tLow.y, tHigh.y), fmaxf(tLow.z, tHigh.z) };

		float close = fmaxf(tClose.x, fmaxf(tClose.y, tClose.z));
		float far = fminf(tFar.x, fminf(tFar.y, tFar.z));

		hitDist = close;

		if (close <= far && close <= data.t && far >= 0.0f) return true;
		return false;
	}

	AABB ComputeBounds(std::vector<Triangle>& tris, std::vector<int> indices, int start, int count) {
		AABB b;
		for (int i = start; i < start + count; i++) {
			b.Grow(tris[indices[i]].v0);
			b.Grow(tris[indices[i]].v1);
			b.Grow(tris[indices[i]].v2);
		}
		return b;
	}

	int Build(std::vector<Triangle>& tris, std::vector<XMFLOAT3>& centroids, std::vector<int>& indices, int start, int count, std::vector<BoundingVolume>& nodes, int self) {
		
		nodes[self].bounds = ComputeBounds(tris, indices, start, count);

		int bestAxis, bestBin;
		float bestCost = FindBestSplit(tris, centroids, indices, start, count, bestAxis, bestBin);
		float splitCost = C_TRAV + (bestCost / nodes[self].bounds.Area()) * C_ISECT;
		float leafcost = count * C_ISECT;

		if (bestAxis < 0 || splitCost >= leafcost || count <= 2) {
			nodes[self].m_index = start;
			nodes[self].m_count = count;
			return self;
		}

		AABB cb;
		for (int i = start; i < start + count; i++) cb.Grow(centroids[indices[i]]);

		float cmin = Axis(cb.min, bestAxis);
		float scale = kBins / (Axis(cb.max, bestAxis) - cmin);

		auto begin = indices.begin() + start;
		auto mid = std::partition(begin, begin + count, [&](int idx) {
			int b = (int)((Axis(centroids[idx], bestAxis) - cmin) * scale);
			if (b >= kBins) b = kBins - 1;
			return b < bestBin;
			});
		int leftCount = (int)(mid - begin);

		int child1 = (int)nodes.size();
		nodes.emplace_back(); nodes.emplace_back();
		Build(tris, centroids, indices, start, leftCount, nodes, child1);
		Build(tris, centroids, indices, start + leftCount, count - leftCount, nodes, child1 + 1);
		nodes[self].m_index = child1;
		nodes[self].m_count = 0;
		return self;
	}

	/*bool RayBoundVolumeIntersect(const Ray& ray,
		const std::vector<Triangle>& triangles,
		const std::vector<int>& indices, 
		HitData& data, 
		std::vector<Material>& materials, 
		XMFLOAT3& divRayDir, 
		std::vector<BoundingVolume>& nodes, int indx) {

		

		data.nodeCount++;
		
		if (nodes[indx].m_count > 0) {
			bool hitAnything = false;
			for (int i = nodes[indx].m_index; i < nodes[indx].m_index + nodes[indx].m_count; i++)
				if (RayTriangle(ray, triangles[indices[i]], 0.01f, data.t, data, materials))
					hitAnything = true;
			return hitAnything;
		}

		bool hitAnything = false;

		float child1Dist = 0;
		float child2Dist = 0;
		bool child1Enter = AABBIntersect(ray, nodes[nodes[indx].m_index].bounds.min, nodes[nodes[indx].m_index].bounds.max, data, divRayDir, child1Dist);
		bool child2Enter = AABBIntersect(ray, nodes[nodes[indx].m_index + 1].bounds.min, nodes[nodes[indx].m_index + 1].bounds.max, data, divRayDir, child2Dist);
		if (child1Dist < child2Dist) {
			if (child1Enter) if (RayBoundVolumeIntersect(ray, triangles, indices, data, materials, divRayDir, nodes, nodes[indx].m_index)) hitAnything = true;
			if (child2Enter && child2Dist <= data.t) if (RayBoundVolumeIntersect(ray, triangles, indices, data, materials, divRayDir, nodes, nodes[indx].m_index + 1)) hitAnything = true;
			}
		else { 
			if (child2Enter) if (RayBoundVolumeIntersect(ray, triangles,indices, data, materials, divRayDir, nodes, nodes[indx].m_index + 1)) hitAnything = true;
			if (child1Enter && child1Dist <= data.t) if (RayBoundVolumeIntersect(ray, triangles, indices, data, materials, divRayDir, nodes, nodes[indx].m_index)) hitAnything = true;
		}
		return hitAnything;
	}*/

	bool RayBoundVolumeIntersect(const Ray& ray,
		const std::vector<Triangle>& triangles,
		const std::vector<int>& indices,
		HitData& data,
		std::vector<Material>& materials,
		XMFLOAT3& divRayDir,
		std::vector<BoundingVolume>& nodes, int indx) {
		
		bool hitAnything = false;

		int stack[64];
		float dstack[64];
		int sp = 0;
		int current = indx;

		while (true) {
			// The node is a leaf
			if (nodes[current].m_count > 0) {
				for (int i = nodes[current].m_index; i < nodes[current].m_index + nodes[current].m_count; i++) {
					if (RayTriangle(ray, triangles[indices[i]], 0.01f, data.t, data, materials)) hitAnything = true;
				}
				if (sp == 0) break;

				bool found = false;
				while (sp > 0) {
					int candidate = stack[--sp];
					if (dstack[sp] <= data.t) { current = candidate; found = true; break; }
				}
				if (!found) break;
			}
			// The node is not a leaf
			else {
				int child1 = nodes[current].m_index; int child2 = nodes[current].m_index + 1;
				float d1, d2;
				bool hit1 = AABBIntersect(ray, nodes[child1].bounds, data, divRayDir, d1);
				bool hit2 = AABBIntersect(ray, nodes[child2].bounds, data, divRayDir, d2);

				if (!hit1 && !hit2) { 
					if (sp == 0) break; 
					bool found = false;
					while (sp > 0) {
						int candidate = stack[--sp];
						if (dstack[sp] <= data.t) { current = candidate; found = true; break; }
					}
					if (!found) break;
				}
				else if (hit1 && hit2) {
					stack[sp] = d1 > d2 ? child1 : child2;
					dstack[sp] = d1 > d2 ? d1 : d2;
					sp++;
					current = d1 > d2 ? child2 : child1;
				}
				else { current = hit1 ? child1 : child2; }
			}
		}

		return hitAnything;
	}

	bool Occluded(const Ray& ray, const std::vector<Triangle>& triangles,
		std::vector<int>& indices, std::vector<BoundingVolume>& nodes,
		int indx, XMFLOAT3& divDir, float maxT, std::vector<Material>& materials) {

		int stack[64];
		int sp = 0;
		int current = indx;

		HitData hitdata{};

		while (true) {
			auto& currentNode = nodes[current];
			if (currentNode.m_count > 0) {
				for (int i = currentNode.m_index; i < currentNode.m_index + currentNode.m_count; i++) {
					if (RayTriangle(ray, triangles[indices[i]], 0.01f, maxT, hitdata, materials)) return true;
				}
				if (sp == 0) break;
				current = stack[--sp];
			}
			else {
				int child1 = currentNode.m_index; int child2 = child1 + 1;
				float d1, d2;
				bool hit1 = AABBIntersect(ray, nodes[child1].bounds, hitdata, divDir, d1);
				bool hit2 = AABBIntersect(ray, nodes[child2].bounds, hitdata, divDir, d2);

				if (!hit1 && !hit2) { if (sp == 0) break; current = stack[--sp]; }
				else if (hit1 && hit2) {
					if (d1 < maxT && d2 < maxT) { current = child1;  stack[sp++] = child2; }
					else if (d1 < maxT && d2 > maxT) { current = child1; }
					else if (d2 < maxT && d1 > maxT) { current = child2; }
					else { if (sp == 0) break; current = stack[--sp]; }
				}
				else {
					current = hit1 ? child1 : child2;
				}
			}
		}
		return false;
	}
}