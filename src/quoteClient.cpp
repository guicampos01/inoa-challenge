#include "quoteClient.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <stdexcept>
#include <string>

using nlohmann::json;

static size_t write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t total = size * nmemb;
    std::string* out = static_cast<std::string*>(userp);
    out->append(static_cast<char*>(contents), total);
    return total;
}

double fetch_price(const std::string& ticker, const Config& cfg) {
    std::string base = cfg.quote.baseUrl.empty() ? "https://brapi.dev/api" : cfg.quote.baseUrl;
    if (!base.empty() && base.back() == '/') base.pop_back();

    std::string url = base + "/quote/" + ticker;
    if (!cfg.quote.token.empty()) {
        url += "?token=" + cfg.quote.token;
    }

    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("curl init failed");

    std::string body;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("curl error: ") + curl_easy_strerror(res));
    }
    if (http_code != 200) {
        throw std::runtime_error("HTTP " + std::to_string(http_code) + " body: " + body);
    }

    json j = json::parse(body);

    return j["results"][0]["regularMarketPrice"].get<double>();
}
