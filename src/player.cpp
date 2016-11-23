#include "player.hpp"

namespace radiotray
{

Player::Player()
{
}

bool
Player::init(int argc, char** argv)
{
    Gst::init(argc, argv);

    playbin = PlayBin::create();
    if (!playbin) {
        LOG(ERROR) << "The PlayBin element could not be created.";
        return false;
    }
    set_buffer_size(config->buffer_size);

    Glib::RefPtr<Gst::Bus> bus = playbin->get_bus();
    bus->add_watch(sigc::mem_fun(*this, &Player::on_bus_message));

    return true;
}

void
Player::play(Glib::ustring data_url, Glib::ustring station)
{
    init_streams(data_url, station);
    play();
}

void
Player::play()
{
    if (streams.empty()) {
        return;
    }

    Glib::ustring stream_url = streams.front();
    next_stream = std::next(std::begin(streams));

    stop();
    set_stream(stream_url);
    start();
}

void
Player::pause()
{
    playbin->set_state(Gst::STATE_PAUSED);
}

void
Player::stop()
{
    playbin->set_state(Gst::STATE_NULL);
}

void
Player::start()
{
    playbin->set_state(Gst::STATE_PLAYING);
}

Glib::RefPtr<PlayBin>
Player::get_playbin()
{
    return playbin;
}

void
Player::set_stream(Glib::ustring url)
{
    playbin->property_uri() = url;
}

void
Player::set_buffer_size(int size)
{
    playbin->property_buffer_size() = size;
}

void
Player::play_next_stream()
{
    auto stream_found = false;

    stop();

    while (next_stream != std::end(streams) and (not stream_found)) {
        auto u = *next_stream;
        next_stream++;

        if (gst_uri_is_valid(u.c_str())) {
            LOG(DEBUG) << "Trying to play stream: " << u;

            set_buffer_size(config->buffer_size);
            set_stream(u);
            start();

            stream_found = true;
        }
    }
}

bool
Player::on_bus_message(const Glib::RefPtr<Gst::Bus>& /*bus*/, const Glib::RefPtr<Gst::Message>& message)
{
    auto message_type = message->get_message_type();

    if (message_type == Gst::MESSAGE_EOS) {
        play_next_stream();
    } else if (message_type == Gst::MESSAGE_ERROR) {
        auto error_msg = Glib::RefPtr<Gst::MessageError>::cast_static(message);

        if (error_msg) {
            Glib::Error err = error_msg->parse();
            LOG(ERROR) << "Error: " << err.what();
        } else {
            LOG(ERROR) << "Error.";
        }

        play_next_stream();
    } else if (message_type == Gst::MESSAGE_TAG) {
        auto msg_tag = Glib::RefPtr<Gst::MessageTag>::cast_static(message);
        Gst::TagList tag_list;
#if GST_VERSION_MAJOR >= 1
        msg_tag->parse(tag_list);
#else
        Glib::RefPtr<Gst::Pad> pad;
        msg_tag->parse(pad, tag_list);
#endif
        if (tag_list.exists("title") && tag_list.size("title") > 0) {
            Glib::ustring title;
            auto ok = tag_list.get("title", title);
            if (ok) {
                em->broadcast_info_changed(current_station, title);
            }
        }
    } else if (message_type == Gst::MESSAGE_STATE_CHANGED) {
        auto state_changed_msg = Glib::RefPtr<Gst::MessageStateChanged>::cast_static(message);
        Gst::State new_state = state_changed_msg->parse();
        Gst::State old_state = state_changed_msg->parse_old();

        StationState st;
        if (new_state == Gst::State::STATE_PLAYING) {
            st = StationState::PLAYING;
        } else {
            st = StationState::IDLE;
        }

        em->state_changed(current_station, st);
        em->state = st;

        auto print = [](Gst::State& state) -> std::string {
            if (state == Gst::State::STATE_PLAYING) {
                return "STATE_PLAYING";
            } else if (state == Gst::State::STATE_NULL) {
                return "STATE_NULL";
            } else if (state == Gst::State::STATE_READY) {
                return "STATE_READY";
            } else if (state == Gst::State::STATE_PAUSED) {
                return "STATE_PAUSED";
            } else if (state == Gst::State::STATE_VOID_PENDING) {
                return "STATE_VOID_PENDING";
            }
            return "STATE_UNKNOWN";
        };
        LOG(DEBUG) << "Type: Gst::MESSAGE_STATE_CHANGED."
                   << " Old: " << print(old_state) << " New: " << print(new_state)
                   << " Source: " << state_changed_msg->get_source()->get_name();
    }

    return true;
}

Glib::ustring
Player::get_station()
{
    return current_station;
}

bool
Player::has_station()
{
    return (current_station.empty() == false);
}

void
Player::init_streams(Glib::ustring data_url, Glib::ustring station)
{
    bool ok;

    std::tie(ok, streams) = playlist.get_streams(data_url);
    if ((not ok) or streams.empty()) {
        // TODO: D-Bus message
        LOG(ERROR) << "Couldn't get audio streams!";
        return;
    }

    current_station = station;
}

void
Player::set_config(std::shared_ptr<Config> cfg)
{
    config = cfg;
    playlist.set_config(cfg);
}

} // namespace radiotray
