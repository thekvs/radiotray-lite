#ifndef __PLAYLIST_DECODER_HPP_INCLUDED__
#define __PLAYLIST_DECODER_HPP_INCLUDED__

#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <vector>

#include "easyloggingpp/easylogging++.h"

namespace radiotray
{
using MediaStreams = std::vector<std::string>;

enum class PlaylistDecoderType {
    UNKNOWN_PLAYLIST_DECODER,
    M3U_PLAYLIST_DECODER,
    PLS_PLAYLIST_DECODER,
    RAM_PLAYLIST_DECODER,
    ASX_PLAYLIST_DECODER,
    XSPF_PLAYLIST_DECODER
};

class PlaylistDecoder
{
public:
    virtual ~PlaylistDecoder() = default;

    virtual bool is_valid(const std::string& content_type) const = 0;
    virtual MediaStreams extract_media_streams(const std::string& data) = 0;
    virtual std::string desc() const = 0;

protected:
    void trim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }
};

} // namespace radiotray

#endif
