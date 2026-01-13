#include "monitor.h"
#include "quoteClient.h"
#include "emailClient.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

void run_monitor_test(const std::string& ticker, double sell_threshold, double buy_threshold, const Config& cfg) {
    // Fake prices just to prove the logic works
    std::vector<double> prices = {
        buy_threshold + 0.03,  // Between
        sell_threshold + 0.01, // Cross above: Alert SELL
        sell_threshold + 0.05, // Still above (no spam)
        sell_threshold - 0.01, // Between
        buy_threshold - 0.01,  // Cross below: Alert BUY
        buy_threshold - 0.02,  // Still below (no spam)
        buy_threshold + 0.01,  // Between
        sell_threshold + 0.02, // Cross above again (Alert or Cooldown)
        buy_threshold - 0.02   // Cross below (Alert or Cooldown)
    };

    // States to do the alert logic
    bool was_above = false;
    bool was_below = false;
    bool above = false;
    bool below = false;

    // Cooldown to avoid spam alert messages
    int sell_cooldown_left = 0;
    int buy_cooldown_left = 0;

    int poll = cfg.monitor.pollIntervalSeconds;
    int cooldown = cfg.monitor.cooldownSeconds;

    std::cout << "\nStarting monitor with simulated prices...\n";

    for (double price : prices) {

        // Decrease cooldown
        if (sell_cooldown_left > 0)
            sell_cooldown_left -= poll;
        if (buy_cooldown_left > 0)
            buy_cooldown_left -= poll;
        if (sell_cooldown_left < 0)
            sell_cooldown_left = 0;
        if (buy_cooldown_left < 0)
            buy_cooldown_left = 0;

        if (price > sell_threshold)
            above = true;
        else
            above = false;

        if (price < buy_threshold)
            below = true;
        else
            below = false;

        std::cout << "[tick] " << ticker << " price=" << price;
        if (above)
            std::cout << " (above sell)\n";
        else if (below)
            std::cout << " (below buy)\n";
        else
            std::cout << " (between)\n";

        // Alert logic
        if (!was_above && above) {
            if (sell_cooldown_left == 0) {
                std::cout << "  ALERT: SELL " << ticker << " (price=" << price << ", sell threshold=" << sell_threshold << ")\n";
                std::string subject = cfg.email.subjectPrefix + " SELL " + ticker;
                std::string body =
                    "Action: SELL\r\n"
                    "Ticker: " + ticker + "\r\n" +
                    "Price: " + std::to_string(price) + "\r\n" +
                    "Sell threshold: " + std::to_string(sell_threshold) + "\r\n";

                try {
                    send_email_smtp(cfg, subject, body);
                    std::cout << "  EMAIL: sent\n";
                } catch (const std::exception& e) {
                    std::cerr << "  EMAIL: error: " << e.what() << "\n";
                }
                sell_cooldown_left = cooldown;
            } else {
                std::cout << "  (SELL alert on cooldown: " << sell_cooldown_left << "s)\n";
            }
        }

        if (!was_below && below) {
            if (buy_cooldown_left == 0) {
                std::cout << "  ALERT: BUY " << ticker << " (price=" << price << ", buy threshold=" << buy_threshold << ")\n";
                std::string subject = cfg.email.subjectPrefix + " BUY " + ticker;
                std::string body =
                    "Action: BUY\r\n"
                    "Ticker: " + ticker + "\r\n" +
                    "Price: " + std::to_string(price) + "\r\n" +
                    "Buy threshold: " + std::to_string(buy_threshold) + "\r\n";

                try {
                    send_email_smtp(cfg, subject, body);
                    std::cout << "  EMAIL: sent\n";
                } catch (const std::exception& e) {
                    std::cerr << "  EMAIL: error: " << e.what() << "\n";
                }
                buy_cooldown_left = cooldown;
            } else {
                std::cout << "  (BUY alert on cooldown: " << buy_cooldown_left << "s)\n";
            }
        }

        was_above = above;
        was_below = below;

        std::this_thread::sleep_for(std::chrono::seconds(poll));
    }

    std::cout << "Monitor test finished.\n";
}

// Same logic but with real prices from the API
void run_monitor_live(const std::string& ticker, double sell_threshold, double buy_threshold, const Config& cfg) {

    // States to do the alert logic
    bool was_above = false;
    bool was_below = false;
    bool above = false;
    bool below = false;

    // Cooldown to avoid spam alert messages
    int sell_cooldown_left = 0;
    int buy_cooldown_left = 0;

    int poll = cfg.monitor.pollIntervalSeconds;
    int cooldown = cfg.monitor.cooldownSeconds;

    std::cout << "\nStarting monitor with live prices...\n";

    while (true) {

        // Decrease cooldown
        if (sell_cooldown_left > 0)
            sell_cooldown_left -= poll;
        if (buy_cooldown_left > 0)
            buy_cooldown_left -= poll;
        if (sell_cooldown_left < 0)
            sell_cooldown_left = 0;
        if (buy_cooldown_left < 0)
            buy_cooldown_left = 0;

        double price = 0.0;

        // Getting price from brapi.dev API
        try {
            price = fetch_price(ticker, cfg);
        } catch (const std::exception& e) {
            std::cerr << "  (quote error: " << e.what() << ")\n";
            std::this_thread::sleep_for(std::chrono::seconds(poll));
            continue;
        }

        if (price > sell_threshold)
            above = true;
        else
            above = false;

        if (price < buy_threshold)
            below = true;
        else
            below = false;

        std::cout << "[tick] " << ticker << " price=" << price;
        if (above)
            std::cout << " (above sell)\n";
        else if (below)
            std::cout << " (below buy)\n";
        else
            std::cout << " (between)\n";

        // Alert logic
        if (!was_above && above) {
            if (sell_cooldown_left == 0) {
                std::cout << "  ALERT: SELL " << ticker << " (price=" << price << ", sell threshold=" << sell_threshold << ")\n";
                std::string subject = cfg.email.subjectPrefix + " SELL " + ticker;
                std::string body =
                    "Action: SELL\r\n"
                    "Ticker: " + ticker + "\r\n" +
                    "Price: " + std::to_string(price) + "\r\n" +
                    "Sell threshold: " + std::to_string(sell_threshold) + "\r\n";

                try {
                    send_email_smtp(cfg, subject, body);
                    std::cout << "  EMAIL: sent\n";
                } catch (const std::exception& e) {
                    std::cerr << "  EMAIL: error: " << e.what() << "\n";
                }
                sell_cooldown_left = cooldown;
            } else {
                std::cout << "  (SELL alert on cooldown: " << sell_cooldown_left << "s)\n";
            }
        }

        if (!was_below && below) {
            if (buy_cooldown_left == 0) {
                std::cout << "  ALERT: BUY " << ticker << " (price=" << price << ", buy threshold=" << buy_threshold << ")\n";
                std::string subject = cfg.email.subjectPrefix + " BUY " + ticker;
                std::string body =
                    "Action: BUY\r\n"
                    "Ticker: " + ticker + "\r\n" +
                    "Price: " + std::to_string(price) + "\r\n" +
                    "Buy threshold: " + std::to_string(buy_threshold) + "\r\n";

                try {
                    send_email_smtp(cfg, subject, body);
                    std::cout << "  EMAIL: sent\n";
                } catch (const std::exception& e) {
                    std::cerr << "  EMAIL: error: " << e.what() << "\n";
                }
                buy_cooldown_left = cooldown;
            } else {
                std::cout << "  (BUY alert on cooldown: " << buy_cooldown_left << "s)\n";
            }
        }

        was_above = above;
        was_below = below;

        std::this_thread::sleep_for(std::chrono::seconds(poll));
    }

    std::cout << "Monitor live finished.\n";
}
