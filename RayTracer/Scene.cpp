#include "Scene.h"
#include "Quad.h"
#include "Camera.h"
#include "Mesh.h"
#include "Cube.h"

namespace RayTracer {
	Scene GetEmptyCornellBox() {
		Scene s;

        s.cameraPos = { 278.0f, 273.0f, -800.0f };
        s.yaw = 0.0f;
        s.pitch = 0.0f;
        s.moveSpeed = 300.0f;


        s.useSky = false;

        Material defaultMatWhite = {
        {0.73f, 0.73f, 0.73f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultMatRed = {
            {0.63f, 0.065f, 0.05f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultMatGreen = {
            {0.14f, 0.45f, 0.091f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultLightWhite = {
            {0.0f, 0.0f, 0.0f}, {18.4f, 15.6f, 8.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultMirror = {
            {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 0.0f
        };

        std::vector<Material> materials = {
        defaultMatWhite,
        defaultMatGreen,
        defaultMatRed,
        defaultLightWhite,
        defaultMirror,
        };

        s.materials = materials;

        std::vector<DirectX::XMFLOAT3> verts = {
            {  0.0f,   0.0f,   0.0f}, // Left - Bottom - Forward - 0
            {  0.0f,   0.0f, 559.2f}, // Left - Bottom - Back - 1
            {  0.0f, 548.8f, 559.2f}, // Left - Top - Back - 2
            {  0.0f, 548.8f,   0.0f}, // Left - Top - Forward - 3
            {556.0f,   0.0f,   0.0f}, // Right - Bottom - Forward - 4
            {556.0f,   0.0f, 559.2f}, // Right - Bottom - Back - 5
            {556.0f, 548.8f, 559.2f}, // Right - Top - Back - 6
            {556.0f, 548.8f,   0.0f}, // Right - Top - Forward - 7
            // Light
            {213.0f, 548.8f, 227.0f}, // Left - Forward - 8
            {213.0f, 548.8f, 332.0f}, // Left - Back - 9
            {343.0f, 548.8f, 332.0f}, // Right - Back - 10
            {343.0f, 548.8f, 227.0f}, // Right - Forward - 11
            // Ceiling seams
            {213.0f, 548.8f,   0.0f}, // Left - Forward - 12
            {  0.0f, 548.8f, 332.0f}, // Left - Back - 13
            {343.0f, 548.8f, 559.2f}, // Right - Back - 14
            {556.0f, 548.8f, 227.0f}, // Right - Forward - 15
            // Quad test
            {456.0f,  80.0f, 500.0f},
            {456.0f, 440.0f, 200.0f},
            {100.0f, 440.0f, 500.0f},
            {100.0f,  80.0f, 500.0f},
        };

        std::vector<Triangle> triangles = {};

        int subX = 4;
        int subY = 4;

        std::vector<Quad> quads{{verts[0],verts[1],verts[2],verts[3], 2, triangles, subX, subY},
        { verts[4],verts[5],verts[6],verts[7], 1, triangles, subX, subY },
        { verts[1],verts[2],verts[6],verts[5], 0, triangles, subX, subY },
        { verts[8],verts[11],verts[10],verts[9], 3, triangles, 1, 1 },
        { verts[3],verts[12],verts[9],verts[13], 0, triangles, subX, subY },
        { verts[13],verts[10],verts[14],verts[2], 0, triangles, subX, subY },
        { verts[11],verts[15],verts[6],verts[14], 0, triangles, subX, subY },
        { verts[12],verts[7],verts[15],verts[8], 0, triangles, subX, subY },
        { verts[0],verts[1],verts[5],verts[4], 0, triangles, subX, subY }};

        s.triangles = triangles;

        std::vector<Sphere> spheres = {};

        s.spheres = spheres;


        return s;
	}

    Scene GetSphereCornellBox() {
        auto s = GetEmptyCornellBox();
        s.spheres = {
            { 4, {170.0f, 110.0f, 280.0f}, 110.0f },
            { 4, {390.0f, 110.0f, 280.0f}, 110.0f },
        };
        return s;
    }

    Scene GetSuzanneCornellBox() {
        auto s = GetEmptyCornellBox();
        Mesh suzanne({ 28.6f, 124.8f, 690.4f }, { 100.0f, 100.0f, 100.0f },
            { 0.0f, 180.0f, 0.0f }, 0, s.triangles, "meshFiles/suzanne.obj");
        return s;
    }

    Scene GetBunnyCornellBox() {
        auto s = GetEmptyCornellBox();
        Mesh Bunny{{ 278.0f, -4.0f, 280.0f }, { 170.0f, 170.0f, 170.0f }, { 0.0f, -90.0f, 0.0f }, 0, s.triangles, "meshFiles/bunny.obj"};
        return s;
    }

    Scene GetDragonCornellBox() {
        auto s = GetEmptyCornellBox();
        Mesh dragon{ { 268.0f, 99.0f, 274.0f }, { 350.0f, 350.0f, 350.0f }, { 0.0f, 90.0f, 0.0f },	0, s.triangles, "meshFiles/dragon.obj" };
        return s;
    }

    Scene GetSponzaScene() {
        Scene s{};
        s.triangles = {};
        s.spheres = {};

        Material defaultMatWhite = {
        {0.73f, 0.73f, 0.73f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultMatRed = {
            {0.63f, 0.065f, 0.05f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultMatGreen = {
            {0.14f, 0.45f, 0.091f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        Material defaultLightWhite = {
            {0.0f, 0.0f, 0.0f}, {18.4f, 15.6f, 8.0f}, {0.0f, 0.0f, 0.0f}, 0.0f, 0.0f
        };

        std::vector<Material> materials = {
        defaultMatWhite,
        defaultMatGreen,
        defaultMatRed,
        defaultLightWhite,
        };

        s.materials = materials;

        Mesh Sponza{ {0, 126, 0} ,{1, 1, 1} ,{0, 0, 0} , 0 , s.triangles, "meshFiles/sponza.obj" };
        s.cameraPos = { -1300.0f, 300.0f, -38.0f };
        s.yaw = 1.57f; s.pitch = 0.0f; s.moveSpeed = 600.0f;
        s.useSky = true;
        return s;
    }
}