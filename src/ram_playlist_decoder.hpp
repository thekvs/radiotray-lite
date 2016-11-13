#ifndef __RAM_PLAYLIST_DECODER_HPP_INCLUDED__
#define __RAM_PLAYLIST_DECODER_HPP_INCLUDED__

#include <sstream>
#include <iomanip>

#include "playlist_decoder.hpp"

namespace radiotray
{
class RamPlaylistDecoder : public PlaylistDecoder
{
public:
    ~RamPlaylistDecoder() = default;

    bool is_valid(const std::string& content_type) const override;
    MediaStreams extract_media_streams(const std::string& data) override;
    std::string desc() const override;

private:
};

} // namespace radiotray

#endif
