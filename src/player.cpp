#include "player.hpp"

namespace radiotray {

Player::Player()
{
}

bool
Player::init(int argc, char **argv)
{
    Gst::init(argc, argv);

    playbin = Gst::PlayBin2::create();
    if (!playbin) {
        LOG(ERROR) << "The playbin2 element could not be created.";
        return false;
    }
    set_buffer_size(1024 * 100);

    mainloop = Glib::MainLoop::create();
    Glib::RefPtr<Gst::Bus> bus = playbin->get_bus();
    bus->add_watch(sigc::mem_fun(*this, &Player::on_bus_message));

    mainloop_thr = std::thread([this](){
        mainloop->run();
        stop(); // cleanup
    });

    return true;
}

void
Player::play(Glib::ustring data_url)
{
    bool ok;
    std::tie(ok, streams) = playlist.get_streams(data_url);
    if ((not ok) or streams.empty()) {
        LOG(ERROR) << "Couldn't get audio streams!";
        return;
    }

    Glib::ustring stream_url = streams.front();
    next_stream = std::next(std::begin(streams));

    stop();
    set_stream(stream_url);
    start();
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

Glib::RefPtr<Gst::PlayBin2>
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

            set_buffer_size(1024 * 100);
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
        auto error_msg = Glib::RefPtr<Gst::MessageError>::cast_dynamic(message);

        if (error_msg) {
            Glib::Error err = error_msg->parse();
            LOG(ERROR) << "Error: " << err.what();
        } else {
            LOG(ERROR) << "Error.";
        }

        play_next_stream();
    } else if (message_type == Gst::MESSAGE_TAG) {
        auto msg_tag = Glib::RefPtr<Gst::MessageTag>::cast_dynamic(message);
        Gst::TagList tag_list;
        Glib::RefPtr<Gst::Pad> pad;
        msg_tag->parse(pad, tag_list);
        if (tag_list.exists("title") && tag_list.size("title") > 0) {
            std::string v;
            auto ok = tag_list.get("title", v);
            if (ok) {
                std::cerr << "Playing: " << v << std::endl;
            }
        }
    } else if (message_type == Gst::MESSAGE_STATE_CHANGED) {
        auto state_changed_msg = Glib::RefPtr<Gst::MessageStateChanged>::cast_dynamic(message);
        Gst::State new_state = state_changed_msg->parse();
        Gst::State old_state = state_changed_msg->parse_old();

        auto print = [](Gst::State& state) -> std::string
        {
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
        std::cerr << "Type: Gst::MESSAGE_STATE_CHANGED." << " Old: " << print(old_state) << " New: " << print(new_state) << std::endl;
    }

    return true;

#if 0
    switch (message->get_message_type()) {
    case Gst::MESSAGE_EOS:
        std::cout << std::endl
                  << "End of stream" << std::endl;
        mainloop->quit();
        return false;
    case Gst::MESSAGE_ERROR: {
        Glib::RefPtr<Gst::MessageError> error_msg = Glib::RefPtr<Gst::MessageError>::cast_dynamic(message);

        if (error_msg) {
            Glib::Error err;
            err = error_msg->parse();
            LOG(ERROR) << "Error: " << err.what();
        } else {
            LOG(ERROR) << "Error.";
        }

        auto stream_found = false;

        while (next_stream != streams.end() or (not stream_found)) {
            auto u = *next_stream;
            next_stream++;

            if (gst_uri_is_valid(u.c_str())) {
                LOG(DEBUG) << "Trying to play stream: " << u;

                playbin->set_state(Gst::STATE_NULL);
                playbin->property_uri() = u;
                playbin->property_buffer_size() = 1024 * 100;
                playbin->set_state(Gst::STATE_PLAYING);

                stream_found = true;
            }
        }

        if (not stream_found) {
            mainloop->quit();
            return false;
        }

        return true;
    }
    case Gst::MESSAGE_TAG: {
        Glib::RefPtr<Gst::MessageTag> msg_tag = Glib::RefPtr<Gst::MessageTag>::cast_dynamic(message);
        Gst::TagList tag_list;
        Glib::RefPtr<Gst::Pad> pad;
        msg_tag->parse(pad, tag_list);
        if (tag_list.exists("title") && tag_list.size("title") > 0) {
            std::string v;
            auto ok = tag_list.get("title", v);
            if (ok) {
                std::cerr << "Playing: " << v << std::endl;
            }
        }
        break;
    }
    default:
        break;
    }

    return true;
#endif
}

} // namespace radiotray

