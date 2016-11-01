#include <thread>
#include <chrono>
#include <iostream>

#include <gstreamermm.h>
#include <glibmm.h>

#include <stdlib.h>
#include <termios.h>

#include "playlist.hpp"
#include "player.hpp"
#include "keyboard_io.hpp"

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

    Player player;

    auto ok = player.init(argc, argv);
    if (ok) {
        KeyboardInputThread keyboard_io(player.get_playbin());
        std::thread t(std::ref(keyboard_io));

        player.play(argv[1]);

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    return EXIT_SUCCESS;
}
