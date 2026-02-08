#pragma once
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

#include "core/camera.h"
#include "core/imgui_editor.h"
#include "core/scene.h"
#include "core/scenemanager.h"
#include "graphics/cubemap.h"
#include "graphics/framebuffer.h"
#include "graphics/gbuffer.h"
#include "graphics/model.h"
#include "graphics/shader.h"
#include "graphics/shadowmap.h"
#include "graphics/texture.h"
#include "graphics/uniform_buffer_object.h"


class Game {

public:
    Game(int argc, char ** argv);
    SDL_AppResult Init();
    SDL_AppResult Iterate();
    SDL_AppResult Event(SDL_Event *event);
    void Quit(SDL_AppResult result) const;

private:
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> m_window;
    SDL_GLContextState *gl_context;
    SceneManager scene_manager;
    std::unique_ptr<Editor> editor;

    std::vector<Shader*> shaders;
    std::unique_ptr<Cubemap> cubemap_;
    std::unique_ptr<UniformBufferObject> ubo;
    std::unique_ptr<GBuffer> gbuffer;
    std::unique_ptr<Shadowmap> shadowmap;
    bool forward_renderer = false;

    float delta_time = 0.0f;
    float fps = 0.0f;
    int _frames = 0.0f;

    Uint64 _freq = SDL_GetPerformanceFrequency();
    Uint64 _last = SDL_GetPerformanceCounter();

    SDL_AppResult setup_window();

    SDL_AppResult OnQuit();
    SDL_AppResult OnRender();
    SDL_AppResult OnUpdate();

    void render_scene();
    void render_imgui();

    void update_trackframetime();

};
