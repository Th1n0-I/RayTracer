#include "Sphere.h"

using namespace DirectX;

namespace RayTracer {
	bool RaySphere(const Ray& ray, const Sphere& sphere, float tMin, float tMax, HitData& data) {
		const XMVECTOR center = XMLoadFloat3(&sphere.pos);

		// The position of the ray origin with the sphere center as (0, 0)
		const XMVECTOR L = XMVectorSubtract(ray.position, center);

		// (Q - O) * D distance along any ray with origin O and direction D that is closest to point Q
		// L = (O - Q) which means h = (O - Q) * D which means that -h = -(O - Q) * D = (Q - O) * D
		// Thus -h is the distance along the ray when it is closest to the sphere center
		const float h = XMVectorGetX(XMVector3Dot(L, ray.direction));

		// c is the squared distance between the ray origin and the sphere edge
		// we get the squared distance because getting the not squared distance is like impossible or something idk
		const float c = XMVectorGetX(XMVector3Dot(L, L)) - sphere.radius * sphere.radius;

		// since -h is the distance between the ray origin and sphere center
		// and sqrt(c) is length of a tangent from the origin to the sphere
		// if h^2 - c < 0 the distance from the surface is greater than the distance where the ray is closest
		// this means that the ray did not hit

		// if h^2 - c = 0 the distances are the same
		// this means that the ray graces the surface

		// if h^2 - c > 0 the distance from the surface is less than the distance where the ray is closest
		// this means that the ray goes inside the sphere
 		const float discriminant = h * h - c;

		// return no hit if the ray does not intersect the sphere
		if (discriminant < 0.0f) return false;

		// the distance from -h where the ray enters or exits the sphere
		const float sqrtD = sqrtf(discriminant);

		// go backwards from the closest point to the point where the ray enters the sphere
		// remember that h is reversed so we have to use -h
		float t = -h - sqrtD;

		// the intersect either happens behind the rays start or behind a previously recorded hit
		if (t < tMin || t > tMax) {

			// go forwards from -h to where the ray exits the sphere
			t = -h + sqrtD;

			// if both hits are invalid return no hit
			if (t < tMin || t > tMax) return false;
		}

		// store the hit distance
		data.t = t;

		// store the hit point using O + t * D
		data.point = XMVectorMultiplyAdd(ray.direction, XMVectorReplicate(t), ray.position);

		// store the hit normal using normalize(point - center) 
		data.normal = XMVectorScale(XMVectorSubtract(data.point, center), 1.0f / sphere.radius);

		// store the color of the sphere in the hitData
		data.color = XMVectorSet(sphere.material.color.x, sphere.material.color.y, sphere.material.color.z, 0.0f);

		return true;
	}
}
