#pragma once

namespace components {

struct Collision {
    float width = 0.0f;
    float height = 0.0f;
    bool is_trigger = false; // If true, we only detect collision but do not resolve it

    template <typename Archive>
    void serialize(Archive& ar) {
        ar & width & height & is_trigger;
    }
};
}
