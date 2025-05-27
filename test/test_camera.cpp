#include <gtest/gtest.h>
#include "camera.hpp"

TEST(CameraTest, ray_from_mouse_pos_is_correct) {
    int32_t window_width = 1028;
    int32_t window_height = 768;
    int32_t window_width_high_dpi = window_width * 2;
    int32_t window_height_high_dpi = window_height * 2;
    Camera camera{};
    float scale_factor = 2.0f;
    camera.init(nullptr, static_cast<float>(window_width_high_dpi), static_cast<float>(window_height_high_dpi), scale_factor);

    camera.x_mouse = window_width/2;
    camera.y_mouse = window_height/2;

    auto mouse_ray = camera.get_ray_from_mouse();
    auto camera_direction = camera.get_direction();
    ASSERT_NEAR(mouse_ray.x, camera_direction.x, 1e-6);
    ASSERT_NEAR(mouse_ray.y, camera_direction.y, 1e-6);
    ASSERT_NEAR(mouse_ray.z, camera_direction.z, 1e-6);
}