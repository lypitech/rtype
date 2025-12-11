#include "logger/Logger.h"

namespace rt::callback {

void onConnect() { LOG_DEBUG("Connected to peer."); }

}  // namespace rt::callback
