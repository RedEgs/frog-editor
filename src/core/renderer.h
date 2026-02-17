//
// Created by User on 13/02/2026.
//
#ifndef SDL3_FIRST_RENDERER_H
#define SDL3_FIRST_RENDERER_H
#include <vector>

class Shader;
class Model;
class SceneManager;
class LightSource;
class Primitive;
class Mesh;
class GameObject;
struct LightType;

class Renderer {
private:
    std::vector<LightType*> lights;
    std::vector<Model*> geometry;

    bool in_pass = false;
public:


    void render_scene(SceneManager *scene_manager, Shader *shader) {
        in_pass = true;
    }

    std::vector<LightSource*> collect_light_source_components(SceneManager *scene_manager);
    std::vector<Primitive*> collect_primitive_components(SceneManager *scene_manager);
    std::vector<Mesh*> collect_mesh_components(SceneManager *scene_manager);

    void submit_light(LightType* light_type) {
        if (in_pass) {
            lights.emplace_back(light_type);
        }
    }

    void submit_model(Model* model) {
        if (in_pass) {
            geometry.emplace_back(model);
        }
    }



};


#endif //SDL3_FIRST_RENDERER_H