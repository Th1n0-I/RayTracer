# RayTracer
A super duper generic ray tracer that was supposed to be not generic but Macondo is ending soon.

## Features
- Traces rays
  - Standard ray tracing approach, sends rays from the camera to sample light levels and make accurate light simulations.
- Bounding volume hierarchy for super performance
  - Bounding volumes with depth first search reduce intersection test from ~700k triangles to 15 or so AABB bound boxes and around 6 triangles.
- NEE light sampling to reduce noise
  - NEE samples a secondary light ray at each bounce so that we don't have to hope it eventually reaches light which greatly decreases noise when using a light.
- Materials for easy cutomization
  - Materials with color, emission, reflection, and roughness allow you to customize objects easily 
- Scenes for easy and organised testing
  - Scenes make it so you easily can swap between different scenes to test performance and apperance.
- Mesh loading to import large meshes
  - import meshes from OBJ files to easily make pretty scenes, currently have suzanne, stanford bunny and dragon, and the sponza scene.
- MultiThreading with threadPool
  - By using multiple CPU threads at the same time I was able to increase performance quite a lot. I also have a thread pool which means I don't have to request threads each frame.
 
## Performance
Performance was a big struggle since it's run on the cpu. This means I had to put a lot of effort into learning multiple optimisation techniques.

### BVH
BVH, or Bounding Volume Hierarchy, is a method to reduce the amount of intersections needed per ray. By engulfing the scene with a box, we can test if the ray intersects the scene. We then split the scene in half, by cheking which side of the scene we only have to test intersects with half the triangles. If we do this again and again and again we eventually only have a few triangles to test, average is around 6 or 7. This greatly reduces the time it takes to render especially when the scene contains a small super complex object like the stanford dragon. 

<img width="745" height="751" alt="image" src="https://github.com/user-attachments/assets/e9158b8c-4b0b-45f3-884f-800d2998867d" />

Test of BVH with stanford dragon. Green is 1 or less nodes visited. Black - white is 0 - 70 nodes visited. and red is > 70 nodes visited.

### NEE
NEE, or Next Event Estimation, is a method to reduce the amount of samples needed to denoise the image. This is done by sending a secondary ray towards a random point on a random light and checking if it's obstructed. If it isn't it adds the light energy from all the lights averaged with some things taken into consideration. By doing this over multiple samples it averages out and you get accurate lighting with much less noise.

<img width="457" height="454" alt="image" src="https://github.com/user-attachments/assets/f5f7e0ea-478f-4418-a925-93441e04a409" />

### Tests
 
When measuring performance SPS, or Samples Per Second is used, since I take multiple samples per frame to reduce presenting costs.

All tests were performed in the scenes that I shipped. Taking the first 100 samples as a sample size.

- Box scene: 20.17 SPS
- Mirror Sphere scene: 18.26 SPS
- Suzanne scene: 12.08 SPS
- Bunny scene: 14.70 SPS
- Dragon scene: 12.31 SPS
- Sponza scene: 1.44 SPS

The most impactful thing for performance is detail density, how much detail there is and how spread out it is. Even tough the dragon itself is more detailed thamn the whole sponza scene it is concentrated in a small part of the screen, which means most rays aren't impacted. In the sponza scene the detail is spread out and all rays are effectd.

## How to build
You open the .sln file in visual studio, switch configuration from Debug to Release if it is selected, and the press Build. Very simple 👍 

Thank you for reading my amazing README ❤️
