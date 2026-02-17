//
// Created by User on 13/02/2026.
//

#include "renderer.h"
#include "scenemanager.h"
#include "components/lightsource.h"
#include "components/primitive.h"


std::vector<LightSource*> collect_light_source_components(SceneManager *scene_manager) {
    std::vector<LightSource*> lights;

    for (auto& go_ptr : scene_manager->get_current_scene()->game_objects) {
        GameObject* go = go_ptr.get(); // get pointer to existing GameObject

        for (auto& comp : go->components) {
            if (auto* light = dynamic_cast<LightSource*>(comp.get())) {
                lights.push_back(light);
            }
        }
    }

    return lights;
}

std::vector<Primitive*> collect_primitive_components(SceneManager *scene_manager) {
    std::vector<Primitive*> primitives;

    for (auto& go_ptr : scene_manager->get_current_scene()->game_objects) {
        GameObject* go = go_ptr.get(); // get pointer to existing GameObject

        for (auto& comp : go->components) {
            if (auto* primitive = dynamic_cast<Primitive*>(comp.get())) {
                primitives.push_back(primitive);
            }
        }
    }

    return primitives;
}

std::vector<Mesh*> collect_mesh_components(SceneManager *scene_manager) {
    std::vector<Mesh*> meshs;

    for (auto& go_ptr : scene_manager->get_current_scene()->game_objects) {
        GameObject* go = go_ptr.get(); // get pointer to existing GameObject

        for (auto& comp : go->components) {
            if (auto* mesh = dynamic_cast<Mesh*>(comp.get())) {
                meshs.push_back(mesh);
            }
        }
    }

    return meshs;
}
