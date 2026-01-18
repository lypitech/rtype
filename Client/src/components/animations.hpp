#pragma once

namespace components {
/**
 * @brief specifies everything needed to animate a sprite
 */
struct Animation
{
    int frame_count;
    int current_frame;
    float frame_time;
    float elapsed_time;
    int frame_width;
    int frame_height;
    bool loop;
};
}  // namespace components
