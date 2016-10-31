#include <thread>
#include <iostream>

#include <gstreamermm.h>
#include <glibmm.h>

#include <stdlib.h>
#include <termios.h>

#include "playlist.hpp"
#include "player.hpp"

INITIALIZE_EASYLOGGINGPP

int
main(int argc, char** argv)
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    // Values are always std::string
    defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", defaultConf);

    radiotray::Player player;

    auto ok = player.init(argc, argv);
    if (ok) {
        player.play();
    }

    return EXIT_SUCCESS;
}
