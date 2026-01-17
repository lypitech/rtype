#include <gtest/gtest.h>

#include "rteng.hpp"

struct Position
{
    int x;
    int y;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & x & y;
    }
};

struct Type
{
    int type;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & type;
    }
};

struct Name
{
    std::string name;

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & name;
    }
};

TEST(EngineUtilities,
     getEntityInfosEntityWithAlltypes)
{
    rteng::GameEngine engine = rteng::GameEngine(rteng::ComponentsList<Position, Type>{});
    std::vector<uint8_t> content{0, 0, 0, 10, 0, 0, 0, 15, 0, 0, 0, 1};
    std::vector<uint64_t> mask{1, 2};

    const rtecs::types::EntityID id = engine.registerEntity<Position, Type>(nullptr, {10, 15}, {1});
    const rteng::EntityInfos infos =
        engine.getEntityInfos(rteng::ComponentsList<Position, Type>{}, id);

    EXPECT_EQ(infos.first, mask);
    EXPECT_EQ(infos.second, content);
}

TEST(EngineUtilities,
     getEntityInfosEntityWithoutSometypes)
{
    constexpr rteng::ComponentsList<Position, Type, Name> all{};
    rteng::GameEngine engine(all);
    std::vector<uint8_t> content{0, 0, 0, 10, 0, 0, 0, 15, 0, 0, 0, 1};
    std::vector<uint64_t> mask{1, 2};

    const rtecs::types::EntityID id = engine.registerEntity<Position, Type>(nullptr, {10, 15}, {1});
    const rteng::EntityInfos infos = engine.getEntityInfos(all, id);

    EXPECT_EQ(infos.first, mask);
    EXPECT_EQ(infos.second, content);
}

TEST(EngineUtilities,
     getEntityInfosEntityWithComplexType)
{
    constexpr rteng::ComponentsList<Position, Type, Name> all{};
    rteng::GameEngine engine(all);
    std::vector<uint8_t> content{0, 0, 0, 1, 0, 5, 'H', 'e', 'l', 'l', 'o'};
    std::vector<uint64_t> mask{2, 3};

    const rtecs::types::EntityID id = engine.registerEntity<Type, Name>(nullptr, {1}, {"Hello"});
    const rteng::EntityInfos infos = engine.getEntityInfos(all, id);

    EXPECT_EQ(infos.first, mask);
    EXPECT_EQ(infos.second, content);
}
