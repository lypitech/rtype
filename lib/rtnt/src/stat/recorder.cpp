#include "rtnt/stat/recorder.hpp"

#include <mutex>

namespace rtnt::stat {

void Recorder::start(std::chrono::milliseconds interval)
{
    if (_isRunning) {
        return;
    }

    _isRunning = true;
    _workerThread = std::thread{&Recorder::_workerLoop, this, interval};
}

void Recorder::stop()
{
    if (!_isRunning) {
        return;
    }

    _isRunning = false;
    _cv.notify_all();
    if (_workerThread.joinable()) {
        _workerThread.join();
    }
}

void Recorder::exportToCsv(const std::string& filename) const
{
    std::lock_guard lock(_historyMutex);

    // global

    std::ofstream file(filename + ".csv");

    if (file.is_open()) {
        file << "Timestamp,TotalBytesSent,TotalBytesReceived,AvgRTT,TotalRetries\n";

        for (const auto& entry : _history) {
            uint64_t totalRtt = 0;
            uint64_t totalRetries = 0;

            if (!entry.sessions.empty()) {
                for (const auto& s : entry.sessions) {
                    totalRtt += s.rtt;
                    totalRetries += s.retransmitCount;
                }
                totalRtt /= entry.sessions.size();
            }

            file << entry.timestamp << "," << entry.totalBytesSent << ","
                 << entry.totalBytesReceived << "," << totalRtt << "," << totalRetries << "\n";
        }

        LOG_INFO("Global stats exported to {}", filename);
    }

    // channels

    std::string chFilename = filename.substr(0, filename.find_last_of('.')) + "_channels.csv";
    std::ofstream chFile(chFilename);

    if (chFile.is_open()) {
        chFile << "Timestamp,SessionId,ChannelId,PacketsSent,PacketsReceived,BytesSent,"
                  "BytesReceived\n";

        for (const auto& entry : _history) {
            for (const auto& session : entry.sessions) {
                for (const auto& ch : session.activeChannels) {
                    chFile << entry.timestamp << "," << session.sessionId << ","
                           << static_cast<int>(ch.channelId) << "," << ch.packetsSent << ","
                           << ch.packetsReceived << "," << ch.bytesSent << "," << ch.bytesReceived
                           << "\n";
                }
            }
        }

        LOG_INFO("Channel stats exported to {}", chFilename);
    }
}

void Recorder::_workerLoop(milliseconds interval)
{
    auto start = steady_clock::now();

    while (_isRunning) {
        std::unique_lock lock(_cvMutex);
        if (_cv.wait_for(lock, interval, [this]() { return !_isRunning; })) {
            break;
        }

        auto snapshot = _takeSnapshot();

        auto now = steady_clock::now();
        snapshot.timestamp = duration_cast<milliseconds>(now - start).count();

        {
            std::lock_guard historyLock(_historyMutex);
            _history.push_back(snapshot);
        }
    }
}

SystemSnapshot Recorder::_takeSnapshot()
{
    SystemSnapshot snap{};

    // 1. Global Network Stats (Peer)
    auto& net = _server.getNetworkMetrics();
    snap.totalBytesSent = net.totalBytesSent.load(std::memory_order_relaxed);
    snap.totalBytesReceived = net.totalBytesReceived.load(std::memory_order_relaxed);

    // 2. Session Stats
    {
        // Friend access to server sessions
        std::lock_guard lock(_server._sessionsMutex);
        for (const auto& [endpoint, session] : _server._sessions) {
            const auto& metrics = session->getSessionMetrics();

            SessionSnapshot sSnap{};
            sSnap.sessionId = session->getId();
            sSnap.rtt = metrics.rtt.load(std::memory_order_relaxed);
            sSnap.retransmitCount = metrics.retransmitCount.load(std::memory_order_relaxed);
            sSnap.duplicateCount = metrics.duplicateCount.load(std::memory_order_relaxed);

            // Channels
            for (size_t i = 0; i < metrics.channels.size(); ++i) {
                uint64_t ps = metrics.channels[i].packetsSent.load(std::memory_order_relaxed);
                uint64_t pr = metrics.channels[i].packetsReceived.load(std::memory_order_relaxed);
                uint64_t bs = metrics.channels[i].bytesSent.load(std::memory_order_relaxed);
                uint64_t br = metrics.channels[i].bytesReceived.load(std::memory_order_relaxed);

                if (ps > 0 || pr > 0) {
                    sSnap.activeChannels.push_back({static_cast<uint8_t>(i), ps, pr, bs, br});
                }
            }
            snap.sessions.push_back(sSnap);
        }
    }

    return snap;
}

}  // namespace rtnt::stat
