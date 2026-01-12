#ifndef QUOTECLIENT_H
#define QUOTECLIENT_H

#include "config.h"
#include <string>

double fetch_price(const std::string& ticker, const Config& cfg);

#endif // QUOTECLIENT_H
