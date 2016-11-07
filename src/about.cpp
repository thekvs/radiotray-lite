#include "about.hpp"

namespace radiotray {

AboutDialog::AboutDialog()
{
    auto icon = std::string(kImagePath) + std::string(kAppIcon);

    this->set_icon_from_file(icon);
    this->set_program_name(kAppName);
    this->set_version(kAppVersion);

    auto logo = Gdk::Pixbuf::create_from_file(icon);
    this->set_logo(logo);

    std::vector<Glib::ustring> authors = { kAuthor };
    this->set_authors(authors);

    this->set_website(kWebSite);
    this->set_website_label("Project's website");

    char copyright[1024];
    memset(copyright, 0, sizeof(copyright));
    snprintf(copyright, sizeof(copyright) - 1, kCopyrightTmpl, kAppName, kCopyrightYear, kAuthor);
    this->set_copyright(copyright);
}

} // namespace radiotray

