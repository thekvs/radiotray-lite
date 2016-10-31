#ifndef __PLS_PLAYLIST_DECODER_HPP_INCLUDED__
#define __PLS_PLAYLIST_DECODER_HPP_INCLUDED__

#include <sstream>

#include "playlist_decoder.hpp"

namespace playradio
{
class PlsPlaylistDecoder : public PlaylistDecoder
{
public:
    ~PlsPlaylistDecoder() = default;

    bool is_valid(const std::string& content_type) const override;
    MediaStreams extract_media_streams(const std::string& data) override;
    std::string desc() const override;

private:
};

} // namespace playradio

#endif
