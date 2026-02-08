//
// Created by User on 07/02/2026.
//

#ifndef SDL3_FIRST_IMGUI_EDITOR_H
#define SDL3_FIRST_IMGUI_EDITOR_H
#include <imgui.h>
#include "imgui_stdlib.h"

#include "scenemanager.h"
#include "components/transform.h"

class Transform;

class Editor {
private:
    int current_index = 0;
    Component *current_component;

public:
    SceneManager *scene_manager;

    Editor(SceneManager *scene_manager) {
        this->scene_manager = scene_manager;
    }

    void sceneview() {
        ImGui::Begin("Scene Game Objects");

        if (ImGui::BeginPopupContextItem("scene_objects_context_menu")) {
            if (ImGui::MenuItem("Add Empty Game Object")) {
                scene_manager->get_current_scene()->game_objects.push_back(std::make_unique<GameObject>());
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::BeginMenu("Add Game Object")) {
                static std::vector<std::string> object_names = GameObjectRegistry::instance().get_names();
                for (int i = 0; i < object_names.size(); i++) {
                    if (ImGui::Button(object_names[i].c_str())) {
                        auto* obj = GameObjectRegistry::instance().get_game_object_constructor(i);
                        scene_manager->get_current_scene()->game_objects.push_back(std::unique_ptr<GameObject>(obj));

                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        
        if (ImGui::BeginListBox("##SceneObjectsListbox", ImVec2(-FLT_MIN, -FLT_MIN))) {
            if (!scene_manager->get_game_objects()->empty()) {
                for (int i = 0; i < scene_manager->get_game_objects()->size(); i++) {
                    std::string n = scene_manager->get_game_objects()->at(i)->get_instance_name();
                    n += (" " + std::to_string(i));
                    const bool is_selected = (current_index == i);

                    if (ImGui::Selectable(n.c_str(), is_selected)) {
                        current_index = i;
                    }
                    if (ImGui::BeginPopupContextItem()) {
                        current_index = i;

                        if (ImGui::Button("Remove")) {
                            current_index = 0;
                            scene_manager->remove_game_object(i);
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
        if (!scene_manager->get_game_objects()->empty()) {

            auto selected_game_object = scene_manager->get_game_objects()->at(current_index).get();

            std::string game_object_rename = selected_game_object->instance_name.data();
            if (ImGui::InputText("Game Object Name", &game_object_rename, ImGuiInputTextFlags_EnterReturnsTrue)) {
                selected_game_object->instance_name = game_object_rename;
            }
            ImGui::Spacing();

            for (int i = 0; i < selected_game_object->components.size(); i++) {

                auto component = selected_game_object->components.at(i).get();

                if (ImGui::CollapsingHeader(component->get_class_name())) {
                    component->properties_editor_imgui(scene_manager->get_current_scene());
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


    void perf_stats(float delta_time, float fps) {
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

    void render(float delta_time, float fps) {
        sceneview();
        perf_stats(delta_time, fps);
    }

    glm::vec3 get_selected_object_position() {
        auto game_object = scene_manager->get_game_objects()->at(current_index).get();
        auto* t = game_object->get_component<Transform>();

        if (t != NULL) {
            return t->position;
        } else {
            return glm::vec3(0);
        }

    }







};














#endif //SDL3_FIRST_IMGUI_EDITOR_H