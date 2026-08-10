// RayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <thread>
#include "Window.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Cube.h"
#include "Random.h"

using namespace RayTracer;

const static float PI = 3.14159f;

struct LightRef {
    bool isTriangle;
    int index;
};

struct LightSample {
    DirectX::XMVECTOR point;
    DirectX::XMVECTOR normal;
    DirectX::XMVECTOR emission;
    float invPdf;
};

DirectX::XMVECTOR SkyColor(DirectX::XMVECTOR dir) {
    const float t = 0.5f * (DirectX::XMVectorGetY(dir) + 1.0f);
    return DirectX::XMVectorLerp(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
                                 DirectX::XMVectorSet(0.5f, 0.7f, 1.0f, 0.0f), t);
}

LightSample SampleSphereLight(const Sphere& sphere, DirectX::XMVECTOR from, uint32_t& rng) {
    const DirectX::XMVECTOR center = DirectX::XMLoadFloat3(&sphere.pos);
    const DirectX::XMVECTOR towardLight = DirectX::XMVector3Normalize(
        DirectX::XMVectorSubtract(center, from));
    DirectX::XMVECTOR dir = RandomUnitVector(rng);
    if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(dir, towardLight)) > 0.0f) dir = DirectX::XMVectorNegate(dir);

    LightSample ls{};
    ls.point = DirectX::XMVectorMultiplyAdd(dir, DirectX::XMVectorReplicate(sphere.radius), center);
    ls.normal = dir;
   
    ls.emission = DirectX::XMLoadFloat3(&sphere.material.emissionColor);
    ls.invPdf = 2.0f * sphere.radius * sphere.radius * PI;
    return ls;
}

LightSample SampleTriangleLight(const Triangle& tri, DirectX::XMVECTOR from, uint32_t& rng) {
    LightSample ls{};
    const DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&tri.v0);
    const DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&tri.v1), v0);
    const DirectX::XMVECTOR e2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&tri.v2), v0);

    float u = RandFloat(rng);
    float v = RandFloat(rng);

    if (u + v > 1.0f) { u = 1.0f - u; v = 1.0 - v; }

    ls.point = DirectX::XMVectorAdd(v0,
        DirectX::XMVectorAdd(DirectX::XMVectorScale(e1, u), DirectX::XMVectorScale(e2, v)));

    const DirectX::XMVECTOR cross = DirectX::XMVector3Cross(e1, e2);
    ls.normal = DirectX::XMVector3Normalize(cross);
    const float area = 0.5f * DirectX::XMVectorGetX(DirectX::XMVector3Length(cross));

    ls.invPdf = area;
    ls.emission = DirectX::XMLoadFloat3(&tri.material.emissionColor);
    return ls;
}


DirectX::XMVECTOR sampleDirectLight(DirectX::XMVECTOR point, DirectX::XMVECTOR normal,
    const std::vector<Sphere>& spheres, const std::vector<Triangle>& triangles, const std::vector<Cube>& cubes,
    const std::vector<LightRef>& lights, uint32_t& rng) {

    if (lights.empty()) return DirectX::XMVectorZero();

    // Choses a random light
    const int idx = (int)(RandFloat(rng) * lights.size());
    const bool isTriangle = lights[idx].isTriangle;
    LightSample light{};
    if (isTriangle) {
        light = SampleTriangleLight(triangles[lights[idx].index], point, rng);
    }else
    {
        light = SampleSphereLight( spheres[lights[idx].index], point, rng);
    }

    // Gets the amount of lights and multiplies value by that amount, will average out.
    const float lightCountScale = (float)lights.size();

    // The direction from the sample point to the light
    DirectX::XMVECTOR toLight = DirectX::XMVectorSubtract(light.point, point);
    const float dist2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(toLight, toLight));
    const float dist = sqrt(dist2);
    toLight = DirectX::XMVectorScale(toLight, 1.0f / dist);

    // How lined up the ray is with the surfaces
    const float cosSurface = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, toLight));
    const float cosLight = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(light.normal, toLight));
    if (cosSurface <= 0.0f || cosLight <= 0.0f) return DirectX::XMVectorZero();

    // Check if the light is blocked
    Ray shadow{ point, toLight };
    HitData block;
    block.t = dist - 0.001f;
    for (const auto& s : spheres) {
        if (RaySphere(shadow, s, 0.001f, block.t, block)) return DirectX::XMVectorZero();
    }

    for (const auto& t : triangles) {
        if (RayTriangle(shadow, t, 0.001f, block.t, block)) return DirectX::XMVectorZero();
    }

    
    // Add a bunch of terms to see how much light to add
    const float geom = cosSurface * cosLight * light.invPdf / (dist2 * PI);

    return DirectX::XMVectorScale(light.emission, geom * lightCountScale);
}

DirectX::XMVECTOR TracePath(Ray ray, const std::vector<Sphere>& spheres, const std::vector<Triangle>& triangles, std::vector<Cube>& cubes, const std::vector<LightRef>& lights,
    int maxBounces, uint32_t& rng) {
    DirectX::XMVECTOR throughput = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR radiance = DirectX::XMVectorZero();

    bool takeEmission = true;

    for (int bounce = 0; bounce < maxBounces; bounce++) {
        HitData hit;
        bool hitAnything = false;
        for (const auto& s : spheres) {
            if (RaySphere(ray, s, 0.01f, hit.t, hit)) hitAnything = true;
        }

        for (const auto& t : triangles) {
            if (RayTriangle(ray, t, 0.01f, hit.t, hit)) hitAnything = true;
        }

        if (!hitAnything) {
            //radiance = DirectX::XMVectorAdd(radiance,
                //DirectX::XMVectorMultiply(throughput, SkyColor(ray.direction)));
            break;
        }

        if (takeEmission) {
            radiance = DirectX::XMVectorAdd(radiance,
                DirectX::XMVectorMultiply(throughput, hit.emission));
        }

        if (hit.specularChance < 1.0f) {
            DirectX::XMVECTOR direct = sampleDirectLight(hit.point, hit.normal, spheres, triangles, cubes, lights, rng);
            direct = DirectX::XMVectorMultiply(direct, hit.color);
            direct = DirectX::XMVectorScale(direct, 1.0f - hit.specularChance);
            radiance = DirectX::XMVectorAdd(radiance, DirectX::XMVectorMultiply(throughput, direct));
        }
       
        ray.position = hit.point;

        const bool doSpecular = RandFloat(rng) < hit.specularChance;

        const DirectX::XMVECTOR diffuseDir = DirectX::XMVector3Normalize(
            DirectX::XMVectorAdd(hit.normal, RandomUnitVector(rng)));
        DirectX::XMVECTOR specularDir = ray.direction = DirectX::XMVector3Reflect(ray.direction, hit.normal);
        specularDir = DirectX::XMVector3Normalize(DirectX::XMVectorLerp(specularDir, diffuseDir, hit.roughness * hit.roughness));

        if (doSpecular) {
            throughput = DirectX::XMVectorMultiply(throughput,
                DirectX::XMVectorScale(hit.specularColor, 1.0f / hit.specularChance));
            ray.direction = specularDir;
        }
        else {
            throughput = DirectX::XMVectorMultiply(throughput,
                DirectX::XMVectorScale(hit.color, 1.0f / (1.0f - hit.specularChance)));
            ray.direction = diffuseDir;
        }

        takeEmission = doSpecular;

        if (bounce > 2) {
            float p = fmaxf(DirectX::XMVectorGetX(throughput),
                fmaxf(DirectX::XMVectorGetY(throughput),
                    DirectX::XMVectorGetZ(throughput)));
            p = fminf(p, 0.95f);

            if (RandFloat(rng) > p) break;

            throughput = DirectX::XMVectorScale(throughput, 1.0f / p);
        }
    }

    return radiance;
}

int main()
{
    

    auto lastTime = std::chrono::steady_clock::now();
    Window window(L"Sigma", 600, 600);
    Camera camera;
    camera.position = { 0.0f, 0.0f, -5.0f };
    Material defaultMatWhite = {
        {0.9f, 0.9f, 0.9f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
    };

    Material defaultMatRed = {
        {0.9f, 0.2f, 0.2f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
    };

    Material defaultMatGreen = {
        {0.2f, 0.9f, 0.2f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
    };

    Material defaultLightWhite = {
        {0.0f, 0.0f, 0.0f}, {6.0f, 6.0f, 6.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
    };

    std::vector<uint32_t> framebuffer;
    std::vector<DirectX::XMFLOAT3> accum;
    std::vector<Sphere> spheres = { 
        //{{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, },{0.0f, 1.0f, 0.0f}, 1.0f,},
    };

    std::vector<DirectX::XMFLOAT3> verts = {
        {-2.0f, -2.0f, -2.0f}, //0 
        { 2.0f, -2.0f, -2.0f}, //1 
        {-2.0f, -2.0f,  2.0f}, //2 
        { 2.0f, -2.0f,  2.0f}, //3 
        {-2.0f,  2.0f,  2.0f}, //4
        {-2.0f,  2.0f, -2.0f}, //5
        { 2.0f,  2.0f,  2.0f}, //6
        { 2.0f,  2.0f, -2.0f}, //7

        {-0.5f, 1.98f, -0.5f}, // light 0 - 8
        {0.5f, 1.98f, -0.5f}, // light 1 - 9
        {0.5f, 1.98f, 0.5f}, // light 2 - 10
        {- 0.5f, 1.98f, 0.5f}, // light 3 - 11 
    };

    std::vector<Triangle> triangles = {
        {verts[0], verts[1], verts[2], defaultMatWhite},
        {verts[3], verts[1], verts[2], defaultMatWhite},
        {verts[0], verts[2], verts[4], defaultMatRed},
        {verts[0], verts[5], verts[4], defaultMatRed},
        {verts[2], verts[6], verts[4], defaultMatWhite},
        {verts[2], verts[6], verts[3], defaultMatWhite},
        {verts[6], verts[1], verts[3], defaultMatGreen},
        {verts[6], verts[1], verts[7], defaultMatGreen},
        {verts[5], verts[6], verts[4], defaultMatWhite},
        {verts[5], verts[6], verts[7], defaultMatWhite},
        {verts[8], verts[9], verts[10], defaultLightWhite},
        {verts[8], verts[10], verts[11], defaultLightWhite},
    };

    std::vector<Cube> cubes = {
        {{0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 45.0f, 0.0f}, defaultMatWhite, triangles},
    };

    std::vector<LightRef> lights;
    for (int i = 0; i < (int)spheres.size(); i++) {
        const auto& e = spheres[i].material.emissionColor;
        if (e.x > 0.0f || e.y > 0.0f || e.z > 0.0f) {
            lights.push_back({false, i});
        }
    }
    for (int i = 0; i < (int)triangles.size(); i++) {
        const auto& e = triangles[i].material.emissionColor;
        if (e.x > 0.0f || e.y > 0.0f || e.z > 0.0f) {
            lights.push_back({ true, i });
        }
    }

    int sampleCount = 0;
    int frameCount = 0;

    DirectX::XMFLOAT3 prevPos = camera.position;
    float prevYaw = camera.yaw, prevPitch = camera.pitch;
    int prevW = 0, prevH = 0;
  

    float dt = 0.0f;

    while (window.ProccessMessages()) {
        auto t0 = std::chrono::steady_clock::now();
        frameCount++;

        int dx = 0; int dy = 0;
        window.UpdateMouseLock(dx, dy);
        if (dx || dy) camera.Rotate((float)dx, (float)dy);

        camera.Update(window.GetInput(), dt);
        camera.SetAspect(window.Width(), window.Height());
        camera.CalculateViewPlane();

        const int w = window.Width();
        const int h = window.Height();
        if (w <= 0 || h <= 0) continue;

        const bool moved =
            camera.position.x != prevPos.x || camera.position.y != prevPos.y || camera.position.z != prevPos.z ||
            camera.yaw != prevYaw || camera.pitch != prevPitch || w != prevW || h != prevH;

        if (moved) {
            accum.assign((size_t)w * h, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
            framebuffer.resize((size_t)w * h);
            sampleCount = 0;
            prevPos = camera.position;
            prevYaw = camera.yaw;
            prevPitch = camera.pitch;
            prevW = w; prevH = h;
        }

        const int samplesPerFrame = moved ? 1 : 4;

        const unsigned threadCount = std::max(1u, std::thread::hardware_concurrency());
        std::vector<std::thread> workers;
        workers.reserve(threadCount);
        sampleCount += samplesPerFrame;
        for (unsigned ti = 0; ti < threadCount; ti++) {
            workers.emplace_back([&, ti] {
                for (int y = (int)ti; y < h; y += (int)threadCount) {
                    for (int x = 0; x < w; x++) {
                        const size_t idx = (size_t)y * w + x;

                        uint32_t rng = Hash((uint32_t)idx ^ Hash((uint32_t)frameCount)) | 1u;

                        DirectX::XMFLOAT3 sum{ 0.0f, 0.0f, 0.0f };
                        for(int sp = 0; sp < samplesPerFrame; sp++){
                            const float s = (x + RandFloat(rng)) / (float)w;
                            const float t = (h - 1 - y + RandFloat(rng)) / (float)h;

                            DirectX::XMFLOAT3 c;
                            DirectX::XMStoreFloat3(&c, TracePath(camera.GetRay(s,t), spheres, triangles, cubes, lights, 32, rng));
                            sum.x += c.x; sum.y += c.y; sum.z += c.z;
                        }

                        accum[idx].x += sum.x;
                        accum[idx].y += sum.y;
                        accum[idx].z += sum.z;

                        const float inv = 1.0f / (float)sampleCount;
                        float rf = sqrt(accum[idx].x * inv);
                        float gf = sqrt(accum[idx].y * inv);
                        float bf = sqrt(accum[idx].z * inv);

                        if (rf > 1.0f) rf = 1.0f;
                        if (gf > 1.0f) gf = 1.0f;
                        if (bf > 1.0f) bf = 1.0f;

                        framebuffer[idx] = ((uint32_t)(255.0f * rf) << 16)
                            | ((uint32_t)(255.0f * gf) << 8)
                            | ((uint32_t)(255.0f * bf));
                    }
                }
            });
        }

        for (auto& t : workers) {
            t.join();
        }
        auto t1 = std::chrono::steady_clock::now();
        window.Present(framebuffer.data(), w, h);
        auto t2 = std::chrono::steady_clock::now();

        printf("render %.2f ms | present %.2f ms | spp %d | fps %.2f | sps %.2f\n",
            std::chrono::duration<float, std::milli>(t1 - t0).count(),
            std::chrono::duration<float, std::milli>(t2 - t1).count(),
            sampleCount,
            1.0f / std::chrono::duration<float>(t2 - t0).count(),
            1.0f / std::chrono::duration<float>(t2 - t0).count() * samplesPerFrame);


        auto now = std::chrono::steady_clock::now();
        dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        printf("%.2f ms (%.0f fps)\n", dt * 1000.0f, 1.0f / dt);
        
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

