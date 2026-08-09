#include "Triangle.h"

using namespace DirectX;

namespace RayTracer {
	bool RayTriangle(const Ray& ray, const Triangle& triangle, float tMin, float tMax, HitData& data) {
		const XMVECTOR v0 = XMLoadFloat3(&triangle.v0);
		const XMVECTOR e1 = XMVectorSubtract(XMLoadFloat3(&triangle.v1), v0);
		const XMVECTOR e2 = XMVectorSubtract(XMLoadFloat3(&triangle.v2), v0);

		const XMVECTOR pvec = XMVector3Cross(ray.direction, e2);
		const float det = XMVectorGetX(XMVector3Dot(e1, pvec));

		if (fabsf(det) < 1e-8f) return false;

		const float invDet = 1.0f / det;
		const XMVECTOR tvec = XMVectorSubtract(ray.position, v0);

		const float u = XMVectorGetX(XMVector3Dot(tvec, pvec)) * invDet;
		if (u < 0.0f || u > 1.0f) return false;

		const XMVECTOR qvec = XMVector3Cross(tvec, e1);

		const float v = XMVectorGetX(XMVector3Dot(ray.direction, qvec)) * invDet;
		if (v < 0.0f || u + v > 1.0f) return false;

		const float t = XMVectorGetX(XMVector3Dot(e2, qvec)) * invDet;
		if (t < tMin || t > tMax) return false;

		data.t = t;
		data.point = XMVectorMultiplyAdd(ray.direction, XMVectorReplicate(t), ray.position);

		XMVECTOR n = XMVector3Normalize(XMVector3Cross(e1, e2));
		if (XMVectorGetX(XMVector3Dot(n, ray.direction)) > 0.0f) n = XMVectorNegate(n);
		data.normal = n;

		data.color = XMVectorSet(triangle.material.color.x, triangle.material.color.y, triangle.material.color.z, 0.0f);

		data.emission = XMVectorSet(triangle.material.emissionColor.x,
			triangle.material.emissionColor.y,
			triangle.material.emissionColor.z, 0.0f);

		data.specularColor = XMVectorSet(triangle.material.specularColor.x,
			triangle.material.specularColor.y,
			triangle.material.specularColor.z, 0.0f);

		data.specularChance = triangle.material.specularChance;
		data.roughness = triangle.material.roughness;

		return true;
	}
}