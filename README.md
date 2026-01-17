# 🐸 FROG Editor

FROG Editor is my first official attempt at SDL3, OpenGL and C++. Whilst I have attempted both OpenGL and C++ in the past under different contexts, I've never truly done everything from the ground up like this. 

*FROG Stands for First, RedEgs, Open, Gl* 

I've gotten quite annoyed that half my git projects are all called engines, so now im calling this one an editor 😂

# 📦 Technologies

I made this project using the following technologies:

- [OpenGL 3.3](https://www.khronos.org/opengl/) 
- [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)
- [SDL3-ttf](https://wiki.libsdl.org/SDL3_ttf/FrontPage)
- [SDL3-image](https://wiki.libsdl.org/SDL3_image/FrontPage)
- [assimp](https://github.com/assimp/assimp)
- [imgui](https://github.com/ocornut/imgui)
- [imguizmo](https://github.com/CedricGuillemet/ImGuizmo)

# ⭐ Features

The project is very early at the moment, I'm sure there's going to be plenty of rewrites, so its not very feature complete as of 17/01/2026. Currently the project has:

- **OBJ Model Loading:** You can load any .obj using assimp. So far other formats aren't supported but they will be!
- **Game Object Component System:** The engine doesn't utilise full ECS, it uses something more aligned to what unity has. Everything is an object, which is essentially just an ID and container. Every object can have components like a transform, shape, light etc. The components then control/affect the game objects
- **Scene System:** Game objects are contained to scenes which can be loaded and unloaded. 
- **Blinn-Phong Lighting:** The default lighting system in the engine a simple blinn-phong implementation (Which I will definitely be expanding).

# 🎯 Goals

I've got a few goals which I wish to hit with this engine, I'm still very new to OpenGL and C++ so my goals might seem quite optimisitc. *These are in no particular order*

- **Skeletal Animation:** I want to be able to have player characters which can be animated, so far only static objs can exist within the engine.
- **Wider Model Format Support:** So far only .obj's are supported because they're quite simple and robust file formats but inevitably i'm going to need a wider range if I wish to support more modern features.
- **Text Rendering:** So far there's no kind of UI facilities other than ImGui and thats mainly for the editor's purpose rather than for the game, so once I get to UI, text rendering is first on my chopping block.
- **Anti-Aliasing Support:** Right now the edges on models aren't that harsh really, but anti-aliasing will help deliver that extra visual polish and clarity, definitely not one of my priorities right now.
- **Shadow Mapping:** To help the lighting stop being so static, I wish to implement lighting. Currently I've got point lights and directional lights but shadows look far to unrealistic especially on planes.
- **Cube Maps:** Cubemaps will help bring my scenes together a bit and aren't too hard to implement, *i think*.
- **GPU Instancing:** I'm definitely going to need to implement this sooner rather than later, huge optimisation trick for voxel games and repetitive geometry like foliage.
- **PBR Lighting:** This is definitely not on my priority list, but would be very nice to have, It might even be one of my last features.
- **User Interface/2D facilities:** UI is essential to any game so this is a must. I will likely add it once i've figured out entirely what I want to do with my game object component system as thats foundational to the engine. 
