// RayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include "Window.h"
#include "Camera.h"
#include "Sphere.h"
#include "Random.h"

using namespace RayTracer;

DirectX::XMVECTOR SkyColor(DirectX::XMVECTOR dir) {
    const float t = 0.5f * (DirectX::XMVectorGetY(dir) + 1.0f);
    return DirectX::XMVectorLerp(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f),
                                 DirectX::XMVectorSet(0.5f, 0.7f, 1.0f, 0.0f), t);
}

DirectX::XMVECTOR TracePath(Ray ray, const std::vector<Sphere>& spheres,
    int maxBounces, uint32_t& rng) {
    DirectX::XMVECTOR throughput = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
    DirectX::XMVECTOR radiance = DirectX::XMVectorZero();

    for (int bounce = 0; bounce < maxBounces; bounce++) {
        HitData hit;
        bool hitAnything = false;
        for (const auto& s : spheres) {
            if (RaySphere(ray, s, 0.01f, hit.t, hit)) hitAnything = true;
        }

        if (!hitAnything) break;
        

        radiance = DirectX::XMVectorAdd(radiance,
            DirectX::XMVectorMultiply(throughput, hit.emission));

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
    }

    return radiance;
}

int main()
{
    auto lastTime = std::chrono::steady_clock::now();
    Window window(L"Sigma", 800, 600);
    Camera camera;

    std::vector<uint32_t> framebuffer;
    std::vector<DirectX::XMFLOAT3> accum;
    std::vector<Sphere> spheres = { 
        {{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, },{0.0f, 0.0f, 0.0f}, 1.0f,},
        {{{0.8f, 0.2f, 0.2f}},{0.0f, -101.0f, 0.0f}, 100.0f,},
        {{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.78f, 0.34f}, 1.0f, 0.05f},{2.0f, -0.5f, 0.0f}, 0.5f}
    };

    int sampleCount = 0;
    int frameCount = 0;

    DirectX::XMFLOAT3 prevPos = camera.position;
    float prevYaw = camera.yaw, prevPitch = camera.pitch;
    int prevW = 0, prevH = 0;
  

    float dt = 0.0f;

    while (window.ProccessMessages()) {
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

        sampleCount++;

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                const size_t i = (size_t)y * w + x;

                uint32_t rng = Hash((uint32_t)i ^ Hash((uint32_t)frameCount)) | 1u;

 
                const float s = (x + RandFloat(rng)) / (float)w;
                const float t = (h - 1 - y + RandFloat(rng)) / (float)h;

                Ray ray = camera.GetRay(s, t);

                DirectX::XMFLOAT3 c;
                DirectX::XMStoreFloat3(&c, TracePath(ray, spheres, 20, rng));

                accum[i].x += c.x;
                accum[i].y += c.y;
                accum[i].z += c.z;

                const float inv = 1.0f / (float)sampleCount;
                float rf = sqrt(accum[i].x * inv);
                float gf = sqrt(accum[i].y * inv);
                float bf = sqrt(accum[i].z * inv);

                if (rf > 1.0f) rf = 1.0f;
                if (gf > 1.0f) gf = 1.0f;
                if (bf > 1.0f) bf = 1.0f;

                framebuffer[i] = ((uint32_t)(255.0f * rf) << 16)
                    | ((uint32_t)(255.0f * gf) << 8)
                    | ((uint32_t)(255.0f * bf));
            }
        }
        window.Present(framebuffer.data(), w, h);

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

