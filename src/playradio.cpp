#include <thread>
#include <chrono>
#include <iostream>

#include <gstreamermm.h>
#include <glibmm.h>

#include <stdlib.h>
#include <termios.h>

#include "playlist.hpp"
#include "player.hpp"
#include "keyboard_control.hpp"

INITIALIZE_EASYLOGGINGPP

using namespace radiotray;

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

    auto player = std::make_shared<Player>();
    auto ok = player->init(argc, argv);

    if (ok) {
        KeyboardControl keyboard(player, stations);
        std::thread t(std::ref(keyboard));

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return EXIT_SUCCESS;
}
