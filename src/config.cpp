#include "config.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using nlohmann::json;

static std::string read_file_to_string(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open config file: " + path);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

Config load_config(const std::string& path) {
    const std::string content = read_file_to_string(path);

    json j;
    try {
        j = json::parse(content);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Invalid JSON in config file: ") + e.what());
    }

    Config cfg;

    // email
    cfg.email.to = j["email"].value("to", "");
    cfg.email.from = j["email"].value("from", "");
    cfg.email.subjectPrefix = j["email"].value("subjectPrefix", "[StockAlert]");

    // smtp
    cfg.smtp.host = j["smtp"].value("host", "");
    cfg.smtp.port = j["smtp"].value("port", 0);
    cfg.smtp.username = j["smtp"].value("username", "");
    cfg.smtp.password = j["smtp"].value("password", "");
    cfg.smtp.useTLS = j["smtp"].value("useTLS", true);

    // monitor
    cfg.monitor.mode = j["monitor"].value("mode", "");
    cfg.monitor.pollIntervalSeconds = j["monitor"].value("pollIntervalSeconds", 180);
    cfg.monitor.cooldownSeconds = j["monitor"].value("cooldownSeconds", 300);

    // quote
    cfg.quote.provider = j["quote"].value("provider", "brapi");
    cfg.quote.token = j["quote"].value("token", "");
    cfg.quote.baseUrl = j["quote"].value("baseUrl", "https://brapi.dev/api");

    return cfg;
}
