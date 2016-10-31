#ifndef __PLAYLIST_DECODER_HPP_INCLUDED__
#define __PLAYLIST_DECODER_HPP_INCLUDED__

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>
#include <functional>
#include <cctype>
#include <locale>

#include "easyloggingpp/easylogging++.h"

namespace playradio
{
using MediaStreams = std::vector<std::string>;

class PlaylistDecoder
{
public:
    virtual ~PlaylistDecoder() = default;

    virtual bool is_valid(const std::string& content_type) const = 0;
    virtual MediaStreams extract_media_streams(const std::string& data) = 0;
    virtual std::string desc() const = 0;

protected:
    void
    trim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }
};

} // namespace playradio

#endif
