#include "config.hpp"

namespace radiotray
{
Config::~Config()
{
    if (last_station.empty() or filename.empty()) {
        return;
    }

    if (config.first_child().empty()) {
        auto option = config.append_child("config").append_child("option");
        option.append_attribute("name").set_value("last_station");
        option.append_attribute("value").set_value(last_station.c_str());
    } else {
        try {
            pugi::xpath_node node = config.select_node("/config/option[@name='last_station']");
            if (not node.node().empty()) {
                // replace attribute's value
                node.node().attribute("value").set_value(last_station.c_str());
            }
        } catch (pugi::xpath_exception& exc) {
            LOG(WARNING) << "XPath expression failed: " << exc.what();
            return;
        }
    }

    config.save_file(filename.c_str(), "  ");
}

void
Config::set_config_file(const std::string& name)
{
    filename = name;
}

void
Config::load_config()
{
    if (filename.empty()) {
        LOG(WARNING) << "Name of a configuration file is not specified!";
        return;
    }

    pugi::xml_parse_result result = config.load_file(filename.c_str());
    if (result) {
        try {
            pugi::xpath_node node = config.select_node("/config/option[@name='last_station']");
            if (not node.node().empty()) {
                last_station = node.node().attribute("value").as_string();
            }
        } catch (pugi::xpath_exception& exc) {
            LOG(ERROR) << "XPath query error: " << exc.what() << " File: " << filename;
        }
    } else {
        LOG(ERROR) << "Failed to parse configuration file: " << result.description();
    }
}

bool
Config::has_last_station()
{
    return (last_station.empty() == false);
}

} // namespace radiotray
