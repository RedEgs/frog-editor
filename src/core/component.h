//
// Created by Charlie on 13/01/2026.
//

#ifndef SDL3_FIRST_COMPONENT_H
#define SDL3_FIRST_COMPONENT_H
#include "gameobject.h"


class Scene;
class Shader;
class GameObject;

class Component {
public:
    GameObject *owner = nullptr;
    virtual ~Component() = default;

    virtual void on_start() {}
    virtual void on_update(float dt) {}
    virtual void on_render(Shader *shader) {}
    virtual void properties_editor_imgui(Scene *scene) {} // Put ImGui code which is used in the property editor to modify components
    virtual const char* get_class_name() {return "Component";}
};

class ComponentRegistry {
public:
    using Func = std::function<Component*()>;

    static ComponentRegistry& instance() {
        static ComponentRegistry reg;
        return reg;
    }

    void registerClass(const std::string& name, Func func) {
        entries.push_back({name, func});
    }

    // void callAll() {
    //     for (auto& e : entries) {
    //         std::cout << "Calling " << e.name << "\n";
    //         e.func();
    //     }
    // }

    std::vector<std::string> get_names() {
        std::vector<std::string> names;
        for (auto& e : entries) {
            names.push_back(e.name.c_str());
        }
        return names;
    }

    Component* get_component_constructor(int index) {
        return entries.at(index).func();
    }

private:
    struct Entry {
        std::string name;
        Func func;
    };
    std::vector<Entry> entries;
};

struct AutoRegisterComponent {
    AutoRegisterComponent(const std::string& name, ComponentRegistry::Func func) {
        ComponentRegistry::instance().registerClass(name, func);
    }
};

#define REGISTER_COMPONENT(CLASS) \
    namespace { \
    AutoRegisterComponent _autoReg_##CLASS(#CLASS, []() -> Component* { return new CLASS(); }); \
    }

#endif //SDL3_FIRST_COMPONENT_H