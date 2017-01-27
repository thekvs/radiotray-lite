#include "tray.hpp"

INITIALIZE_EASYLOGGINGPP

using namespace radiotray;

int
main(int argc, char* argv[])
{
    el::Configurations easylogging_config;
    easylogging_config.setToDefault();
    // Values are always std::string

#ifndef NDEBUG
    easylogging_config.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    easylogging_config.set(el::Level::Error, el::ConfigurationType::Format, "%datetime %level %loc %msg");
    easylogging_config.set(el::Level::Warning, el::ConfigurationType::Format, "%datetime %level %loc %msg");
#else
    easylogging_config.set(el::Level::Info, el::ConfigurationType::Format, "%level %msg");
    easylogging_config.set(el::Level::Error, el::ConfigurationType::Format, "%level %msg");
    easylogging_config.set(el::Level::Warning, el::ConfigurationType::Format, "%level %msg");
#endif
    // do not log to a file
    easylogging_config.setGlobally(el::ConfigurationType::ToFile, "false");

    // default logger uses default configurations
    el::Loggers::reconfigureLogger("default", easylogging_config);

    RadioTrayLite rtl;

    auto ok = rtl.init(argc, argv);
    if (not ok) {
        LOG(ERROR) << "Initialization failed";
        return EXIT_FAILURE;
    }

    auto rc = rtl.run();

    return rc;
}
