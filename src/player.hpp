#ifndef __PLAYER_HPP_INCLUDED__
#define __PLAYER_HPP_INCLUDED__

#include <thread>
#include <memory>

#include <glib.h>

#include <gst/gstversion.h>

#include <gstreamermm.h>
#include <gstreamermm/playbin.h>

#include <glibmm.h>

#include "playlist.hpp"
#include "event_manager.hpp"

namespace radiotray
{
#if GST_VERSION_MAJOR >= 1
    typedef Gst::PlayBin PlayBin;
#else
    typedef Gst::PlayBin2 PlayBin;
#endif

class Player
{
public:
    Player();

    Player(const Player&) = delete;

    bool init(int argc, char** argv);
    void play(Glib::ustring url, Glib::ustring station = Glib::ustring());
    void play();
    void pause();
    void stop();
    void start();
    // void quit();
    Glib::ustring get_station();
    Glib::RefPtr<PlayBin> get_playbin();

    std::shared_ptr<EventManager> em;

private:
    // Glib::RefPtr<Glib::MainLoop> mainloop;
    Glib::RefPtr<PlayBin> playbin;

    Playlist playlist;
    MediaStreams streams;
    MediaStreams::iterator next_stream;

    Glib::ustring current_station;

    std::thread mainloop_thr;

    bool on_bus_message(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);
    void set_stream(Glib::ustring url);
    void set_buffer_size(int size);
    void play_next_stream();
    // void gstreamer_loop();
};

} // namespace radiotray

#endif
