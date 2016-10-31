#include "asx_playlist_decoder.hpp"

namespace playradio
{

AsxPlaylistDecoder::AsxPlaylistDecoder()
{
    xmlInitParser();
    LIBXML_TEST_VERSION;
}

AsxPlaylistDecoder::~AsxPlaylistDecoder()
{
    xmlCleanupParser();
}

bool
AsxPlaylistDecoder::is_valid(const std::string& content_type) const
{
    bool result = false;
    auto npos = std::string::npos;

    if (content_type.find("audio/x-ms-wax") != npos or content_type.find("video/x-ms-wvx") != npos
        or content_type.find("video/x-ms-asf") != npos or content_type.find("video/x-ms-wmv") != npos) {
        result = true;
    }

    return result;
}

MediaStreams
AsxPlaylistDecoder::extract_media_streams(const std::string& data)
{
    MediaStreams streams;

    xmlDocPtr doc;
    xmlXPathContextPtr xpath_ctx;
    xmlXPathObjectPtr xpath_obj;

    const unsigned char* xpath_expr = BAD_CAST("//ref/@href");

    doc = xmlRecoverMemory(data.c_str(), data.size());
    if (doc == nullptr) {
        return streams;
    }

    xpath_ctx = xmlXPathNewContext(doc);
    if (xpath_ctx == nullptr) {
        xmlFreeDoc(doc);
        return streams;
    }

    xpath_obj = xmlXPathEvalExpression(xpath_expr, xpath_ctx);
    if (xpath_obj == nullptr) {
        xmlXPathFreeContext(xpath_ctx);
        xmlFreeDoc(doc);
        return streams;
    }

    auto nodes = xpath_obj->nodesetval;
    auto size = (nodes) ? nodes->nodeNr : 0;

    for (decltype(size) i = 0; i < size; ++i) {
        auto cur = nodes->nodeTab[i];
        if (cur->type == XML_ATTRIBUTE_NODE) {
            std::string s = (char*)xmlNodeGetContent(cur);
            trim(s);
            streams.push_back(s);
        }
    }

    xmlXPathFreeObject(xpath_obj);
    xmlXPathFreeContext(xpath_ctx);
    xmlFreeDoc(doc);

    return streams;
}

std::string
AsxPlaylistDecoder::desc() const
{
    return std::string("ASX playlist decoder");
}

} // namespace playradio
