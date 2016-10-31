#include "playlist.hpp"

namespace playradio
{

static const char* kUserAgent = "PlayRadio";
static const bool kOnlyHeaders = true;
static const long kHTTPRequestTimeout = 5 * 1000;

Playlist::Playlist()
{
    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errbuffer);

    decoders.push_back(std::make_shared<M3UPlaylistDecoder>());
    decoders.push_back(std::make_shared<PlsPlaylistDecoder>());
    decoders.push_back(std::make_shared<AsxPlaylistDecoder>());
}

Playlist::~Playlist()
{
    curl_easy_cleanup(handle);
}

std::tuple<bool, MediaStreams>
Playlist::get_streams(std::string url)
{
    bool status = false;
    MediaStreams streams;

    prepare_playlist_request(url, kOnlyHeaders);
    auto rc = curl_easy_perform(handle);
    auto http_status_code = get_http_status();

    if (rc == CURLE_OK and http_status_code == 200) {
        streams = run_playlist_decoders(url);
        status = true;
    } else {
        // Some streaming services don't allow HEAD request so we have
        // to do GET request and receive some small amount of data.
        LOG(WARNING) << "HEAD request failed!";

        abort_get_request = true;
        prepare_playlist_request(url, not kOnlyHeaders);
        rc = curl_easy_perform(handle);
        abort_get_request = false;

        if (rc == CURLE_OK or rc == CURLE_WRITE_ERROR /* it's ok, we've aborted reading */) {
            streams = run_playlist_decoders(url);
            status = true;
        }
    }

    return std::make_tuple(status, streams);
}

void
Playlist::prepare_playlist_request(std::string url, bool only_headers)
{
    data.clear();
    curl_easy_reset(handle);

    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, kHTTPRequestTimeout);

    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 7);

    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_memory_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, static_cast<void*>(this));

    curl_easy_setopt(handle, CURLOPT_USERAGENT, kUserAgent);

    curl_easy_setopt(handle, CURLOPT_ENCODING, "gzip, deflate");

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    if (only_headers) {
        curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
    }
}

long
Playlist::get_http_status()
{
    long status;

    auto rc = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &status);
    if (rc != 0) {
        status = 0;
    }

    return status;
}

std::string
Playlist::get_content_type()
{
    char* content_type_data;
    std::string content_type;

    auto rc = curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &content_type_data);
    if (rc == CURLE_OK && content_type_data != NULL) {
        content_type = content_type_data;
        for (auto& v : content_type) {
            v = std::tolower(v);
        }
    }

    return content_type;
}

MediaStreams
Playlist::run_playlist_decoders(std::string url)
{
    MediaStreams streams;
    bool extracted = false;

    auto content_type = get_content_type();

    if (!content_type.empty()) {
        LOG(DEBUG) << "Content-Type: " << content_type;
        for (const auto& decoder : decoders) {
            if (decoder->is_valid(content_type)) {
                LOG(DEBUG) << "Matched " << decoder->desc();
                prepare_playlist_request(url, not kOnlyHeaders);
                auto rc = curl_easy_perform(handle);
                if (rc == CURLE_OK) {
                    LOG(DEBUG) << "Playlist: " << data;
                    streams = decoder->extract_media_streams(data);
                    for (auto& s : streams) {
                        LOG(DEBUG) << "Stream: " << s;
                    }
                    extracted = true;
                    break;
                }
            }
        }
    }

    // No decoder found, consider url a media stream
    if (streams.empty() && !extracted) {
        streams.push_back(url);
    }

    return streams;
}

size_t
Playlist::write_memory_cb(void* ptr, size_t size, size_t nmemb, void* data)
{
    size_t realsize = 0;
    Playlist* instance = static_cast<Playlist*>(data);

    if (instance) {
        realsize = size * nmemb;
        instance->data.append(static_cast<char*>(ptr), realsize);
        if (instance->abort_get_request) {
            realsize = 0;
        }
    }

    return realsize;
}

} // namespace playradio
