#ifndef __PLAYLIST_HPP_INCLUDED__
#define __PLAYLIST_HPP_INCLUDED__

#include <vector>
#include <string>
#include <tuple>
#include <algorithm>

#include <curl/curl.h>

#include "constants.hpp"
#include "config.hpp"
#include "m3u_playlist_decoder.hpp"
#include "pls_playlist_decoder.hpp"
#include "asx_playlist_decoder.hpp"
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

    std::tuple<bool, MediaStreams> get_streams(std::string url);
    void set_config(std::shared_ptr<Config> cfg);

private:
    CURL* handle = nullptr;
    char errbuffer[CURL_ERROR_SIZE];

    std::shared_ptr<Config> config;

    bool abort_get_request = false;
    std::string data;

    std::vector<std::shared_ptr<PlaylistDecoder>> decoders;

    void prepare_playlist_request(std::string url, bool only_headers);
    long get_http_status();
    std::string get_content_type();
    MediaStreams run_playlist_decoders(std::string url);

    static size_t write_memory_cb(void* ptr, size_t size, size_t nmemb, void* data);
};
} // namespace radiotray

#endif
