#ifndef __CONSTANTS_HPP_INCLUDED__
#define __CONSTANTS_HPP_INCLUDED__

#include <string>

namespace radiotray {

const char* const kAppDirName = "radiotray";
const char* const kBookmarksFileName = "bookmarks.xml";

// Images
const char* const kImagePath = "/usr/share/radiotray/images/";

const char* const kAppIcon = "radiotray.png";
const char* const kAppIconOn = "radiotray_on.png";
const char* const kAppIconOff = "radiotray_off.png";
const char* const kAppIconConnecting = "radiotray_connecting.gif";

const char* const kAppIndicatorIconOn = "radiotray_on";
const char* const kAppIndicatorIconOff = "radiotray_off";
const char* const kAppIndicatorIconConnecting = "radiotray_connecting";

// Application info
const char* const kAppVersion = "0.1.0";
const char* const kAppName = "Radio Tray Lite";
const char* const kWebSite = "https://github.com/thekvs/radiotray-lite/";
const char* const kAuthor = "Konstantin Sorokin <kvs@sigterm.ru>";

} // namespace radiotray

#endif
