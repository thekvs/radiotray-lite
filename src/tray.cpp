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
        auto station_url = attr_url.as_string();
        auto sub_item = Gtk::manage(new Gtk::MenuItem(station_name));
        sub_item->signal_activate().connect(sigc::bind<Glib::ustring, Glib::ustring, Glib::ustring>(
            sigc::mem_fun(radiotray, &RadioTrayLite::on_station_button), station_group_name, station_name, station_url));
        menus.top()->append(*sub_item);
        LOG(DEBUG) << "Bookmark depth: " << depth() << ", level: " << level << ", #menus: " << menus.size() << ", station: " << station_name
                   << ", group: " << station_group_name;
    }

    return true; // continue traversal
}

RadioTrayLite::RadioTrayLite(int argc, char** argv)
{
    config = std::make_shared<Config>();

    app = Gtk::Application::create(argc, argv, "github.com.thekvs.radiotray-lite");
    menu = std::make_shared<Gtk::Menu>();

    player = std::make_shared<Player>();
    auto ok = player->init(argc, argv);
    if (!ok) {
        // TODO
    }

    notifier = std::make_shared<Notification>(kAppName);
    ok = notifier->init();
    if (!ok) {
        // TODO
    }

    em = std::make_shared<EventManager>();
    em->state_changed.connect(sigc::mem_fun(*this, &RadioTrayLite::on_station_changed_signal));
    em->broadcast_info_changed.connect(sigc::mem_fun(*this, &RadioTrayLite::on_broadcast_info_changed_signal));
    em->broadcast_info_changed.connect(sigc::mem_fun(*notifier, &Notification::on_broadcast_info_changed_signal));

    player->em = em;

    indicator = app_indicator_new_with_path("Radio Tray Lite", kAppIndicatorIconOff, APP_INDICATOR_CATEGORY_APPLICATION_STATUS, kImagePath);
    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon(indicator, kAppIndicatorIconOn);
    app_indicator_set_menu(indicator, menu->gobj());

    search_for_bookmarks_file();
}

RadioTrayLite::~RadioTrayLite()
{
    clear_menu();

    if (indicator != nullptr) {
        g_object_unref(G_OBJECT(indicator));
    }
}

void
RadioTrayLite::run()
{
    build_menu();

    // FIXME: WHY THIS DOESN'T RUN IN A LOOP?
    // app->run();

    gtk_main();
}

void
RadioTrayLite::on_quit_button()
{
    LOG(DEBUG) << "'Quit' button was pressed.";

    player->stop();
    gtk_main_quit();
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

    auto turn_on = (em->state == StationState::PLAYING ? false : true);
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
    for (auto& e : menu->get_children()) {
        menu->remove(*e);
        delete e;
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
RadioTrayLite::search_for_bookmarks_file()
{
    std::vector<std::string> paths;

    auto home = getenv("HOME");
    if (home != nullptr) {
        auto dir = std::string(home) + "/.config/" + kAppDirName + "/";
        paths.push_back(dir);
        dir = std::string(home) + "/.local/share/" + kRadioTrayAppDirName + "/";
        paths.push_back(dir);
    }

    // default bookmarks file
    auto base_dir = INSTALL_PREFIX"/share/";
    paths.push_back(std::string(base_dir) + kAppDirName + "/");

    auto file_exists = [](const std::string& dir, const std::string& file) -> bool {
        std::string full_name = dir + file;
        struct stat st;
        auto rc = stat(full_name.c_str(), &st);
        if (rc == 0 and S_ISREG(st.st_mode)) {
            return true;
        }
        return false;
    };

    auto bookmarks_file_exists = std::bind(file_exists, std::placeholders::_1, kBookmarksFileName);
    auto result = std::find_if(std::begin(paths), std::end(paths), bookmarks_file_exists);
    if (result != std::end(paths)) {
        auto dir = *result;
        bookmarks_file = dir + kBookmarksFileName;

        // This is the last search path i.e. bookmarks.xml which comes with distribution
        if (dir == paths.back()) {
            copy_default_bookmarks(bookmarks_file);
        }

        config->set_config_file(dir + kConfigFileName);

        if (file_exists(dir, kConfigFileName)) {
            config->load_config();
        }
    }
}

void
RadioTrayLite::set_current_station(bool turn_on)
{
    if ((not player->has_station()) and config->has_last_station()) {
        try {
            char xpath_query[1024];
            memset(xpath_query, 0, sizeof(xpath_query));
            snprintf(xpath_query, sizeof(xpath_query) - 1, "//bookmark[@name='%s']", config->last_station.c_str());

            pugi::xpath_node node = bookmarks_doc.select_node(xpath_query);
            if (not node.node().empty()) {
                auto data_url = node.node().attribute("url").as_string();
                player->init_streams(data_url, config->last_station);
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

        std::string original = info;
        std::string result;

        size_t chunk = 0;
        for (auto& ch : original) {
            if (std::isspace(ch) and chunk >= size) {
                result += "\n";
                chunk = 0;
            } else {
                result += ch;
            }
            chunk++;
        }

        return Glib::ustring(result);
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

    config->last_station = station;

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

    if (home != nullptr) {
        std::string path = home;
        path.append("/.config/").append(kAppDirName).append("/");

        auto rc = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if (rc == 0) {
            auto dst_file = path + kBookmarksFileName;

            std::ifstream src(src_file, std::ios::binary);
            std::ofstream dst(dst_file, std::ios::binary);

            dst << src.rdbuf();
        } else {
            LOG(WARNING) << "Couldn't create '" << path << "': " << strerror(errno);
        }
    }
}

} // namespace radiotray
