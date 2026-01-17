//
// Created by Charlie on 12/01/2026.
//

#ifndef SDL3_FIRST_SCENEMANAGER_H
#define SDL3_FIRST_SCENEMANAGER_H
#include <memory>

#include "scene.h"


class SceneManager {
public:
    SceneManager(): current_scene_index(0) {};

    void events(SDL_Event *event) const {
        if (scenes.size() > 0) {
            scenes[current_scene_index]->events(event);
        } else {
            std::cerr << "No scenes in the scene manager" << std::endl;
        }
    };

    void update(float delta_time) const {
        if (scenes.size() > 0) {
            scenes[current_scene_index]->update(delta_time);
        } else {
            std::cerr << "No scenes in the scene manager" << std::endl;
        }
    };

    void imgui() const {
        if (scenes.size() > 0) {
            scenes[current_scene_index]->imgui();
        } else {
            std::cerr << "No scenes in the scene manager" << std::endl;
        }
    };

    void render(Shader* shader) const {
        if (scenes.size() > 0) {
            scenes[current_scene_index]->render(shader);
        } else {
            std::cerr << "No scenes in the scene manager" << std::endl;
        }
    };

    unsigned int add_scene(std::unique_ptr<Scene> scene) {
        std::cout << "Added scene to manager";
        scenes.push_back(std::move(scene));
        return scenes.size() - 1;
    };

    void set_scene(unsigned int scene_index) {
        current_scene_index = scene_index;
    };

    Camera* get_camera() const {
        if (scenes.size() > 0) {
            return &scenes[current_scene_index]->main_camera;
        }
        std::cerr << "No scenes in the scene manager" << std::endl;
        return nullptr;
    };

    std::vector<std::unique_ptr<GameObject>> *get_game_objects() const  {
        if (scenes.size() > 0) {
            return &scenes[current_scene_index]->game_objects;
        }
        std::cerr << "No scenes in the scene manager" << std::endl;
    };

    Scene *get_current_scene() const {
        if (scenes.size() > 0) {
            return scenes[current_scene_index].get();
        }
        std::cerr << "No scenes in the scene manager" << std::endl;
    };

private:
    unsigned int current_scene_index;
    std::vector<std::unique_ptr<Scene>> scenes;
};


#endif //SDL3_FIRST_SCENEMANAGER_H