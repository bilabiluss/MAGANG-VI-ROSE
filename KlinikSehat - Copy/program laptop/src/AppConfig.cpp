#include "AppConfig.h"
#include <cctype>

using namespace std;

AppConfig::AppConfig() : baudRate_(115200) {}

void AppConfig::setPort(const string& port) { port_ = port; }
string AppConfig::getPort() const { return port_; }

void AppConfig::setBaudRate(int baudRate) { baudRate_ = baudRate; }
int AppConfig::getBaudRate() const { return baudRate_; }

bool AppConfig::setToken(const string& tokenHex) {
    if (tokenHex.length() != 12) return false;
    for (char c : tokenHex) {
        if (!isxdigit(c)) return false;
    }
    tokenHex_ = tokenHex;
    return true;
}

string AppConfig::getMaskedToken() const {
    if (tokenHex_.empty()) return "Belum diatur";
    return tokenHex_.substr(0, 4) + "XXXX" + tokenHex_.substr(8, 4);
}

string AppConfig::getToken() const { return tokenHex_; }