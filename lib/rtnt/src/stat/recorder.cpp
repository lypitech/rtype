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
    SystemSnapshot snapshot{};

    const auto& metrics = _peer.getNetworkMetrics();
    snapshot.totalBytesSent = metrics.totalBytesSent.load(std::memory_order_relaxed);
    snapshot.totalBytesReceived = metrics.totalBytesReceived.load(std::memory_order_relaxed);

    auto snapshotSession = [&](const std::shared_ptr<core::Session>& session) {
        if (!session) {
            return;
        }

        const auto& sessionMetrics = session->getSessionMetrics();
        SessionSnapshot snap{};

        snap.sessionId = session->getId();
        snap.rtt = sessionMetrics.rtt.load(std::memory_order_relaxed);
        snap.retransmitCount = sessionMetrics.retransmitCount.load(std::memory_order_relaxed);
        snap.duplicateCount = sessionMetrics.duplicateCount.load(std::memory_order_relaxed);

        for (size_t i = 0; i < sessionMetrics.channels.size(); ++i) {
            uint64_t ps = sessionMetrics.channels[i].packetsSent.load(std::memory_order_relaxed);
            uint64_t pr =
                sessionMetrics.channels[i].packetsReceived.load(std::memory_order_relaxed);
            uint64_t bs = sessionMetrics.channels[i].bytesSent.load(std::memory_order_relaxed);
            uint64_t br = sessionMetrics.channels[i].bytesReceived.load(std::memory_order_relaxed);

            if (ps > 0 || pr > 0) {
                snap.activeChannels.push_back({static_cast<uint8_t>(i), ps, pr, bs, br});
            }
        }
        snapshot.sessions.push_back(snap);
    };

    if (_server) {  // if server, then snapshot all sessions
        std::lock_guard lock(_server->_sessionsMutex);
        for (const auto& [endpoint, session] : _server->_sessions) {
            snapshotSession(session);
        }
    } else if (_client) {  // and client only has one.
        std::lock_guard lock(_client->_mutex);
        snapshotSession(_client->_serverSession);
    }

    return snapshot;
}

}  // namespace rtnt::stat
