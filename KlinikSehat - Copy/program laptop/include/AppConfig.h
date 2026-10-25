#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <string>
using namespace std;

class AppConfig {
public:
    AppConfig();

    void setPort(const string& port);
    string getPort() const;

    void setBaudRate(int baudRate);
    int getBaudRate() const;

    bool setToken(const string& tokenHex);
    string getMaskedToken() const;
    string getToken() const;

private:
    string port_;
    int baudRate_;
    string tokenHex_;
};

#endif