#include "m3u_playlist_decoder.hpp"

namespace playradio
{

bool
M3UPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if (content_type.find("audio/mpegurl") != std::string::npos or content_type.find("audio/x-mpegurl") != std::string::npos) {
        result = true;
    }

    return result;
}

MediaStreams
M3UPlaylistDecoder::extract_media_streams(const std::string& data)
{
    MediaStreams streams;

    std::istringstream iss(data);
    std::string line;

    while (!iss.eof()) {
        line.clear();
        iss >> line;

        trim(line);
        if (!line.empty() and line.front() != '#') {
            streams.push_back(line);
        }
    }

    return streams;
}

std::string
M3UPlaylistDecoder::desc() const
{
    return std::string("M3U playlist decoder");
}

} // namespace playradio
