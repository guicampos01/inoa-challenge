#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct EmailConfig {
    std::string to;
    std::string from;
    std::string subjectPrefix;
};

struct SmtpConfig {
    std::string host;
    int port = 0;
    std::string username;
    std::string password;
    bool useTLS = true;
};

struct MonitorConfig {
    std::string mode = "live"; // test or live
    int pollIntervalSeconds = 180;
    int cooldownSeconds = 300;
};

struct QuoteConfig {
    std::string provider; // "brapi"
    std::string token;    
    std::string baseUrl;  // "https://brapi.dev/api"
};

struct Config {
    EmailConfig email;
    SmtpConfig smtp;
    MonitorConfig monitor;
    QuoteConfig quote;
};

// Loads and validates configuration from a JSON file.
Config load_config(const std::string& path);

#endif // CONFIG_H
