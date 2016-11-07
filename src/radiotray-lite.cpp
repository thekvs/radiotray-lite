#include "tray.hpp"

INITIALIZE_EASYLOGGINGPP

using namespace radiotray;

int
main(int argc, char* argv[])
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    // Values are always std::string
    defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    defaultConf.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", defaultConf);

    RadioTrayLite(argc, argv).run();

    return 0;
}
