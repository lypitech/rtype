#include <gtest/gtest.h>

#include "network_fixture.hpp"

namespace {

struct Example
{
    static constexpr rtnt::core::packet::Id kId = 1801;
    static constexpr rtnt::core::packet::Name kName = "EXAMPLE";

    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;

    float f;
    double d;

    std::string s;

    std::vector<uint8_t> v_u8;
    std::vector<uint16_t> v_u16;
    std::vector<uint32_t> v_u32;
    std::vector<uint64_t> v_u64;

    std::vector<int8_t> v_s8;
    std::vector<int16_t> v_s16;
    std::vector<int32_t> v_s32;
    std::vector<int64_t> v_s64;

    std::vector<float> v_f;
    std::vector<double> v_d;

    std::vector<std::string> v_s;

    bool operator==(const Example&) const = default;

    void dump()
    {
        LOG_TRACE_R3(
            "u8: {}\n"
            "u16: {}\n"
            "u32: {}\n"
            "u64: {}\n"
            "s8: {}\n"
            "s16: {}\n"
            "s32: {}\n"
            "s64: {}\n"
            "f: {}\n"
            "d: {}\n"
            "s: {}\n"
            "v_u8: {} (size: {})\n"
            "v_u16: {} (size: {})\n"
            "v_u32: {} (size: {})\n"
            "v_u64: {} (size: {})\n"
            "v_s8: {} (size: {})\n"
            "v_s16: {} (size: {})\n"
            "v_s32: {} (size: {})\n"
            "v_s64: {} (size: {})\n"
            "v_f: {} (size: {})\n"
            "v_d: {} (size: {})\n"
            "v_s: {} (size: {})",
            u8,
            u16,
            u32,
            u64,
            s8,
            s16,
            s32,
            s64,
            f,
            d,
            s,
            v_u8,
            v_u8.size(),
            v_u16,
            v_u16.size(),
            v_u32,
            v_u32.size(),
            v_u64,
            v_u64.size(),
            v_s8,
            v_s8.size(),
            v_s16,
            v_s16.size(),
            v_s32,
            v_s32.size(),
            v_s64,
            v_s64.size(),
            v_f,
            v_f.size(),
            v_d,
            v_d.size(),
            v_s,
            v_s.size());
    }

    template <typename Archive>
    void serialize(Archive& ar)
    {
        ar & u8 & u16 & u32 & u64 & s8 & s16 & s32 & s64 & f & d & s & v_u8 & v_u16 & v_u32 &
            v_u64 & v_s8 & v_s16 & v_s32 & v_s64 & v_f & v_d & v_s;
    }
};

}  // namespace

TEST_F(NetworkTest,
       complex_packet)
{
    Example sent{
        .u8 = 0xFF,
        .u16 = 0xAABB,
        .u32 = 0xAABBCCDD,
        .u64 = 0x1122334455667788,

        .s8 = -120,
        .s16 = -30000,
        .s32 = -2000000000,
        .s64 = -9000000000000000000,

        .f = 3.14159f,
        .d = 1.23456789012345,

        .s = "aymeric caca boudin 🥇",

        .v_u8 = {1, 2, 255},
        .v_u16 = {100, 0xAABB, 65535},
        .v_u32 = {100000, 0xAABBCCDD},
        .v_u64 = {0x1122334455667788, 999999},

        .v_s8 = {-1, -127, 50},
        .v_s16 = {-32000, 32000},
        .v_s32 = {-2000000, 2000000},
        .v_s64 = {-123456789, 123456789},

        .v_f = {1.1f, -2.5f, 100.0f},
        .v_d = {0.00001, -9999.9999, 3.1415926535},

        .v_s = {"ermmmm", "what", "ermmm what the sigma 24-70 f2.8 art ii with a sony a7riii"}};

    std::optional<Example> received;

    server->getPacketDispatcher().bind<Example>([&](const auto&, const Example& pkt) {
        received = pkt;
        received->dump();
    });

    client->connect("127.0.0.1", 4242);

    ASSERT_TRUE(waitFor([&]() { return client->isConnected(); })) << "Client failed to connect.";

    client->send(sent);

    ASSERT_TRUE(waitFor([&]() { return received.has_value(); }))
        << "Server has received no EXAMPLE packet.";

    EXPECT_EQ(*received, sent);

    // EXPECT_EQ(receivedData1, dataToTransmit1);
    // EXPECT_EQ(receivedData2, dataToTransmit2);
}
