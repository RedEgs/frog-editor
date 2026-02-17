//
// Created by Charlie on 12/01/2026.
//
#pragma once

#ifndef SDL3_FIRST_SCENEMANAGER_H
#define SDL3_FIRST_SCENEMANAGER_H
#include <memory>

#include "gameobject.h"


class Renderer;
class Shader;
class GameObject;

class Scene {
private:
    std::vector<int> remove_queue;
public:
    static SDL_Window* window;

    virtual ~Scene() = default;

    Camera main_camera = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -2));
    std::vector<std::unique_ptr<GameObject>> game_objects;

    // virtual void on_enter();
    // virtual void on_exit();

    virtual void events(SDL_Event *event) {};
    virtual void imgui() {};

    virtual void update(float delta_time) {
        main_camera.update(delta_time);
        for (int i = 0; i < game_objects.size(); i++) {
            game_objects[i].get()->update(delta_time);
        }
    };
    virtual void render(Shader* shader, Renderer *renderer) {
        for (int i = 0; i < game_objects.size(); i++) {
            Camera c = main_camera;
            game_objects[i].get()->render(shader, c, renderer);
        }
    };

    void remove_game_object(int index) {
        remove_queue.push_back(index);
        //
    }

    void process_remove_queue() {
        if (remove_queue.size() > 0) {
            for (int i = 0; i < remove_queue.size(); i++) {
                game_objects.erase(game_objects.begin() + remove_queue.at(i));
            }
        }
        remove_queue.clear();
    }
};

class SceneManager {
private:
    unsigned int current_scene_index;
    std::vector<std::unique_ptr<Scene>> scenes;

public:
    SceneManager(): current_scene_index(0) {};

    void events(SDL_Event *event) const {
        if (!scenes.empty()) {
            scenes[current_scene_index]->events(event);
        } else {
            std::cerr << "No scenes in the scene manager" << std::endl;
        }
    };

    void update(float delta_time) const {
        if (!scenes.empty()) {
            scenes[current_scene_index]->update(delta_time);
        } else {
            std::cerr << "No scenes in the scene manager" << std::endl;
        }
    };

    void imgui() const {
        if (!scenes.empty()) {
            scenes[current_scene_index]->imgui();
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
        if (!scenes.empty()) {
            return &scenes[current_scene_index]->main_camera;
        }
        std::cerr << "No scenes in the scene manager" << std::endl;
        return nullptr;
    };

    std::vector<std::unique_ptr<GameObject>> *get_game_objects() const  {
        if (!scenes.empty()) {
            return &scenes[current_scene_index]->game_objects;
        }
        std::cerr << "No scenes in the scene manager" << std::endl;
    };

    Scene *get_current_scene() const {
        if (!scenes.empty()) {
            return scenes[current_scene_index].get();
        }
        std::cerr << "No scenes in the scene manager" << std::endl;
    };

    void remove_game_object(int index) {
        if (scenes.empty()) return;

        Scene * s = scenes[current_scene_index].get();

        if (index > s->game_objects.size()) return;
        s->remove_game_object(index);
    }

    void process_remove_queue() {
        if (scenes.empty()) return;
        Scene * s = scenes[current_scene_index].get();
        s->process_remove_queue();
    }
};


#endif //SDL3_FIRST_SCENEMANAGER_H