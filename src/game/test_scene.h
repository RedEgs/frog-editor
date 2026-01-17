//
// Created by Charlie on 12/01/2026.
//

#ifndef SDL3_FIRST_TEST_SCENE_H
#define SDL3_FIRST_TEST_SCENE_H
#include "../core/scene.h"
#include "../core/components/lightsource.h"
#include "../core/objects/lightsourceobject.h"
#include "../core/objects/modelobject.h"


class Test_Scene : public Scene {
public:
    explicit Test_Scene() {
        std::shared_ptr<Model> m(new Model("models/sphere.obj"));

        game_objects.push_back(std::make_unique<ModelObject>(m, glm::vec3(0.0), glm::vec3(1.0)));
        game_objects.push_back(std::make_unique<LightSourceObject>(DirectionalLightType::MakeSun()));
        game_objects.push_back(std::make_unique<LightSourceObject>(PointLightType::Regular()));
    };
};


#endif //SDL3_FIRST_TEST_SCENE_H