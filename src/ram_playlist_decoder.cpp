#include "ram_playlist_decoder.hpp"

namespace radiotray
{

bool
RamPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if (content_type.find("audio/x-pn-realaudio") != std::string::npos
        or content_type.find("audio/vnd.rn-realaudio") != std::string::npos) {
        result = true;
    }

    return result;
}

MediaStreams
RamPlaylistDecoder::extract_media_streams(const std::string& data)
{
    MediaStreams streams;

    std::istringstream iss(data);
    std::string line;

    while (!iss.eof()) {
        line.clear();
        iss >> line;

        trim(line);
        if ((not line.empty()) and line.front() != '#') {
            streams.push_back(line);
        }
    }

    return streams;
}

std::string
RamPlaylistDecoder::desc() const
{
    return std::string("RAM playlist decoder");
}

} // namespace radiotray
