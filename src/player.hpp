#ifndef __PLAYER_HPP_INCLUDED__
#define __PLAYER_HPP_INCLUDED__

#include <thread>

#include <gstreamermm.h>
#include <glibmm.h>

#include "playlist.hpp"

namespace radiotray {

class Player {
public:
    Player();

    Player(const Player&) = delete;

    bool init(int argc, char **argv);
    void play(Glib::ustring url);
    Glib::RefPtr<Gst::PlayBin2> get_playbin();

private:
    Glib::RefPtr<Glib::MainLoop> mainloop;
    Glib::RefPtr<Gst::PlayBin2> playbin;

    Playlist playlist;
    MediaStreams streams;
    MediaStreams::iterator next_stream;

    bool on_bus_message(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);
};

} // namespace radiotray

#endif
