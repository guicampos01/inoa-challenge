#ifndef EMAILCLIENT_H
#define EMAILCLIENT_H

#include "config.h"
#include <string>

void send_email_smtp(const Config& cfg, const std::string& subject, const std::string& body);

#endif // EMAILCLIENT_H
