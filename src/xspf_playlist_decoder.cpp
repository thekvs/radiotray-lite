#include "xspf_playlist_decoder.hpp"

namespace radiotray
{

bool
XspfPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if (content_type.find("application/xspf+xml") != std::string::npos) {
        result = true;
    }

    return result;
}

MediaStreams
XspfPlaylistDecoder::extract_media_streams(const std::string& data)
{
    MediaStreams streams;

    pugi::xml_parse_result parsed = playlist_doc.load_buffer(data.c_str(), data.size());
    if (parsed) {
        try {
            pugi::xpath_node_set nodes = playlist_doc.select_nodes("//track/location");
            for (auto& node : nodes) {
                if (not node.node().text().empty()) {
                    streams.push_back(node.node().text().as_string());
                }
            }
        } catch (pugi::xpath_exception& exc) {
            LOG(ERROR) << "Parsing XSPF playlist failed: " << exc.what();
        }
    } else {
        LOG(ERROR) << "Parsing XSPF playlist failed: " << parsed.description();
    }

    return streams;
}

std::string
XspfPlaylistDecoder::desc() const
{
    return std::string("XSPF playlist decoder");
}

} // namespace radiotray

