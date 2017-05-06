#ifndef __CONFIG_HPP_INCLUDED__
#define __CONFIG_HPP_INCLUDED__

#include <chrono>
#include <string>

#include <gtkmm.h>

#include "easyloggingpp/easylogging++.h"
#include "pugixml/pugixml.hpp"

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
    void set_last_played_station(Glib::ustring station);
    Glib::ustring get_last_played_station() const;

    long url_timeout_ms = kDefaultHTTPRequestTimeout;
    int buffer_size = kDefaultGStreamerBufferSize;

private:
    std::string filename;
    pugi::xml_document config;
    Glib::ustring last_station;

    void create_new_config();
    void save_config();
};

} // namespace radiotray

#endif
