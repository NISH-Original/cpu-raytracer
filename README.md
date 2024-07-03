# CPU Based Raytracer in C++

## Overview

This project is a basic raytracer implemented in C++. A raytracer is a rendering algorithm that simulates the way light interacts with objects in a virtual scene to produce realistic images. By tracing the paths of rays of light as they travel through the scene, the raytracer can determine the color and intensity of the light that reaches the camera from each point in the scene.

## Features

- **Ray-Sphere Intersection:** The raytracer can detect intersections between rays and spheres, which are used as the primary geometric objects in the scene.
- **Basic Reflection:** The raytracer includes basic reflection calculations, enabling it to render reflective surfaces.
- **Phong Shading:** The project implements Phong shading to simulate the way light reflects off surfaces, providing a more realistic rendering of objects.
- **Materials:** It also supports modifying the aspects of the material such as Albedo, Roughness, and Emission for a wider variety of renderable objects.
- **Multiple Light Sources:** The raytracer supports multiple light sources using emission materials, allowing for complex lighting setups and more dynamic scenes.
- **Interactive Camera and UI:** The application also has an interactive camera to move around the scene and UI to modify the objects in the scene.

## Future Plans

- **Using the GPU:** Leveraging the Vulcan API to use the GPU for faster render times, because the CPU is not best optimised for raytracing applications.
- **More Geometry:** Adding support for various other 3D objects such as Quadrilaterals.
- **Advanced Lighting:** Implementing more advanced lighting phenomena such as refraction.

## Some Renders

**Here are some renders from the current version of the engine:**

First iteration of the renderer, capable of rendering multiple spheres at once:

![v2_rendering_multi_spheres](https://github.com/NISH-Original/raytracing_test/assets/75113251/0ceddbc4-3303-43df-9001-8fa317d46e04)


Basic reflections:

![v3_basic_reflections](https://github.com/NISH-Original/raytracing_test/assets/75113251/f24043c6-caeb-4155-8e79-c5b984197a07)


Roughness without Path Tracing:

![v4_basic_roughness](https://github.com/NISH-Original/raytracing_test/assets/75113251/f3da70fe-6f18-46e2-96c0-afe76068bde1)


Roughness with Path Tracing:

![v5_denoising_path_tracing](https://github.com/NISH-Original/raytracing_test/assets/75113251/30eb9f76-b9c8-4925-9267-31c0bdc7819f)


Emission and Diffuse:

![emission](https://github.com/NISH-Original/raytracing_test/assets/75113251/4edb6fa1-3899-444f-ac81-9c20f86e22ce)


![emission_v2](https://github.com/NISH-Original/raytracing_test/assets/75113251/496693c7-893a-42e3-af40-8844bb7ddc4b)
