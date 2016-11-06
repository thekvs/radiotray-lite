#ifndef __ABOUT_HPP_INCLUDED__
#define __ABOUT_HPP_INCLUDED__

#include <gtkmm.h>
#include "constants.hpp"

namespace radiotray {

class AboutDialog : public Gtk::AboutDialog
{
public:
    AboutDialog();
};

} // namespace radiotray

#endif
