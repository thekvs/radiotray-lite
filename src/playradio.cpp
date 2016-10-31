/* gstreamermm - a C++ wrapper for gstreamer
 *
 * Copyright 2011 The gstreamermm Development Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <gstreamermm.h>
#include <glibmm.h>
#include <iostream>
#include <stdlib.h>

#include "playlist.hpp"

namespace
{

Glib::RefPtr<Glib::MainLoop> mainloop;

// This function is used to receive asynchronous messages in the main loop.
bool
on_bus_message(const Glib::RefPtr<Gst::Bus>& /* bus */, const Glib::RefPtr<Gst::Message>& message)
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

        mainloop->quit();

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
    }; break;
    default:
        break;
    }

    return true;
}

} // anonymous namespace

int
play(std::string u)
{
    // Create a playbin2 element.
    Glib::RefPtr<Gst::PlayBin2> playbin = Gst::PlayBin2::create();
    if (!playbin) {
        std::cerr << "The playbin2 element could not be created." << std::endl;
        return EXIT_FAILURE;
    }

    // Create the main loop.
    mainloop = Glib::MainLoop::create();

    // Get the bus from the playbin, and add a bus watch to the default main
    // context with the default priority:
    Glib::RefPtr<Gst::Bus> bus = playbin->get_bus();
    bus->add_watch(sigc::ptr_fun(&on_bus_message));

    Glib::ustring uri;
    if (gst_uri_is_valid(u.c_str())) {
        uri = u;
    } else {
        return -1;
    }

    LOG(DEBUG) << "Trying to play stream: " << u;

    // Set the playbyin2's uri property.
    playbin->property_uri() = uri;
    playbin->property_buffer_size() = 1024 * 100;

    // Now set the playbin to the PLAYING state and start the main loop:
    playbin->set_state(Gst::STATE_PLAYING);
    mainloop->run();

    // cleanup
    playbin->set_state(Gst::STATE_NULL);

    return 0;
}

INITIALIZE_EASYLOGGINGPP

int
main(int argc, char** argv)
{
    // Initialize gstreamermm:
    Gst::init(argc, argv);

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <uri>" << std::endl;
        return EXIT_FAILURE;
    }

    el::Configurations defaultConf;
    defaultConf.setToDefault();
    // Values are always std::string
    defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", defaultConf);

    playradio::Playlist plist;
    playradio::MediaStreams streams;
    bool ok;

    std::tie(ok, streams) = plist.get_streams(argv[1]);
    if ((not ok) or streams.empty()) {
        LOG(ERROR) << "Couldn't get audio streams!";
        return EXIT_FAILURE;
    }

    for (auto& u : streams) {
        play(u);
    }

    return EXIT_SUCCESS;
}
