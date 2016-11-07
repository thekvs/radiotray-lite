#ifndef __TRAY_HPP_INCLUDED__
#define __TRAY_HPP_INCLUDED__

#include <stack>
#include <iostream>
#include <memory>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gtkmm.h>
#include <libappindicator/app-indicator.h>

#include "pugixml/pugixml.hpp"

#include "player.hpp"
#include "about.hpp"
#include "constants.hpp"

namespace radiotray
{

class RadioTrayLite
{
public:
    RadioTrayLite() = delete;
    RadioTrayLite(const RadioTrayLite&) = delete;

    RadioTrayLite(int argc, char** argv);
    ~RadioTrayLite();

    void run();

private:
    Glib::RefPtr<Gtk::Application> app;
    std::shared_ptr<Gtk::Menu> menu;

    Gtk::MenuItem* current_station_menu_entry = nullptr;
    Gtk::MenuItem* current_composition_menu_entry = nullptr;

    AppIndicator* indicator = nullptr;

    std::string bookmarks_file;
    pugi::xml_document bookmarks_doc;

    std::shared_ptr<Player> player;
    std::shared_ptr<EventManager> em;

    int counter = 1; // TODO: for debug, remove.

    class BookmarksWalker : public pugi::xml_tree_walker
    {
    public:
        BookmarksWalker() = delete;
        BookmarksWalker(const BookmarksWalker&) = delete;

        BookmarksWalker(RadioTrayLite& radiotray, Gtk::Menu* menu);

        bool for_each(pugi::xml_node& node) override;

    private:
        RadioTrayLite& radiotray;
        std::stack<Gtk::Menu*> menus;
        int level = 0; // TODO: for debug, remove.
    };

    void on_quit_button();
    void on_about_button();
    void on_station_button(Glib::ustring group_name, Glib::ustring station_name, Glib::ustring station_url);
    void on_reload_button();
    void on_current_station_button();

    void build_menu();
    void rebuild_menu();
    void clear_menu();
    bool parse_bookmarks_file();
    void search_for_bookmarks_file();
    void make_current_station_menu_entry(bool turn_on);

    void on_station_changed_signal(Glib::ustring station, StationState state);
    void on_music_info_changed_signal(Glib::ustring station, Glib::ustring info);

    Glib::ustring mk_name(Glib::ustring base_name); // TODO: for debug, remove.
};

} // namespace radiotray

#endif
