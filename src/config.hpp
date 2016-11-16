#ifndef __CONFIG_HPP_INCLUDED__
#define __CONFIG_HPP_INCLUDED__

#include <string>
#include <chrono>

#include <gtkmm.h>

#include "pugixml/pugixml.hpp"
#include "easyloggingpp/easylogging++.h"

#include "constants.hpp"

namespace radiotray
{
class Config
{
public:

    Config() = default;
    ~Config();

    void set_config_file(const std::string& name);
    void load_config();
    bool has_last_station();

    Glib::ustring last_station;
    long url_timeout_ms = kDefaultHTTPRequestTimeout;
    size_t buffer_size = kDefaultGStreamerBufferSize;

private:

    std::string filename;
    pugi::xml_document config;
};

} // namespace radiotray

#endif
