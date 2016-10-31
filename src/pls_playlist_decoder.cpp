#include "pls_playlist_decoder.hpp"

namespace radiotray
{

bool
PlsPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;

    if (content_type.find("audio/x-scpls") != std::string::npos or content_type.find("application/pls+xml") != std::string::npos) {
        result = true;
    }

    return result;
}

MediaStreams
PlsPlaylistDecoder::extract_media_streams(const std::string& data)
{
    MediaStreams streams;

    std::istringstream iss(data);
    std::string line;

    while (!iss.eof()) {
        line.clear();
        iss >> line;

        trim(line);

        if (!line.empty() and line.front() != '#') {
            auto eq_sign = std::find(std::begin(line), std::end(line), '=');
            if (eq_sign != std::end(line)) {
                auto eq_sign_pos = std::distance(std::begin(line), eq_sign);
                auto k = line.substr(0, eq_sign_pos);
                auto v = line.substr(eq_sign_pos + 1, line.size() - eq_sign_pos - 1);
                if (strncasecmp("file", k.c_str(), 4) == 0) {
                    streams.push_back(v);
                }
            }
        }
    }

    return streams;
}

std::string
PlsPlaylistDecoder::desc() const
{
    return std::string("PLS playlist decoder");
}

} // namespace radiotray
