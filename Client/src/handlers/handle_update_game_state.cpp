#include "app.hpp"
#include "handlers.hpp"

namespace packet::handler {

void handleUpdateGameState(UpdateGameState packet,
                           client::HandlerToolbox& toolbox)
{
    LOG_TRACE_R2("Handling UpdateGameState packet...");
    toolbox.engine.setGameState(packet.gameState);
    LOG_TRACE_R3("Updated game state to {}", packet.gameState);
}

}  // namespace packet::handler
