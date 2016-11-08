#ifndef __EVENT_MANAGER_HPP_INCLUDED__
#define __EVENT_MANAGER_HPP_INCLUDED__

#include <string>
#include <memory>

#include <gtkmm.h>
#include <sigc++/sigc++.h>

namespace radiotray
{
enum class Event {
    EMPTY,
    STATE_CHANGED,
    STATION_INFO_CHANGED,
    BOOKMARKS_RELOADED,
    BOOKMARKS_CHANGED,
    STATION_ERROR,
    VOLUME_CHANGED,
    NOTIFICATION
};

enum class StationState { UNKNOWN, IDLE, CONNECTING, PLAYING };

using BroadcastInfoChangedSignal = sigc::signal<void, Glib::ustring /*station*/, Glib::ustring /*info*/>;
using StateChangedSignal = sigc::signal<void, Glib::ustring /*station*/, StationState /*state*/>;

class EventManager
{
public:
    EventManager() = default;

    StationState state = StationState::UNKNOWN;

    BroadcastInfoChangedSignal broadcast_info_changed;
    StateChangedSignal state_changed;
};

} // namespace radiotray

#endif
