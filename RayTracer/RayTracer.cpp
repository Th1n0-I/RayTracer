// RayTracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include "Window.h"
#include "Camera.h"

using namespace RayTracer;

int main()
{
    auto lastTime = std::chrono::steady_clock::now();
    Window window(L"Sigma", 800, 600);
    Camera camera;

    std::vector<uint32_t> framebuffer;

    while (window.ProccessMessages()) {
        int dx = 0; int dy = 0;
        window.UpdateMouseLock(dx, dy);
        if (dx || dy) camera.Rotate((float)dx, (float)dy);

        camera.Update(window.GetInput(), 1.0 / 60.0f);
        camera.SetAspect(window.Width(), window.Height());
        camera.CalculateViewPlane();

        const int w = window.Width();
        const int h = window.Height();
        if (w <= 0 || h <= 0) continue;
        framebuffer.resize((size_t)w * h);

        for (int y = 0; y < h; y++) {
            const float t = (h - 1 - y) / (float)h;
            for (int x = 0; x < w; x++) {
                const float s = x / (float)w;

                Ray ray = camera.GetRay(s, t);

                DirectX::XMFLOAT3 d;
                DirectX::XMStoreFloat3(&d, ray.direction);

                const uint32_t r = (uint32_t)(255.0f * (0.5f * d.x + 0.5f));
                const uint32_t g = (uint32_t)(255.0f * (0.5f * d.y + 0.5f));
                const uint32_t b = (uint32_t)(255.0f * (0.5f * d.z + 0.5f));

                framebuffer[(size_t)y * w + x] = (r << 16) | (g << 8) | b;
            }
        }
        window.Present(framebuffer.data(), w, h);

        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
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

