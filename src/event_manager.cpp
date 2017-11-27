#include "event_manager.hpp"

namespace radiotray
{

std::string
get_station_state_desc(StationState state)
{
    switch (state) {
    case StationState::CONNECTING:
        return "CONNECTING";
    case StationState::IDLE:
        return "IDLE";
    case StationState::PLAYING:
        return "PLAYING";
    case StationState::UNKNOWN:
        return "UNKNOWN";
    default:
        return "OOPS";
    }
}

} // namespace radiotray
