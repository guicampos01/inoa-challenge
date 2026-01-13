#include "emailClient.h"

#include <curl/curl.h>

#include <cstring>    
#include <stdexcept>
#include <string>

// Besides the curl documentation, AI was also used to help understand and implement these functions

struct upload_status {
    size_t bytes_read;
    const std::string* payload;
};

static size_t payload_source(char* ptr, size_t size, size_t nmemb, void* userp) {
    upload_status* upload = static_cast<upload_status*>(userp);
    size_t room = size * nmemb;

    if (room == 0) return 0;

    size_t left = upload->payload->size() - upload->bytes_read;
    if (left == 0) return 0;

    size_t to_copy = (left < room) ? left : room;
    memcpy(ptr, upload->payload->data() + upload->bytes_read, to_copy);
    upload->bytes_read += to_copy;

    return to_copy;
}

void send_email_smtp(const Config& cfg, const std::string& subject, const std::string& body) {
    if (cfg.smtp.host.empty()) throw std::runtime_error("smtp.host is empty");
    if (cfg.smtp.port <= 0) throw std::runtime_error("smtp.port is invalid");
    if (cfg.email.from.empty()) throw std::runtime_error("email.from is empty");
    if (cfg.email.to.empty()) throw std::runtime_error("email.to is empty");

    // Build SMTP URL like: smtp://localhost:1025
    std::string url = "smtp://" + cfg.smtp.host + ":" + std::to_string(cfg.smtp.port);

    // SMTP envelope addresses usually use <addr@domain>
    auto wrap_env = [](const std::string& addr) -> std::string {
        if (!addr.empty() && addr.front() == '<' && addr.back() == '>') return addr;
        return "<" + addr + ">";
    };
    std::string mail_from = wrap_env(cfg.email.from);
    std::string rcpt_to   = wrap_env(cfg.email.to);

    // Build payload
    std::string payload;
    payload += "To: " + cfg.email.to + "\r\n";
    payload += "From: " + cfg.email.from + "\r\n";
    payload += "Subject: " + subject + "\r\n";
    payload += "\r\n";
    payload += body;
    payload += "\r\n";

    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        throw std::runtime_error("curl_global_init failed");
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        curl_global_cleanup();
        throw std::runtime_error("curl_easy_init failed");
    }

    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, rcpt_to.c_str());

    upload_status upload;
    upload.bytes_read = 0;
    upload.payload = &payload;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mail_from.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    // Upload payload
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    res = curl_easy_perform(curl);

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("curl_easy_perform failed: ") + curl_easy_strerror(res));
    }
}
