#ifndef __CONSTANTS_HPP_INCLUDED__
#define __CONSTANTS_HPP_INCLUDED__

#include <string>
#include <cstdint>

namespace radiotray
{
const long kDefaultHTTPRequestTimeout = 5 * 1000;
const int kDefaultGStreamerBufferSize = 320000;
const int64_t kDefaultBufferDuration = 2;

const char* const kRadioTrayAppDirName = "radiotray";
const char* const kAppDirName = "radiotray-lite";
const char* const kBookmarksFileName = "bookmarks.xml";
const char* const kConfigFileName = "config.xml";

// Images
const char* const kImagePath = INSTALL_PREFIX "/share/radiotray-lite/images/";

const char* const kAppIcon = "radiotray-lite.png";
const char* const kAppIconOn = "radiotray_on.png";
const char* const kAppIconOff = "radiotray_off.png";
const char* const kAppIconConnecting = "radiotray_connecting.gif";

const char* const kAppIndicatorIconOn = "radiotray_on";
const char* const kAppIndicatorIconOff = "radiotray_off";
const char* const kAppIndicatorIconConnecting = "radiotray_connecting";

// Application info
const char* const kAppVersion = APP_VERSION;
const char* const kAppName = "Radio Tray Lite";
const char* const kWebSite = "https://github.com/thekvs/radiotray-lite/";
const char* const kAuthor = "Konstantin Sorokin <kvs@sigterm.ru>";
const char* const kCopyrightYear = "2016-2017";
const char* const kCopyrightTmpl = "%s - Copyright (c) %s\n %s";

} // namespace radiotray

#endif
