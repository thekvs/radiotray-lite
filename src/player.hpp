#ifndef __PLAYER_HPP_INCLUDED__
#define __PLAYER_HPP_INCLUDED__

#include <memory>
#include <thread>

#include <glib.h>

#include <gst/gstversion.h>

#include <gstreamermm.h>
#include <gstreamermm/playbin.h>

#include <glibmm.h>

#include "event_manager.hpp"
#include "playlist.hpp"

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
    Player() = default;
    Player(const Player&) = delete;

    bool init(int argc, char** argv);
    void play(Glib::ustring url, Glib::ustring station = Glib::ustring());
    void play();
    void pause();
    void stop();
    void start();

    Glib::ustring get_station();
    bool has_station();
    Glib::RefPtr<PlayBin> get_playbin();

    void init_streams(Glib::ustring data_url, Glib::ustring station);
    void set_config(std::shared_ptr<Config> cfg);

    std::shared_ptr<EventManager> em;

private:
    std::shared_ptr<Config> config;
    Glib::RefPtr<PlayBin> playbin;

    Playlist playlist;
    MediaStreams streams;
    MediaStreams::iterator next_stream;

    Glib::ustring current_station;

    bool on_bus_message(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);
    void set_stream(Glib::ustring url);
    void set_buffer_size(int size);
    void play_next_stream();
};

} // namespace radiotray

#endif
