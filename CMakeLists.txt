cmake_minimum_required(VERSION 3.16)

project(stock_quote_alert LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(stock-quote-alert
    src/main.cpp
    src/config.cpp
    src/monitor.cpp
)

target_include_directories(stock-quote-alert PRIVATE include third_party)
