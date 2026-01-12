#include "monitor.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

void run_monitor(const std::string& ticker, double sell_threshold, double buy_threshold, const Config& cfg) {
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
        else if 
            (below) std::cout << " (below buy)\n";
        else 
            std::cout << " (between)\n";

        // Alert logic
        if (!was_above && above) {
            if (sell_cooldown_left == 0) {
                std::cout << "  ALERT: SELL " << ticker << " (price=" << price << ", sell threshold=" << sell_threshold << ")\n";
                sell_cooldown_left = cooldown;
            } else {
                std::cout << "  (SELL alert on cooldown: " << sell_cooldown_left << "s)\n";
            }
        }

        if (!was_below && below) {
            if (buy_cooldown_left == 0) {
                std::cout << "  ALERT: BUY " << ticker << " (price=" << price << ", buy threshold=" << buy_threshold << ")\n";
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
