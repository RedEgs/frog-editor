//
// Created by Charlie on 07/01/2026.
//

#ifndef SDL3_FIRST_CAMERA_H
#define SDL3_FIRST_CAMERA_H
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>


class Camera {
private:
    glm::vec3 camera_up = glm::vec3(0.0f, 0.1f, 0.0f);

    float yaw = -90.0f;
    float pitch = 0.0f;

    float pitch_limit = 89.0f;

    float delta_x = 400.0f;
    float delta_y = 300.0f;

    bool handle_input = true;

    void handle_mouse_look(float delta_time) {
        float dx, dy;
        SDL_GetRelativeMouseState(&dx, &dy);

        dx *= sensitivity * delta_time;
        dy *= sensitivity * delta_time;

        yaw   += dx;
        pitch -= dy;

    }
    void handle_keyboard_movement(float delta_time) {
        const bool *key_states = SDL_GetKeyboardState(NULL);
        float current_camera_speed = camera_speed * delta_time;

        if (key_states[SDL_SCANCODE_W]) {
            camera_position += current_camera_speed*camera_front;  /* pressed what would be "W" on a US QWERTY keyboard. Move forward! */
        }
        if (key_states[SDL_SCANCODE_S]) {
            camera_position -= current_camera_speed*camera_front;  /* pressed what would be "W" on a US QWERTY keyboard. Move forward! */
        }
        if (key_states[SDL_SCANCODE_A]) {
            camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * current_camera_speed;  /* pressed what would be "W" on a US QWERTY keyboard. Move forward! */
        }
        if (key_states[SDL_SCANCODE_D]) {
            camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * current_camera_speed;  /* pressed what would be "W" on a US QWERTY keyboard. Move forward! */
        }
    }

    void update_view_matrix() {
        pitch = glm::clamp(pitch, -pitch_limit, pitch_limit);

        glm::vec3 dir;
        dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        dir.y = sin(glm::radians(pitch));
        dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_front = glm::normalize(dir);

        view_matrix = glm::lookAt(
        camera_position,
        camera_position+camera_front,
        camera_up);
    }

public:
    Camera(glm::vec3 position, glm::vec3 look_at) {
        camera_position = position;
        camera_front = look_at;
        project_matrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        update_view_matrix();
    }

    Camera(glm::vec3 position, glm::vec3 look_at, float _yaw, float _pitch) {
        camera_position = position;
        camera_front = look_at;
        yaw = _yaw;
        pitch = _pitch;
        project_matrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        update_view_matrix();

    }

    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;

    // Camera(float look_at_x, float look_at_y, float look_at_z, float pos_x, float pos_y, float pos_z);
    void update(float delta_time) {
        // Updates the view matrix to conform to the look at position
        if (handle_input) {
            handle_mouse_look(delta_time);
            handle_keyboard_movement(delta_time);
        }
        update_view_matrix();

    }

    float sensitivity = 8.0f;
    float camera_speed = 1.5f;
    glm::vec3 camera_position;
    glm::vec3 camera_front;
    glm::mat4 view_matrix;
    glm::vec3 camera_direction;
    glm::mat4 project_matrix;

    void toggle_handle_input() {
        handle_input = !handle_input;
    }

    bool get_handle_input() const {
        return this->handle_input;
    };

    glm::vec3 get_right_vector() {
        return glm::vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
    }
    glm::vec3 get_up_vector() {
        return glm::vec3(view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
    }
};


#endif //SDL3_FIRST_CAMERA_H