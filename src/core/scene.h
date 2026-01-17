//
// Created by Charlie on 12/01/2026.
//

#ifndef SDL3_FIRST_SCENE_H
#define SDL3_FIRST_SCENE_H
#include <iostream>
#include <memory>
#include <SDL3/SDL_events.h>

#include "camera.h"
#include "gameobject.h"
#include "../graphics/model.h"


class Scene {
private:
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
    virtual void render(Shader* shader) {
        for (int i = 0; i < game_objects.size(); i++) {
            game_objects[i].get()->render(shader);
        }
    };


};


#endif //SDL3_FIRST_SCENE_H