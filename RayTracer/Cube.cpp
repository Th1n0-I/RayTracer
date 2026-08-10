#include "Cube.h"

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

namespace RayTracer {
	Cube::Cube(XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT3 rotation, int mat, std::vector<Triangle>& tris) {
		std::vector<XMFLOAT3> verts = {
			{-1,-1,-1},
			{-1,-1, 1},
			{-1, 1,-1},
			{ 1,-1,-1},
			{ 1, 1,-1},
			{ 1,-1, 1},
			{-1, 1, 1},
			{ 1, 1, 1},
		};

		for (auto& v : verts) {

			const XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));

			XMStoreFloat3(&v, XMVectorSet(v.x, v.y, v.z, 0.0f) * XMVectorSet(scale.x / 2.0f, scale.y / 2.0f, scale.z / 2.0f, 0.0f));
			XMStoreFloat3(&v, XMVector3Rotate(XMVectorSet(v.x, v.y, v.z, 0.0f), quaternion));
			XMStoreFloat3(&v, XMVectorAdd(XMVectorSet(v.x, v.y, v.z, 0.0f), XMVectorSet(pos.x, pos.y, pos.z, 0.0f)));
		}

		std::vector<int> indx = {
			0, 1, 2,	1, 6, 2,
			3, 0, 2,	3, 4, 2,
			3, 4, 5,	7, 4, 5,
			7, 1, 5,	7, 1, 6,
			7, 2, 6,	7, 2, 4,
			1, 0, 3,	1, 5, 3,
		};

		for (int i = 0; i < indx.size(); i+=3) {
			int index = tris.size() + i / 3;
			indices.push_back(index);
		}

		for (int i = 0; i < indx.size(); i += 3) {
			tris.push_back({
				verts[indx[i]], verts[indx[i + 1]], verts[indx[i + 2]],
				mat
			});
		};
	}
}