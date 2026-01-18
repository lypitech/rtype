#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "enums/entity_types.hpp"
#include "gui/texture.hpp"

namespace gui {

enum class UIAsset
{
    kMenuBackground,
    kPauseButton,
    kHealthBar,
    kScoreFont
};

struct SpriteConfig
{
    int frameCount;
    int frameWidth;
    int frameHeight;
    float frameTime;
};

struct AnimationConfig
{
    int frameCount;
    float frameTime;
    int frameWidth;
    int frameHeight;
    bool loop;
};

static const std::unordered_map<entity::Type, AnimationConfig> entityTypeToAnimation = {
    {entity::Type::kPlayer,
     {5,
      0.15f,
      33,
      15,
      true}},  // {Number of frames, Time per frame, Frame width, Frame height, Loop}
    {entity::Type::kEnemy, {8, 0.15f, 32, 32, true}},
    {entity::Type::kBullet, {3, 0.15f, 18, 14, false}},
};

inline AnimationConfig typeToAnimation(const entity::Type& type)
{
    if (!entityTypeToAnimation.contains(type)) {
        return {1, 0.0f, 0, 0, false};
    }
    return entityTypeToAnimation.at(type);
}

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager() = default;

    void init();

    [[nodiscard]] const gui::Texture& getTexture(entity::Type id,
                                                 short& players) const;

    [[nodiscard]] const Texture2D& getBackground() const;

    [[nodiscard]] const gui::Texture& getUITexture(gui::UIAsset id) const;

    [[nodiscard]] const gui::SpriteConfig& getSpriteConfig(entity::Type id) const;

private:
    std::unique_ptr<gui::Texture> _background;
    std::vector<gui::Texture> _textures;
    std::vector<gui::Texture> _players;
    std::map<entity::Type, gui::SpriteConfig> _spriteConfigs;
    std::map<gui::UIAsset, std::unique_ptr<gui::Texture>> _uiTextures;
};

}  // namespace gui
