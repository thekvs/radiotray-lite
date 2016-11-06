#include "about.hpp"

namespace radiotray {

AboutDialog::AboutDialog()
{
    this->set_icon_from_file(std::string(kImagePath) + std::string(kAppIcon));
    this->set_program_name(kAppName);
    this->set_version(kAppVersion);

    auto logo = Gdk::Pixbuf::create_from_file(std::string(kImagePath) + std::string(kAppIcon));
    this->set_logo(logo);

    std::vector<Glib::ustring> authors = { kAuthor };
    this->set_authors(authors);

    this->set_website(kWebSite);
    this->set_website_label("Project's website");
}

} // namespace radiotray

