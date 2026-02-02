cmake_minimum_required(VERSION 4.0)
project(sdl3_first)

set(CMAKE_CXX_STANDARD 20)

add_executable(sdl3_first src/main.cpp
        src/Game.cpp
        src/Game.h
        include/glad/src/glad.c
        include/glad/include/glad/glad.h
        include/glad/include/KHR/khrplatform.h
        src/graphics/shader.h
        src/graphics/texture.h
        src/core/camera.h
        src/graphics/vertex.h
        src/graphics/mesh.h
        src/graphics/model.h
        src/core/scene.h
        src/core/scenemanager.h
        src/game/test_scene.h
        src/core/GameObject.h
        src/core/component.h
        src/core/components/transform.h
        src/core/components/modelrenderer.h
        src/core/components/modelrenderer.h
        src/core/objects/modelobject.h
        src/core/components/lightsource.h
        src/core/objects/lightsourceobject.h
        src/core/components/primitive.h
        src/core/objects/primitiveobject.h
        src/core/components/material.h
        src/graphics/cubemap.h
        src/core/components/sky.h
        src/utility/files.h
)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
find_package(SDL3_ttf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf)
find_package(SDL3_image CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>)

find_package(FreeGLUT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>)
find_package(OpenGL)
target_link_libraries(OpenGL::GL)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imguizmo::imguizmo)
find_package(miniz CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE miniz::miniz)