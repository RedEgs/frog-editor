//
// Created by Charlie on 17/01/2026.
//

#ifndef SDL3_FIRST_MATERIAL_H
#define SDL3_FIRST_MATERIAL_H

#include "../component.h"

class MaterialComponent: public Component {
private:
public:

    MaterialComponent() {}

    static const char* get_static_class_name() { return "Model Renderer"; };
    const char* get_class_name() override {return "ModelRendererComponent";}
};
REGISTER_COMPONENT(MaterialComponent);

#endif //SDL3_FIRST_MATERIAL_H