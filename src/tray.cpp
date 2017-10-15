#include "tray.hpp"

namespace radiotray
{

RadioTrayLite::BookmarksWalker::BookmarksWalker(RadioTrayLite& radiotray, Gtk::Menu* menu)
    : radiotray(radiotray)
{
    menus.push(menu);
}

bool
RadioTrayLite::BookmarksWalker::for_each(pugi::xml_node& node)
{
    auto name = node.name();
    auto attr_name = node.attribute("name");
    auto attr_url = node.attribute("url");

    if (attr_name.empty() or strcasecmp(attr_name.as_string(), "root") == 0) {
        return true; // continue traversal
    }

    auto is_group = strcasecmp(name, "group") == 0;
    auto is_bookmark = strcasecmp(name, "bookmark") == 0;

    if (is_group) {
        while (menus.size() >= static_cast<size_t>(depth())) {
            menus.pop();
        }
        auto group_name = attr_name.as_string();
        auto menu_item = Gtk::manage(new Gtk::MenuItem(group_name));
        auto submenu = Gtk::manage(new Gtk::Menu());
        menus.top()->append(*menu_item);
        menu_item->set_submenu(*submenu);
        menus.push(submenu);
        level = depth();
        LOG(DEBUG) << "Group: " << group_name << ", depth: " << depth();
    } else if (is_bookmark and (!attr_url.empty())) {
        auto station_name = attr_name.as_string();
        auto station_group_name = node.parent().attribute("name").as_string();

        if (strncasecmp(station_name, kSeparatorPrefix, kSeparatorPrefixLength) == 0) {
            auto separator = Gtk::manage(new Gtk::SeparatorMenuItem());
            menus.top()->append(*separator);
        } else {
            auto station_url = attr_url.as_string();
            auto sub_item = Gtk::manage(new Gtk::MenuItem(station_name));
            sub_item->signal_activate().connect(sigc::bind<Glib::ustring, Glib::ustring, Glib::ustring>(
                sigc::mem_fun(radiotray, &RadioTrayLite::on_station_button), station_group_name, station_name, station_url));
            menus.top()->append(*sub_item);
        }

        LOG(DEBUG) << "Bookmark depth: " << depth() << ", level: " << level << ", #menus: " << menus.size() << ", station: " << station_name
                   << ", group: " << station_group_name;
    }

    return true; // continue traversal
}

RadioTrayLite::~RadioTrayLite()
{
    clear_menu();

    if (indicator != nullptr) {
        g_object_unref(G_OBJECT(indicator));
    }
}

bool
RadioTrayLite::init(int argc, char** argv, std::shared_ptr<CmdLineOptions>& opts)
{
    cmd_line_options = opts;
    config = std::make_shared<Config>();

    load_configuration();

    // app = Gtk::Application::create(argc, argv, "github.com.thekvs.radiotray-lite");
    app = Gtk::Application::create("github.com.thekvs.radiotray-lite");
    app->register_application();
    if (app->is_remote()) {
        LOG(WARNING) << "This application is already running!";
        return false;
    }

    menu = std::make_shared<Gtk::Menu>();

    player = std::make_shared<Player>();
    player->set_config(config);
    auto ok = player->init(argc, argv);
    if (not ok) {
        return false;
    }

    notifier = std::make_shared<Notification>(kAppName, config);
    ok = notifier->init();
    if (not ok) {
        return false;
    }

    em = std::make_shared<EventManager>();
    em->state_changed.connect(sigc::mem_fun(*this, &RadioTrayLite::on_station_changed_signal));
    em->broadcast_info_changed.connect(sigc::mem_fun(*this, &RadioTrayLite::on_broadcast_info_changed_signal));
    em->broadcast_info_changed.connect(sigc::mem_fun(*notifier, &Notification::on_broadcast_info_changed_signal));

    player->em = em;

    indicator = app_indicator_new_with_path(kAppName, kAppIndicatorIconOff, APP_INDICATOR_CATEGORY_APPLICATION_STATUS, kImagePath);

    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon(indicator, kAppIndicatorIconOn);
    app_indicator_set_menu(indicator, menu->gobj());

    initialized = true;

    return true;
}

int
RadioTrayLite::run()
{
    if (not initialized) {
        LOG(ERROR) << "Application wasn't properly initialized!";
        return -1;
    }

    build_menu();

    app->hold();

    // resume the last played staion in timer callback
    sigc::slot<bool> resume_slot = sigc::bind(sigc::mem_fun(*this, &RadioTrayLite::resume), cmd_line_options->resume);
    sigc::connection conn = Glib::signal_timeout().connect(resume_slot, 200);

    auto rc = app->run();

    return rc;
}

void
RadioTrayLite::on_quit_button()
{
    LOG(DEBUG) << "'Quit' button was pressed.";

    player->stop();
    app->release();
}

void
RadioTrayLite::on_about_button()
{
    LOG(DEBUG) << "'About' button was pressed.";

    AboutDialog about;
    about.run();
}

void
RadioTrayLite::on_station_button(Glib::ustring group_name, Glib::ustring station_name, Glib::ustring station_url)
{
    player->play(station_url, station_name);

    LOG(DEBUG) << "'" << station_url << "'"
               << "(group: " << group_name << ", station: " << station_name << ")"
               << " button was pressed.";
}

void
RadioTrayLite::on_reload_button()
{
    LOG(DEBUG) << "'Reload'"
               << " button was pressed";
    rebuild_menu();
}

void
RadioTrayLite::on_current_station_button()
{
    if (em->state == StationState::PLAYING) {
        player->pause();
    } else if (em->state == StationState::IDLE or em->state == StationState::UNKNOWN) {
        player->play();
    }
}

bool
RadioTrayLite::resume(bool resume_last_station)
{
    if (resume_last_station) {
        if (config->has_last_station()) {
            Glib::ustring data_url;
            try {
                std::stringstream xpath_query;
                xpath_query << "//bookmark[@name='" << config->get_last_played_station() << "']";

                pugi::xpath_node node = bookmarks_doc.select_node(xpath_query.str().c_str());
                if (not node.node().empty()) {
                    data_url = node.node().attribute("url").as_string();
                }
            } catch (pugi::xpath_exception& exc) {
                LOG(WARNING) << "XPath error: " << exc.what();
            }

            LOG(DEBUG) << "Resuming the last played station: " << config->get_last_played_station() << " (stream url: " << data_url << ")";
            player->play(data_url, config->get_last_played_station());
        }
    }

    // When we return false from the timer callback it deletes itself automatically
    // and won't be executed any more. So we have one time event here.
    return false;
}

void
RadioTrayLite::build_menu()
{
    auto bookmarks_parsed = parse_bookmarks_file();
    if (bookmarks_parsed) {
        BookmarksWalker walker(*this, &(*menu));
        bookmarks_doc.traverse(walker);
    } else {
        // TODO: notify about parsing errors
    }

    Glib::ustring name;

    auto separator_item = Gtk::manage(new Gtk::SeparatorMenuItem());
    menu->append(*separator_item);

    name = "Reload Bookmarks";
    auto menu_item = Gtk::manage(new Gtk::MenuItem(name));
    menu_item->signal_activate().connect(sigc::mem_fun(*this, &RadioTrayLite::on_reload_button));
    menu->append(*menu_item);

    name = "About";
    menu_item = Gtk::manage(new Gtk::MenuItem(name));
    menu_item->signal_activate().connect(sigc::mem_fun(*this, &RadioTrayLite::on_about_button));
    menu->append(*menu_item);

    name = "Quit";
    menu_item = Gtk::manage(new Gtk::MenuItem(name));
    menu_item->signal_activate().connect(sigc::mem_fun(*this, &RadioTrayLite::on_quit_button));
    menu->append(*menu_item);

    separator_item = Gtk::manage(new Gtk::SeparatorMenuItem());
    menu->prepend(*separator_item);

    set_current_broadcast();

    auto turn_on = not(em->state == StationState::PLAYING);
    set_current_station(turn_on);
}

void
RadioTrayLite::rebuild_menu()
{
    clear_menu();
    build_menu();
}

void
RadioTrayLite::clear_menu()
{
    if (menu) {
        for (auto& e : menu->get_children()) {
            menu->remove(*e);
            delete e;
        }
    }

    current_station_menu_entry = nullptr;
    current_broadcast_menu_entry = nullptr;
}

bool
RadioTrayLite::parse_bookmarks_file()
{
    bool status = false;

    if (not bookmarks_file.empty()) {
        pugi::xml_parse_result result = bookmarks_doc.load_file(bookmarks_file.c_str());
        if (result) {
            status = true;
        } else {
            LOG(ERROR) << "XML parser failed: " << result.description();
        }
    } else {
        LOG(WARNING) << "Bookmarks file not specified!";
    }

    return status;
}

void
RadioTrayLite::load_configuration()
{
    std::string user_config_dir, dist_config_dir;
    bool has_user_bookmarks = false;

    auto home = getenv("HOME");
    if (home != nullptr) {
        user_config_dir = std::string(home) + "/.config/" + kAppDirName + "/";
    }

    dist_config_dir = std::string(INSTALL_PREFIX "/share/") + kAppDirName + "/";

    if (file_exists(user_config_dir, kBookmarksFileName)) {
        bookmarks_file = user_config_dir + kBookmarksFileName;
        has_user_bookmarks = true;
    }

    if (not has_user_bookmarks) {
        if (file_exists(dist_config_dir, kBookmarksFileName)) {
            bookmarks_file = dist_config_dir + kBookmarksFileName;
            copy_default_bookmarks(bookmarks_file);
        } else {
            LOG(WARNING) << "Distribution's bookmarks file doesn't exist in '" << dist_config_dir << "'";
        }
    }

    if (dir_exists(user_config_dir)) {
        config->set_config_file(user_config_dir + kConfigFileName);
        if (file_exists(user_config_dir, kConfigFileName)) {
            config->load_config();
        }
    }
}

void
RadioTrayLite::set_current_station(bool turn_on)
{
    if ((not player->has_station()) and config->has_last_station()) {
        try {
            std::stringstream xpath_query;
            xpath_query << "//bookmark[@name='" << config->get_last_played_station() << "']";

            pugi::xpath_node node = bookmarks_doc.select_node(xpath_query.str().c_str());
            if (not node.node().empty()) {
                auto data_url = node.node().attribute("url").as_string();
                player->init_streams(data_url, config->get_last_played_station());
            }
        } catch (pugi::xpath_exception& exc) {
            LOG(WARNING) << "XPath error: " << exc.what();
        }
    }

    if (player->has_station()) {
        auto mk_menu_entry = [](Glib::ustring name, bool turn_on) {
            std::stringstream ss;
            if (turn_on) {
                ss << "Turn On \"" << name << "\"";
            } else {
                ss << "Turn Off \"" << name << "\"";
            }

            return Glib::ustring(ss.str());
        };

        if (current_station_menu_entry == nullptr) {
            current_station_menu_entry = Gtk::manage(new Gtk::MenuItem(mk_menu_entry(player->get_station(), turn_on)));
            current_station_menu_entry->signal_activate().connect(sigc::mem_fun(*this, &RadioTrayLite::on_current_station_button));
            menu->prepend(*current_station_menu_entry);
        } else {
            current_station_menu_entry->set_label(mk_menu_entry(player->get_station(), turn_on));
        }
    }

    menu->show_all();
}

void
RadioTrayLite::set_current_broadcast(Glib::ustring info)
{
    auto split = [](const Glib::ustring& info, size_t size) {
        if (info.size() <= size) {
            return info;
        }

        Glib::ustring result;

        size_t chunk = 0;
        for (const auto& ch : info) {
            if (std::isspace(ch) != 0 and chunk >= size) {
                result += "\n";
                chunk = 0;
            } else {
                result += ch;
            }
            chunk++;
        }

        return result;
    };

    if (current_broadcast_menu_entry == nullptr) {
        current_broadcast_menu_entry = Gtk::manage(new Gtk::MenuItem(split(info, 30)));
        menu->prepend(*current_broadcast_menu_entry);
    } else {
        current_broadcast_menu_entry->set_label(split(info, 30));
    }

    current_broadcast_menu_entry->set_sensitive(false);
}

void
RadioTrayLite::on_station_changed_signal(Glib::ustring station, StationState state)
{
    if (state == em->state) {
        return;
    }

    config->set_last_played_station(station);

    auto turn_on = (state == StationState::PLAYING ? false : true);
    set_current_station(turn_on);

    if (state == StationState::IDLE) {
        set_current_broadcast();
    }

    if (state == StationState::PLAYING) {
        app_indicator_set_icon(indicator, kAppIndicatorIconOn);
        set_current_broadcast(station);
    } else {
        app_indicator_set_icon(indicator, kAppIndicatorIconOff);
    }
}

void
RadioTrayLite::on_broadcast_info_changed_signal(Glib::ustring /*station*/, Glib::ustring info)
{
    set_current_broadcast(info);

    LOG(DEBUG) << info;
}

void
RadioTrayLite::copy_default_bookmarks(std::string src_file)
{
    auto home = getenv("HOME");
    if (home == nullptr) {
        return;
    }

    auto copy_file = [](const std::string& dst_dir, const std::string& src_file) {
        auto dst_file = dst_dir + kBookmarksFileName;

        std::ifstream src(src_file, std::ios::binary);
        std::ofstream dst(dst_file, std::ios::binary);

        dst << src.rdbuf();
    };

    std::string path = home;
    path.append("/.config/").append(kAppDirName).append("/");

    if (not dir_exists(path)) {
        auto rc = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if (rc == 0) {
            copy_file(path, src_file);
        } else {
            LOG(WARNING) << "Couldn't create '" << path << "': " << strerror(errno);
        }
    } else {
        copy_file(path, src_file);
    }
}

bool
RadioTrayLite::file_exists(const std::string& dir, const std::string& file)
{
    if (dir.empty() or file.empty()) {
        return false;
    }

    std::string full_name = dir + file;

    struct stat st = {};
    auto rc = stat(full_name.c_str(), &st);
    if (rc == 0 and S_ISREG(st.st_mode)) {
        return true;
    }

    return false;
};

bool
RadioTrayLite::dir_exists(const std::string& dir)
{
    if (dir.empty()) {
        return false;
    }

    struct stat st = {};
    auto rc = stat(dir.c_str(), &st);
    if (rc == 0 and S_ISDIR(st.st_mode)) {
        return true;
    }

    return false;
};

} // namespace radiotray
