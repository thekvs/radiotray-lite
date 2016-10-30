#include "tray.hpp"
#include "constants.hpp"

namespace radiotray {

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
        std::cout << "Group: " << group_name << ", depth: " << depth() << std::endl;
    } else if (is_bookmark and (!attr_url.empty())) {
        auto station_name = attr_name.as_string();
        auto station_group_name = node.parent().attribute("name").as_string();
        auto station_url = attr_url.as_string();
        auto sub_item = Gtk::manage(new Gtk::MenuItem(station_name));
        sub_item->signal_activate().connect(sigc::bind<Glib::ustring, Glib::ustring>(
                sigc::mem_fun(radiotray, &RadioTrayLite::on_station_button), station_group_name, station_url));
        menus.top()->append(*sub_item);
        std::cout << "Bookmark depth: " << depth() << ", level: " << level << ", #menus: " << menus.size() <<  ", station: " << station_name << ", group: " << station_group_name << std::endl;
    }

    std::cout.flush();

    return true; // continue traversal
}

RadioTrayLite::RadioTrayLite(int argc, char** argv)
{
    app = Gtk::Application::create(argc, argv, "github.com.thekvs.radiotray-lite");
    menu = std::make_shared<Gtk::Menu>();
    indicator = app_indicator_new_with_path("Radio Tray Lite", "radiotray_off",
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS, kImagePath);

    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon(indicator, "radiotray_on");
    app_indicator_set_menu(indicator, menu->gobj());

    if (argc > 1) {
        bookmarks_file = argv[1];
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
    std::cout << "'Quit' button was pressed." << std::endl;
    gtk_main_quit();
}

void
RadioTrayLite::on_about_button()
{
    std::cout << "'About' button was pressed." << std::endl;
}

void
RadioTrayLite::on_station_button(Glib::ustring group_name, Glib::ustring station_url)
{
    std::cout << "'" << station_url << "'" << "(group: " << group_name << ")" << " button was pressed." << std::endl;
}

void
RadioTrayLite::on_reload_button()
{
    std::cout << "'Reload'" << " button was pressed" << std::endl;
    rebuild_menu();
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

    auto menu_item = Gtk::manage(new Gtk::MenuItem()); // separator
    menu->append(*menu_item);

    name = mk_name("Reload Bookmarks");
    menu_item = Gtk::manage(new Gtk::MenuItem(name));
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

    menu->show_all();
}

void
RadioTrayLite::rebuild_menu()
{
    counter++;

    clear_menu();
    build_menu();
}

void
RadioTrayLite::clear_menu()
{
    for (auto &e : menu->get_children()) {
        menu->remove(*e);
    }
}

bool
RadioTrayLite::parse_bookmarks_file()
{
    bool status = false;

    if (not bookmarks_file.empty()) {
        pugi::xml_parse_result result = bookmarks_doc.load_file(bookmarks_file.c_str());
        if (!result) {
            std::cerr << "XML parser failed: " << result.description() << std::endl;
        } else {
            status = true;
        }
    } else {
        std::cerr << "Bookmarks file not specified!" << std::endl;
    }

    return status;
}

Glib::ustring
RadioTrayLite::mk_name(Glib::ustring base_name)
{
    Glib::ustring result;
    char c[64];

    snprintf(c, sizeof(c), "%i", counter);
    result = base_name + " " + c;

    return result;
}
} // namespace radiotray
