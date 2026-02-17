//
// Created by Charlie on 05/01/2026.
//

#include "Game.h"

#include <array>
#include <chrono>
#include <format>
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <SDL3_image/SDL_image.h>
#include <imgui.h>
#include <ImGuizmo.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

#include "core/camera.h"
#include "game/test_scene.h"
#include "graphics/model.h"
#include "imgui_stdlib.h"
#include "core/components/sky.h"
#include "core/components/billboard.h"
#include "graphics/cubemap.h"
#include "graphics/quad.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "utility/debug.h"

#define RESOURCES(path) "../resources/shaders/" path
#define glCheckError__() Debug::glCheckError_(__FILE__, __LINE__)

Game::Game(int argc, char ** argv) : m_window(nullptr, &SDL_DestroyWindow), gl_context(nullptr) {
}

SDL_AppResult Game::Init() {
    setup_window();


    scene_manager = SceneManager();
    renderer = std::make_unique<Renderer>();
    editor = std::make_unique<Editor>(&scene_manager);

    ubo = std::make_unique<UniformBufferObject>(160, 0);
    std::cout << "Made UBO" << std::endl;
    glCheckError__();

    Shader *shader_prog = new Shader(RESOURCES("forward-renderer/vertex.glsl"), RESOURCES("forward-renderer/fragment.glsl"), "forward-renderer");
    Shader *cubemap_shader = new Shader(RESOURCES("cubemap/cubemapvert.glsl"), RESOURCES("cubemap/cubemapfrag.glsl"), "skybox");
    Shader *billboard_shader = new Shader(RESOURCES("billboard/bvertex.glsl"), RESOURCES("billboard/bfragment.glsl"), "billboard");
    Shader *quad_shader = new Shader(RESOURCES("quad-2d/vertex.glsl"), RESOURCES("quad-2d/fragment.glsl"), "quad-2d");
    Shader *lightpass_shader = new Shader(RESOURCES("lightpass/vertex.glsl"), RESOURCES("lightpass/fragment.glsl"), "light-pass");
    Shader *geometrypass_shader = new Shader(RESOURCES("gpass/vertex.glsl"), RESOURCES("gpass/fragment.glsl"), "geometry-pass");
    Shader *shadowdepth_shader = new Shader(RESOURCES("shadowdepth/vertex.glsl"), RESOURCES("shadowdepth/fragment.glsl"), "shadow-pass");
    Shader *shadowmap_shader = new Shader(RESOURCES("shadowmap/vertex.glsl"), RESOURCES("shadowmap/fragment.glsl"), "shadow-renderer");
    std::cout << "Compiled shaders" << std::endl;
    glCheckError__();

    gbuffer = std::make_unique<GBuffer>(1280, 720);
    std::cout << "Made Gbuffer" << std::endl;
    glCheckError__();

    shaders.emplace_back(shader_prog);
    shaders.emplace_back(cubemap_shader);
    shaders.emplace_back(billboard_shader);
    shaders.emplace_back(quad_shader);
    shaders.emplace_back(lightpass_shader);
    shaders.emplace_back(geometrypass_shader);
    shaders.emplace_back(shadowdepth_shader);
    shaders.emplace_back(shadowmap_shader);

    std::cout << "Cached shaders" << std::endl;

    int scene = scene_manager.add_scene(std::make_unique<Test_Scene>());
    scene_manager.set_scene(scene);
    std::cout << "Set Scene" << std::endl;

    auto lights = renderer->collect_light_source_components(&scene_manager);
    for (int i = 0; i < lights.size(); i++) {

        Shadowmap s = Shadowmap(1024);
        shadowmaps.emplace_back(s);
        std::cout << "Set Shadowmap " << i << std::endl;
    }

    std::cout << "Finished Init" << std::endl;
    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::setup_window()   {
    SDL_SetAppMetadata("Example SDL3", "1.0.0", "com.redegs");
    int width = 1280; int height = 720;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    m_window.reset(SDL_CreateWindow("Example SDL3 OpenGL", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));
    if (!m_window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetSwapInterval(0);
    SDL_GLContextState *gl_context = SDL_GL_CreateContext(m_window.get());

    if (!gl_context) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialise OpenGL context state: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_GL_MakeCurrent(m_window.get(), gl_context)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to make current OpenGL context: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!gladLoadGLLoader( reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress) )) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialise GLAD: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)

    ImGui_ImplSDL3_InitForOpenGL(m_window.get(), gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    glViewport(0, 0, width, height);
    glCheckError__();

    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::Iterate() {
    update_trackframetime();
    if (auto result = OnUpdate(); result != SDL_APP_CONTINUE) {
        return result;
    }
    if (auto result = OnRender(); result != SDL_APP_CONTINUE) {
        return result;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult Game::Event(SDL_Event *event) {
    ImGui_ImplSDL3_ProcessEvent(event);
    scene_manager.events(event);
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return OnQuit();
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (SDL_GetWindowID(m_window.get()) == event->window.windowID)
                return OnQuit();

        case SDL_EVENT_WINDOW_RESIZED:
            int width, height;
            SDL_GetWindowSizeInPixels(m_window.get(), &width, &height);
            glViewport(0, 0, width, height);
        case SDL_EVENT_KEY_DOWN:
            switch (event->key.key) {
                case SDLK_TAB:
                    scene_manager.get_camera()->toggle_handle_input();
                    SDL_SetWindowRelativeMouseMode(m_window.get(), scene_manager.get_camera()->get_handle_input()); break;
                case SDLK_F1:
                    forward_renderer = not forward_renderer;
                    break;
                case SDLK_F:
                    scene_manager.get_camera()->lookat(editor->get_selected_object_position());
                    break;
            }

        default: return SDL_APP_CONTINUE;
    }
}

void Game::Quit(SDL_AppResult result) const {
    SDL_GL_DestroyContext(gl_context);
}

SDL_AppResult Game::OnQuit() {
    return SDL_APP_SUCCESS;
}

void Game::render_scene() {
    float timestamp = (static_cast<float>(SDL_GetTicks()) / 1000);

    Quad q;

    Shader *frenderer_program = shaders.at(0); // forward renderer
    Shader *cubemap_shader = shaders.at(1);
    Shader *billboard_shader = shaders.at(2);
    Shader *quad_shader = shaders.at(3);
    Shader *lightpass_shader = shaders.at(4);
    Shader *geometrypass_shader = shaders.at(5);
    Shader *shadowdepth_shader = shaders.at(6);
    Shader *shadowmap_shader = shaders.at(7);

    UniformBufferObject *uniform = ubo.get();

    uniform->set_object(0, timestamp); // set time
    uniform->set_object(16, scene_manager.get_camera()->camera_position);
    uniform->set_object(32, scene_manager.get_camera()->view_matrix);
    uniform->set_object(96, scene_manager.get_camera()->project_matrix);

    std::vector<LightSource*> lights = renderer->collect_light_source_components(&scene_manager);
    for (int i = 0; i < lights.size(); i++) {
        try {
            LightType* lt = lights.at(i)->light_type.get();
            if (auto* d = dynamic_cast<DirectionalLightType*>(lt)) {
                if (!d->cast_shadow) continue;
            } else if (auto* p = dynamic_cast<PointLightType*>(lt)) {
                if (!p->cast_shadow) continue;
            }
            shadowmaps.at(i).first_pass(1280, 720, shadowdepth_shader, &scene_manager, lights.at(i)->get_position());

        } catch(std::out_of_range e) {
            std::cout << "Need to add another shadow map" << std::endl;
            Shadowmap s = Shadowmap(1024);
            shadowmaps.emplace_back(s);
        }

    }

    if (!forward_renderer) {
        //gbuffer->draw(geometrypass_shader, lightpass_shader, &scene_manager, &q);
        gbuffer->geometry_pass(geometrypass_shader, &scene_manager, renderer.get());
        gbuffer->light_pass(lightpass_shader, &scene_manager, q, shadowmaps, lights, renderer.get());
        gbuffer->blit();
    } else {
        frenderer_program->use();
        renderer->render_scene(&scene_manager, frenderer_program);
    }



    // //
    // quad_shader->use();
    //
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, shadowmap->depthMap);
    // quad_shader->setInt("screenTexture", 0);
    // q.draw();


    billboard_shader->use();
    renderer->render_scene(&scene_manager, billboard_shader);

    cubemap_shader->use();
    renderer->render_scene(&scene_manager, cubemap_shader);

    ImGuizmo::Enable(true);

}


SDL_AppResult Game::OnRender() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);



    glClearColor(0.00f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    render_scene();
    glCheckError__();

    // ---------- IMGUI ----------
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window.get());

    scene_manager.process_remove_queue();
    return SDL_APP_CONTINUE;
}

void Game::render_imgui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGuizmo::SetRect(
        vp->Pos.x,
        vp->Pos.y,
        vp->Size.x,
        vp->Size.y
    );

    ImGuizmo::SetOrthographic(false);

    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    editor->render(delta_time, fps);
    scene_manager.imgui();

    ImGui::Render();
}


void Game::update_trackframetime() {
    Uint64 now = SDL_GetPerformanceCounter();
    delta_time = static_cast<float>(now - _last) / static_cast<float>(_freq);
    _last = now;

    fps = (delta_time > 0.0) ? (1.0 / delta_time) : 0.0;

    static float acc = 0.0;
    static int frames = 0;
    acc += delta_time;
    frames++;
    if (acc >= 1.0) {
        frames = 0;
        acc = 0;
    }
}


SDL_AppResult Game::OnUpdate() {
    scene_manager.update(delta_time);
    render_imgui();

    return SDL_APP_CONTINUE;
}

