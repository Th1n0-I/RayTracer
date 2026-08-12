#include "Quad.h"

using namespace DirectX;

namespace RayTracer {

	float lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	XMFLOAT3 lerpFloat3(XMFLOAT3 a, XMFLOAT3 b, XMFLOAT3 c, XMFLOAT3 d, float u, float v) {
		XMFLOAT3 bottom = { lerp(a.x, d.x, u), lerp(a.y, d.y, u), lerp(a.z, d.z, u) };
		XMFLOAT3 top    = { lerp(b.x, c.x, u), lerp(b.y, c.y, u), lerp(b.z, c.z, u) };
		XMFLOAT3 result = { lerp(bottom.x, top.x, v), lerp(bottom.y, top.y, v), lerp(bottom.z, top.z, v) };
		return result;
	}

	Quad::Quad(XMFLOAT3 corner0, XMFLOAT3 corner1, XMFLOAT3 corner2, XMFLOAT3 corner3,
		int mat, std::vector<Triangle>& tris, int subDivX, int subDivY) {

		std::vector<XMFLOAT3> verts;

		for (int u = 0; u <= subDivX; u ++) {
			for (int v = 0; v <= subDivY; v ++) {
				verts.push_back(lerpFloat3(corner0, corner1, corner2, corner3, (float)u / subDivX, (float)v / subDivY));
			}
		}

		for (int x = 0; x < subDivX; x++) {
			for (int y = 0; y < subDivY; y++) {
				auto indx = x * (subDivY + 1) + y;
				tris.push_back({
					verts[indx],
					verts[indx + 1],
					verts[indx + subDivY + 2],
					mat
					});
				tris.push_back({
					verts[indx],
					verts[indx + subDivY + 2],
					verts[indx + subDivY + 1],
					mat
					});
			}
		}

	}
}