# ToyGL Renderer
ToyGL is a hobby renderer I started developing during my spare time, written in C++11 and using OpenGL 4 API.  <br>

![alt text](https://github.com/embooo/toygl/blob/main/gallery/BlinnPhong-Mipmaps.PNG?raw=true)
*Current progress 04/10/21 : Crytek Sponza, with base color texture, mip mapping and a directional + ambient light.

## Motivations
My main motivation is to learn more about rendering and implement cool techniques/shaders/effects, as it is a passion of mine. <br/> It is also an opportunity for me to apply what I'll learn during my 2nd year of master's degree in computer graphics.

## Features
Currently, ToyGL is a forward renderer but the goal is to implement the [Forward+](https://takahiroharada.files.wordpress.com/2015/04/forward_plus.pdf) method to render multiple light sources while maintaining real-time performance.

### General
- [x] Free Camera
- [x] Keyboard/Mouse events handling
- [x] GLSL shader loading/recompiling 
- [x] User interface (ImGui) 

###  Model loading
- [x] Custom glTF 2.0 model loader (loading materials, textures, transforms)

### Lighting 
- [x] Lambert BRDF
- [x] Blinn-Phong BRDF
- [x] Physically based rendering - Microfacet BRDF (Cook-Torrance)
- [x] Point lights, directional lights
- [x] Normal mapping
- [x] Gamma correction / Lighting calculations in linear space
- [ ] Shadow mapping 

### Rendering methods
- [ ] Deferred rendering 
- [ ] Forward+ 

### Geometry culling methods
- [ ] Frustum culling

### Resources
[Real Shading in Unreal Engine 4](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf) (Brian Karis, Epic Games) <br/>
[Chapter 24. The Importance of Being Linear](https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-24-importance-being-linear) (L.Gritz, E. d'Eon, GPU Gems 3)


