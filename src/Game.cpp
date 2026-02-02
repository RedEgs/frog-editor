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
#include "graphics/cubemap.h"

#include "graphics/shader.h"
#include "graphics/texture.h"




Game::Game(int argc, char ** argv) : m_window(nullptr, &SDL_DestroyWindow), gl_context(nullptr) {
}

SDL_AppResult Game::Init() {
    setup_window();


    scene_manager = SceneManager();

    Shader *shader_prog = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl", "main");
    Shader *cubemap_shader = new Shader("shaders/cubemapvert.glsl", "shaders/cubemapfrag.glsl", "skybox");

    shaders.emplace_back(shader_prog);
    shaders.emplace_back(cubemap_shader);

    int scene = scene_manager.add_scene(std::make_unique<Test_Scene>());
    scene_manager.set_scene(scene);

    // cubemap_ = std::make_unique<Cubemap>(
    //     "skybox/right.jpg",
    //     "skybox/left.jpg",
    //     "skybox/top.jpg",
    //     "skybox/bottom.jpg",
    //     "skybox/front.jpg",
    //     "skybox/back.jpg"
    // );



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
                    SDL_SetWindowRelativeMouseMode(m_window.get(), scene_manager.get_camera()->get_handle_input());
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

    Shader *shader_program = shaders.at(0);
    Shader *cubemap_shader = shaders.at(1);

    //cubemap_->draw(glm::mat4(glm::mat3(scene_manager.get_camera()->view_matrix)), scene_manager.get_camera()->project_matrix, cubemap_shader);
    cubemap_shader->use();
    scene_manager.render(cubemap_shader);

    shader_program->use();

    shader_program->setFloat("time", timestamp);
    shader_program->setMat4("view", scene_manager.get_camera()->view_matrix);
    shader_program->setMat4("proj", scene_manager.get_camera()->project_matrix);
    shader_program->setVec3("view_position", scene_manager.get_camera()->camera_position);
    shader_program->setFloat("material.shininess", 10.0f);

    scene_manager.render(shader_program);
    shader_program->dir_light_count = 0;
    shader_program->point_light_count = 0;


    ImGuizmo::Enable(true);

}


SDL_AppResult Game::OnRender() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_scene();
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

