# Stock Quote Alert (B3)

A simple console application that monitors a B3 stock quote and sends an email alert when the price goes **above a sell threshold** or **below a buy threshold**.

This project was built as a technical challenge.

---

## Features

- Console application
- Called via command line with 3 parameters:
  - **ticker**
  - **sell threshold**
  - **buy threshold**
- Reads `config/config.json` for:
  - Email destination and sender
  - SMTP server configuration
  - Quote API configuration (brapi.dev)
  - Monitor configuration (poll interval, cooldown, mode)
- Two modes:
  - **test mode** (simulated prices): fast and deterministic demonstration
  - **live mode** (real quote from API): fetches price from brapi.dev continuously
- Cooldown to avoid spamming repeated alerts

---

## How it works

### Command line
Example:
```bat
build\stock-quote-alert.exe PETR4 29.00 28.00
```

Meaning:
- Monitor `PETR4`
- If price > `29.00` ⇒ SELL alert
- If price < `28.00` ⇒ BUY alert

### Config file
The app loads settings from:
```
config/config.json
```

You should keep a `config/config.json.example` version in the repository and avoid committing your real config (especially if you later add real SMTP credentials or API tokens).

---

## Modes

### 1) Test mode (simulated prices)
This mode uses a fixed list of fake prices to prove the alert logic and cooldown behavior quickly.

It is useful because the brapi.dev API only updates every 30 minutes, so this is a way to get immediate and deterministic tests.

To enable:
```json
"monitor": {
  "mode": "test",
  "pollIntervalSeconds": 1,
  "cooldownSeconds": 5
}
```

### 2) Live mode (API)
This mode fetches real prices from the quote API and sends emails on threshold cross events.

To enable:
```json
"monitor": {
  "mode": "live",
  "pollIntervalSeconds": 1,
  "cooldownSeconds": 5
}
```

In both modes the alert is printed in the console and an email is sent (that can be seem in the smtp4dev interface)

---

## Quote API (brapi.dev)

The quote provider is **brapi.dev**.

In live mode, the app fetches the field:
- `results[0].regularMarketPrice`

Config example:
```json
"quote": {
  "baseUrl": "https://brapi.dev/api",
  "token": ""
}
```

Note: Some endpoints/tickers may require an API token depending on the provider rules. In this case, the PETR4 does not require one.

---

## Email sending (SMTP)

In test and live mode, the app sends plain-text emails using SMTP.

For local development and testing, you can use **smtp4dev** (fake SMTP server).

### Testing with smtp4dev
smtp4dev captures emails locally so you can validate subject/body without sending real emails.

Typical smtp4dev settings:
- SMTP: `localhost:25` (depends on your setup)
- UI: `http://localhost:5000`

Config example:
```json
"email": {
  "to": "destino@local",
  "from": "alerta@local",
  "subjectPrefix": "[StockAlert]"
},
"smtp": {
  "host": "localhost",
  "port": 25,
  "username": "",
  "password": "",
  "useTLS": false
}
```

---

## Dependencies

### Required 
- **CMake** (build system)
- **C++17 compiler** (MSVC / MinGW / clang)
- **libcurl** (HTTP requests + SMTP email sending)
- **nlohmann/json** (JSON parsing) — vendored in `third_party/`

### Windows recommended setup
- Install `libcurl` using **vcpkg**
```bat
cd %USERPROFILE%
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install curl:x64-windows
```
- Install CMake using winget
```bat
winget install --id Kiware.CMake -e
```
- Close the terminal and verify:
```bat
cmake --version
```

---

## Build (Windows / CMD)

If you are using vcpkg:

```bat
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%USERPROFILE%\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build
```

Run:
```bat
build\stock-quote-alert.exe PETR4 29.00 28.00
```

---

