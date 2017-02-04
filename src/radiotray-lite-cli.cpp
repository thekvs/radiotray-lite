#include <chrono>
#include <iostream>
#include <thread>

#include <glibmm.h>
#include <gstreamermm.h>

#include <stdlib.h>
#include <termios.h>

#include "player.hpp"
#include "playlist.hpp"
#include "keyboard_control.hpp"

INITIALIZE_EASYLOGGINGPP

using namespace radiotray;

void
on_broadcast_info_changed_signal(Glib::ustring /*station*/, Glib::ustring info)
{
    std::cout << "playing: " << info << std::endl;
}

int
main(int argc, char** argv)
{
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

    std::vector<std::string> stations;
    for (int i = 1; i < argc; i++) {
        stations.push_back(argv[i]);
    }

    auto config = std::make_shared<Config>();

    auto player = std::make_shared<Player>();
    player->em = std::make_shared<EventManager>(); // FIXME: EventManager should be part of Player?
    player->em->broadcast_info_changed.connect(sigc::ptr_fun(on_broadcast_info_changed_signal));
    player->set_config(config);

    auto ok = player->init(argc, argv);

    if (ok) {
        KeyboardControl keyboard(player, stations);
        std::thread t(std::ref(keyboard));

        auto mainloop = Glib::MainLoop::create();
        mainloop->run();
    }

    return EXIT_SUCCESS;
}
