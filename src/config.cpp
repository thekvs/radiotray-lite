#include "config.hpp"

namespace radiotray
{
Config::~Config()
{
    if (last_station.empty() or filename.empty()) {
        return;
    }

    if (config.first_child().empty()) { // create new config
        auto root = config.append_child("config");

        {
            auto option = root.append_child("option");
            option.append_attribute("name").set_value("last_station");
            option.append_attribute("value").set_value(last_station.c_str());
        }

        {
            auto option = root.append_child("option");
            option.append_attribute("name").set_value("buffer_size");
            option.append_attribute("value").set_value(buffer_size);
        }

        {
            auto option = root.append_child("option");
            option.append_attribute("name").set_value("url_timeout");
            option.append_attribute("value").set_value(static_cast<float>(url_timeout_ms) / 1000.f);
        }
    } else {
        try {
            pugi::xpath_node node = config.select_node("/config/option[@name='last_station']");
            if (not node.node().empty()) {
                // replace attribute's value
                node.node().attribute("value").set_value(last_station.c_str());
            }
        } catch (pugi::xpath_exception& exc) {
            LOG(ERROR) << "XPath expression failed: " << exc.what();
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

            node = config.select_node("/config/option[@name='buffer_size']");
            if (not node.node().empty()) {
                buffer_size = node.node().attribute("value").as_int();
            }

            node = config.select_node("/config/option[@name='url_timeout']");
            if (not node.node().empty()) {
                url_timeout_ms = node.node().attribute("value").as_float() * 1000; /* 'url_timeout' is in seconds */
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
