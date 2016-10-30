#ifndef __TRAY_HPP_INCLUDED__
#define __TRAY_HPP_INCLUDED__

#include <stack>
#include <iostream>
#include <memory>

#include <gtkmm.h>
#include <libappindicator/app-indicator.h>

#include "pugixml/pugixml.hpp"

namespace radiotray {

class RadioTrayLite
{
public:

    RadioTrayLite() = delete;
    RadioTrayLite(const RadioTrayLite&) = delete;

    RadioTrayLite(int argc, char** argv);

    void run();

private:
    Glib::RefPtr<Gtk::Application> app;
    std::shared_ptr<Gtk::Menu> menu;
    AppIndicator* indicator = nullptr;
    std::string bookmarks_file;
    pugi::xml_document bookmarks_doc;

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
    void on_station_button(Glib::ustring group_name, Glib::ustring station_url);
    void on_reload_button();
    void build_menu();
    void rebuild_menu();
    void clear_menu();
    bool parse_bookmarks_file();

    Glib::ustring mk_name(Glib::ustring base_name); // TODO: for debug, remove.
};

} // namespace radiotray

#endif
