#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>

using namespace DirectX;

namespace RayTracer {

	static int ParseFaceIndex(const std::string& token, size_t vertexCount) {
		const size_t slash = token.find("/");
		const int raw = std::stoi(token.substr(0, slash));

		if (raw > 0) return raw - 1;

		return (int)vertexCount + raw;
	}

	Mesh::Mesh(XMFLOAT3 pos, XMFLOAT3 scale, XMFLOAT3 rot, int mat,
		std::vector<Triangle>& tris, const char* path) {

		std::ifstream file(path);
		if (!file) return;

		const XMMATRIX world =
			XMMatrixScaling(scale.x, scale.y, scale.z) *
			XMMatrixRotationRollPitchYaw(
				XMConvertToRadians(rot.x),
				XMConvertToRadians(rot.y),
				XMConvertToRadians(rot.z)) *
			XMMatrixTranslation(pos.x, pos.y, pos.z);

		std::vector<XMFLOAT3> verts;
		std::string line;

		while (std::getline(file, line)) {
			std::istringstream s(line);
			std::string prefix;
			s >> prefix;

			if (prefix == "v") {
				XMFLOAT3 v;

				s >> v.x >> v.y >> v.z;

				XMVECTOR p = XMLoadFloat3(&v);
				p = XMVector3Transform(p, world);	
				XMStoreFloat3(&v, p);
				verts.push_back(v);
			}
			else if (prefix == "f") {
				std::vector<int> face;
				std::string token;
				while (s >> token)
					face.push_back(ParseFaceIndex(token, verts.size()));

				for (size_t i = 2; i < face.size(); i++) {
					tris.push_back({
						verts[face[0]],
						verts[face[i-1]],
						verts[face[i]], mat
					});
				}
			}
		}
	}
}
