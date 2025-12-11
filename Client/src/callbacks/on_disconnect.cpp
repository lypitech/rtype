#include "logger/Logger.h"

namespace rt::callback {

void onDisconnect() { LOG_DEBUG("Disconnected from peer."); }

}  // namespace rt::callback
