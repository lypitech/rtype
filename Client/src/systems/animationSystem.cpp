#include "animationSystem.hpp"

#include <raylib.h>

#include "components/animations.hpp"

namespace systems {

AnimationSystem::AnimationSystem()
    : rtecs::systems::ASystem("AnimationSystem")
{
}

void AnimationSystem::apply(rtecs::ECS& ecs)
{
    float dt = GetFrameTime();

    ecs.group<components::Animation>().apply(
        [dt](const rtecs::types::EntityID&, components::Animation& anim) {
            anim.elapsed_time += dt;
            if (anim.elapsed_time >= anim.frame_time) {
                anim.elapsed_time = 0;
                anim.current_frame = (anim.current_frame + 1) % anim.frame_count;
            }
        });
}
}  // namespace systems
