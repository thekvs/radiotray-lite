#ifndef __PLAYLIST_HPP_INCLUDED__
#define __PLAYLIST_HPP_INCLUDED__

#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <curl/curl.h>
#include <magic.h>

#include "config.hpp"
#include "constants.hpp"
#include "asx_playlist_decoder.hpp"
#include "m3u_playlist_decoder.hpp"
#include "pls_playlist_decoder.hpp"
#include "ram_playlist_decoder.hpp"
#include "xspf_playlist_decoder.hpp"

namespace radiotray
{

class Playlist
{
public:
    Playlist();
    Playlist(const Playlist&) = delete;

    ~Playlist();

    bool init();
    std::tuple<bool, MediaStreams> get_streams(std::string url);
    void set_config(std::shared_ptr<Config> cfg);

private:
    CURL* handle = nullptr;
    char errbuffer[CURL_ERROR_SIZE];

    magic_t mcookie = nullptr;

    std::shared_ptr<Config> config;

    bool abort_get_request = false;
    std::string data;

    std::map<PlaylistDecoderType, std::shared_ptr<PlaylistDecoder>> decoders;

    void prepare_playlist_request(std::string url, bool only_headers);
    long get_http_status();
    std::string get_content_type();
    MediaStreams run_playlist_decoders(std::string url);
    bool has_prefix(const std::string& prefix, const std::string& str);
    PlaylistDecoderType guess_playlist_decoder_type();

    static size_t write_memory_cb(void* ptr, size_t size, size_t nmemb, void* data);
};
} // namespace radiotray

#endif
