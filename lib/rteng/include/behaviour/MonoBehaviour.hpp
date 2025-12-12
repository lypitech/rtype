#pragma once

namespace rteng::behaviour {

class MonoBehaviour
{
public:
    virtual ~MonoBehaviour() = default;

    // Called once when the behaviour is first enabled
    virtual void Start() = 0;

    // Called every frame with delta time in seconds
    virtual void Update(float /*dt*/) = 0;

    // Called when the behaviour is destroyed / removed
    virtual void OnDestroy() = 0;
};

}  // namespace rteng::behaviour
