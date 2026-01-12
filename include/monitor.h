#ifndef MONITOR_H
#define MONITOR_H

#include "config.h"
#include <string>

void run_monitor_test(const std::string& ticker, double sell_threshold, double buy_threshold, const Config& cfg);
void run_monitor_live(const std::string& ticker, double sell_threshold, double buy_threshold, const Config& cfg);

#endif // MONITOR_H
