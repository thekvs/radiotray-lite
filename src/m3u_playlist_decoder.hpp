#ifndef __M3U_PLAYLIST_DECODER_HPP_INCLUDED__
#define __M3U_PLAYLIST_DECODER_HPP_INCLUDED__

#include <iomanip>
#include <regex>
#include <sstream>

#include "playlist_decoder.hpp"

namespace radiotray
{
class M3UPlaylistDecoder : public PlaylistDecoder
{
public:
    ~M3UPlaylistDecoder() = default;

    bool is_valid(const std::string& content_type) const override;
    MediaStreams extract_media_streams(const std::string& data) override;
    std::string desc() const override;

private:
};

} // namespace radiotray

#endif
