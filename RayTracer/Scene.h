#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Triangle.h"
#include "Sphere.h"

namespace RayTracer {
	struct Scene {
		std::vector<Triangle> triangles;
		std::vector<Material> materials;
		std::vector<Sphere> spheres;
		DirectX::XMFLOAT3 cameraPos;
		float yaw, pitch, moveSpeed;
		bool useSky;
	};

	Scene GetEmptyCornellBox();
	Scene GetCubeCornellBox();
	Scene GetSphereCornellBox();
	Scene GetSuzanneCornellBox();
	Scene GetBunnyCornellBox();
	Scene GetDragonCornellBox();
	Scene GetSponzaScene();
}