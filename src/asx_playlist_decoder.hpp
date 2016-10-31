#ifndef __ASX_PLAYLIST_DECODER_HPP_INCLUDED__
#define __ASX_PLAYLIST_DECODER_HPP_INCLUDED__

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "playlist_decoder.hpp"

namespace radiotray
{
class AsxPlaylistDecoder : public PlaylistDecoder
{
public:
    AsxPlaylistDecoder();
    ~AsxPlaylistDecoder() override;

    bool is_valid(const std::string& content_type) const override;
    MediaStreams extract_media_streams(const std::string& data) override;
    std::string desc() const override;

private:
};

} // namespace radiotray

#endif
