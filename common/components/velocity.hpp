#pragma once

namespace components {

struct Velocity {
    float vx = 0.0f;
    float vy = 0.0f;

    template <typename Archive>
    void serialize(Archive& ar) {
        ar & vx & vy;
    }
};
}
