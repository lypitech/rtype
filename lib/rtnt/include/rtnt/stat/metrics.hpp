#pragma once

#include <array>
#include <atomic>
#include <vector>

namespace rtnt::stat {

/**
 * @struct  ChannelMetrics
 * @brief   Atomic counters for a specific channel (0-255).
 */
struct ChannelMetrics
{
    std::atomic<uint64_t> packetsSent{0};
    std::atomic<uint64_t> packetsReceived{0};
    std::atomic<uint64_t> bytesSent{0};
    std::atomic<uint64_t> bytesReceived{0};
};

/**
 * @struct  NetworkMetrics
 * @brief   Raw atomic counters for Peers.
 */
struct NetworkMetrics
{
    std::atomic<uint64_t> totalBytesSent = 0;
    std::atomic<uint64_t> totalBytesReceived = 0;
    std::atomic<uint64_t> totalPacketsSent = 0;
    std::atomic<uint64_t> totalPacketsReceived = 0;
};

/**
 * @struct  SessionMetrics
 * @brief   Raw atomic counters for Session.
 */
struct SessionMetrics
{
    std::atomic<uint32_t> rtt = 0;              ///< Last measured RTT (ping) in milliseconds
    std::atomic<uint32_t> maxRtt = 0;           ///< Max RTT (ping) observed
    std::atomic<uint64_t> retransmitCount = 0;  ///< Number of packets re-sent
    std::atomic<uint64_t> duplicateCount = 0;   ///< Number of duplicate packets received
    std::atomic<uint64_t> packetLossCount = 0;  ///< Packets confirmed lost (approx.)

    std::array<ChannelMetrics, 256> channels;
};

/**
 * @struct  PacketMetrics
 * @brief   Raw atomic counters for Dispatcher.
 */
struct PacketMetrics
{
    std::atomic<uint64_t> count{0};
    std::atomic<uint64_t> bytes{0};
};

struct ChannelSnapshot
{
    uint8_t channelId;
    uint64_t packetsSent;
    uint64_t packetsReceived;
    uint64_t bytesSent;
    uint64_t bytesReceived;
};

struct PacketSnapshot
{
    uint16_t packetId;
    uint64_t count;
    uint64_t bytes;
};

struct SessionSnapshot
{
    uint32_t sessionId;
    uint32_t rtt;
    uint64_t retransmitCount;
    uint64_t duplicateCount;

    std::vector<ChannelSnapshot> activeChannels;
};

struct SystemSnapshot
{
    uint64_t timestamp;  // ms since start

    // Global
    uint64_t totalBytesSent;
    uint64_t totalBytesReceived;

    // Breakdowns
    std::vector<SessionSnapshot> sessions;
    std::vector<PacketSnapshot> packetUsage;
};

}  // namespace rtnt::stat
