#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "config.h"

// Example for when the user misses the parameters
static void print_example(const char* program_name) {
    std::cout
        << "Usage:\n"
        << "  " << program_name << " <TICKER> <SELL_PRICE> <BUY_PRICE>\n\n"
        << "Example:\n"
        << "  " << program_name << " PETR4 22.67 22.59\n";
}

// Makes sure the SELL_PRICE and BUY_PRICE are double, to print the correct error if they aren't
static bool parse_double(const std::string& s, double& out) {
    char* end = nullptr;
    const double val = std::strtod(s.c_str(), &end);
    if (end == s.c_str() || *end != '\0') return false;
    out = val;
    return true;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        print_example(argv[0]);
        return 1;
    }

    const std::string ticker = argv[1];

    double sell_price = 0.0;
    double buy_price = 0.0;

    if (ticker.empty()) {
        std::cerr << "Error: TICKER cannot be empty.\n";
        return 1;
    }
    if (!parse_double(argv[2], sell_price)) {
        std::cerr << "Error: SELL_PRICE must be a valid number.\n";
        return 1;
    }
    if (!parse_double(argv[3], buy_price)) {
        std::cerr << "Error: BUY_PRICE must be a valid number.\n";
        return 1;
    }
    if (sell_price <= buy_price) {
        std::cerr << "Error: SELL_PRICE must be greater than BUY_PRICE.\n";
        return 1;
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "CLI arguments validated:\n";
    std::cout << "  Ticker: " << ticker << "\n";
    std::cout << "  Sell threshold: " << sell_price << "\n";
    std::cout << "  Buy threshold:  " << buy_price << "\n";

    // Load configuration
    try {
        const Config cfg = load_config("config/config.json");

        std::cout << "\nConfig loaded successfully:\n";
        std::cout << "  email.to: " << cfg.email.to << "\n";
        std::cout << "  email.from: " << cfg.email.from << "\n";
        std::cout << "  smtp.host: " << cfg.smtp.host << "\n";
        std::cout << "  smtp.port: " << cfg.smtp.port << "\n";
        std::cout << "  monitor.pollIntervalSeconds: " << cfg.monitor.pollIntervalSeconds << "\n";
        std::cout << "  monitor.cooldownSeconds: " << cfg.monitor.cooldownSeconds << "\n";
        std::cout << "  quote.provider: " << (cfg.quote.provider.empty() ? "brapi" : cfg.quote.provider) << "\n";
        std::cout << "  quote.baseUrl: " << (cfg.quote.baseUrl.empty() ? "https://brapi.dev/api" : cfg.quote.baseUrl) << "\n";

    } catch (const std::exception& e) {
        std::cerr << "\nConfig error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
