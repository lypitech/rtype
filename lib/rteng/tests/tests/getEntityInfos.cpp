#include <gtest/gtest.h>

#include "rteng.hpp"

struct Position
{
    int x;
    int y;
};

struct Type
{
    int type;
};

TEST(getEntityInfos,
     EngineUtilities)
{
    rteng::GameEngine engine = rteng::GameEngine(rteng::ComponentsList<Position, Type>{});
    // std::vector<uint8_t> content{0, 0, 0, 0, 0, 0, 0, 0, 1};
    //
    // const rtecs::types::EntityID id = engine.registerEntity<Position, Type>(
    //     nullptr,
    //     {0, 0},
    //     {1});
    // const rteng::EntityInfos infos = engine.getEntityInfos(rteng::ComponentsList<Position, Type>{}, id);
    //
    // EXPECT_EQ(infos.first, engine.getEcs()->getEntityMask(id).serialize());
    // EXPECT_EQ(infos.second, content);
}
