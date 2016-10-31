#include "player.hpp"

namespace radiotray {

Player::Player()
{
}

bool
Player::init(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <uri>" << std::endl;
        return false;
    }

    Gst::init(argc, argv);

    bool ok;
    std::tie(ok, streams) = playlist.get_streams(argv[1]);
    if ((not ok) or streams.empty()) {
        LOG(ERROR) << "Couldn't get audio streams!";
        return false;
    }
    next_stream = streams.begin();

    playbin = Gst::PlayBin2::create();
    if (!playbin) {
        std::cerr << "The playbin2 element could not be created." << std::endl;
        return false;
    }
    playbin->property_buffer_size() = 1024 * 100;

    mainloop = Glib::MainLoop::create();
    Glib::RefPtr<Gst::Bus> bus = playbin->get_bus();
    bus->add_watch(sigc::mem_fun(*this, &Player::on_bus_message));

    return true;
}

void
Player::play()
{
    Glib::ustring uri = *next_stream;
    next_stream++;

    playbin->property_uri() = uri;
    playbin->set_state(Gst::STATE_PLAYING);

    KeyboardInputThread keyboard_io(playbin);
    std::thread t(std::ref(keyboard_io));
    t.detach();

    mainloop->run();

    // cleanup
    playbin->set_state(Gst::STATE_NULL);
}

bool
Player::on_bus_message(const Glib::RefPtr<Gst::Bus>& /*bus*/, const Glib::RefPtr<Gst::Message>& message)
{
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
        }

        return false;
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
}

} // namespace radiotray

