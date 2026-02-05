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
#include "core/scene.h"
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


    ubo = std::make_unique<UniformBufferObject>(160, 0);
    std::cout << "Made UBO" << std::endl;
    glCheckError__();

    Shader *shader_prog = new Shader(RESOURCES("forward-renderer/vertex.glsl"), RESOURCES("forward-renderer/fragment.glsl"), "forward-renderer");
    Shader *cubemap_shader = new Shader(RESOURCES("cubemap/cubemapvert.glsl"), RESOURCES("cubemap/cubemapfrag.glsl"), "skybox");
    Shader *billboard_shader = new Shader(RESOURCES("billboard/bvertex.glsl"), RESOURCES("billboard/bfragment.glsl"), "billboard");
    Shader *quad_shader = new Shader(RESOURCES("quad-2d/vertex.glsl"), RESOURCES("quad-2d/fragment.glsl"), "quad-2d");
    // Shader *lightpass_shader = new Shader(RESOURCES("lightpass/vertex.glsl"), RESOURCES("lightpass/fragment.glsl"), "lightpass");
    // Shader *geometrypass_shader = new Shader(RESOURCES("gpass/vertex.glsl"), RESOURCES("gpass/fragment.glsl"), "gpass");
    std::cout << "Compiled shaders" << std::endl;
    glCheckError__();

    fbo = std::make_unique<Framebuffer>(1280, 720);
    std::cout << "Made FB" << std::endl;
    glCheckError__();

    fbo.get()->new_texture();
    std::cout << "Made FB texture" << std::endl;
    glCheckError__();

    shaders.emplace_back(shader_prog);
    shaders.emplace_back(cubemap_shader);
    shaders.emplace_back(billboard_shader);
    shaders.emplace_back(quad_shader);
    // shaders.emplace_back(lightpass_shader);
    // shaders.emplace_back(geometrypass_shader);
    std::cout << "Cached shaders" << std::endl;

    int scene = scene_manager.add_scene(std::make_unique<Test_Scene>());
    scene_manager.set_scene(scene);
    std::cout << "Set Scene" << std::endl;
    //
    // geometrypass_shader->use();
    // geometrypass_shader->setInt("gPosition", 0);
    // geometrypass_shader->setInt("gNormal", 1);
    // geometrypass_shader->setInt("gAlbedoSpec", 2);
    //

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
                    render_quad = !render_quad;
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

    Shader *shader_program = shaders.at(0);
    Shader *cubemap_shader = shaders.at(1);
    Shader *billboard_shader = shaders.at(2);
    Shader *quad_shader = shaders.at(3);
    // Shader *lightpass_shader = shaders.at(2);
    // Shader *geometrypass_shader = shaders.at(3);

    UniformBufferObject *uniform = ubo.get();

    uniform->set_object(0, timestamp); // set time
    uniform->set_object(16, scene_manager.get_camera()->camera_position);
    uniform->set_object(32, scene_manager.get_camera()->view_matrix);
    uniform->set_object(96, scene_manager.get_camera()->project_matrix);
    //
    // cubemap_shader->use();
    // scene_manager.render(cubemap_shader);

    shader_program->use();
    shader_program->setFloat("material.shininess", 10.0f);
    scene_manager.render(shader_program);


    fbo->use();
    fbo->clear();

    shader_program->use();
    shader_program->setFloat("material.shininess", 10.0f);
    scene_manager.render(shader_program);


    Framebuffer::unbind();

    if (render_quad) {
        quad_shader->use();

        glActiveTexture(GL_TEXTURE0);
        fbo->use_texture(0);
        quad_shader->setInt("screenTexture", 0);
        q.draw();
    }



    // shader_program->setMat4("view", scene_manager.get_camera()->view_matrix);
    // shader_program->setMat4("proj", scene_manager.get_camera()->project_matrix);
    // shader_program->setVec3("view_position", scene_manager.get_camera()->camera_position);

    // billboard_shader->use();
    // scene_manager.render(billboard_shader);


    // lightpass_shader->dir_light_count = 0;
    // lightpass_shader->point_light_count = 0;
    shader_program->dir_light_count = 0;
    shader_program->point_light_count = 0;


    ImGuizmo::Enable(true);

}


SDL_AppResult Game::OnRender() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);


    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
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

void Game::imgui_sceneview() {
    ImGui::Begin("Scene Game Objects");

    if (ImGui::BeginPopupContextItem("scene_objects_context_menu")) {
        if (ImGui::MenuItem("Add Empty Game Object")) {
            scene_manager.get_current_scene()->game_objects.push_back(std::make_unique<GameObject>());
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::BeginMenu("Add Game Object")) {
            static std::vector<std::string> object_names = GameObjectRegistry::instance().get_names();
            for (int i = 0; i < object_names.size(); i++) {
                if (ImGui::Button(object_names[i].c_str())) {
                    auto* obj = GameObjectRegistry::instance().get_game_object_constructor(i);
                    scene_manager.get_current_scene()->game_objects.push_back(std::unique_ptr<GameObject>(obj));

                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }



    static int item_selected_index = 0;
    if (ImGui::BeginListBox("##SceneObjectsListbox", ImVec2(-FLT_MIN, -FLT_MIN))) {
        if (!scene_manager.get_game_objects()->empty()) {
            for (int i = 0; i < scene_manager.get_game_objects()->size(); i++) {
                std::string n = scene_manager.get_game_objects()->at(i)->get_instance_name();
                n += (" " + std::to_string(i));
                const bool is_selected = (item_selected_index == i);

                if (ImGui::Selectable(n.c_str(), is_selected)) {
                    item_selected_index = i;
                }
                if (ImGui::BeginPopupContextItem()) {
                    item_selected_index = i;

                    if (ImGui::Button("Remove")) {
                        item_selected_index = 0;
                        scene_manager.remove_game_object(i);
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
    }
    ImGui::EndListBox();
    // Check if the user right clicked in the empty space of the listbox
    if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()) && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsAnyItemHovered()) {
        ImGui::OpenPopup("scene_objects_context_menu");
    }















    ImGui::End();

    ImGui::Begin("Game Object Properties");
    if (!scene_manager.get_game_objects()->empty()) {

        auto selected_game_object = scene_manager.get_game_objects()->at(item_selected_index).get();

        std::string game_object_rename = selected_game_object->instance_name.data();
        if (ImGui::InputText("Game Object Name", &game_object_rename, ImGuiInputTextFlags_EnterReturnsTrue)) {
            selected_game_object->instance_name = game_object_rename;
        }
        ImGui::Spacing();

        for (int i = 0; i < selected_game_object->components.size(); i++) {

            auto component = selected_game_object->components.at(i).get();

            if (ImGui::CollapsingHeader(component->get_class_name())) {
                component->properties_editor_imgui(scene_manager.get_current_scene());
            }
        }
        ImGui::Separator();
        if (ImGui::Button("Add Component (+)", ImVec2(-FLT_MIN, ImGui::CalcTextSize("Add Component (+)").y *2))) {
            ImGui::OpenPopup("add_component_list");
        }

        if (ImGui::BeginPopup("add_component_list")) {
            ImGui::Text("List:");
            static std::vector<std::string> component_names = ComponentRegistry::instance().get_names();
            for (int i = 0; i < component_names.size(); i++) {
                if (ImGui::Button(component_names[i].c_str())) {
                    auto* obj = ComponentRegistry::instance().get_component_constructor(i);
                    selected_game_object->add_component_instance_runtime(obj);

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }

    }

    ImGui::End();
}

void Game::imgui_perfstats() {
    bool p_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Debug Overlay", &p_open, window_flags);

    static float _time = 0.0; _time += delta_time;
    static std::string fps_text = "FPS: " + std::to_string(fps);
    if (_time >= .5) {
        fps_text = "FPS: " + std::to_string(fps);
        _time = 0.0;
    }

    ImGui::Text(fps_text.c_str());


    ImGui::End();
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

    imgui_sceneview();
    imgui_perfstats();

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

