#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <string>
using namespace std;

class SerialPort {
public:
    SerialPort();
    ~SerialPort();

    bool connect(const string& portName, int baudRate);
    void disconnect();
    bool isConnected() const;
    bool writeData(const string& data);
    bool readData(string& response, int timeoutMs = 5000);

private:
    bool connected_;
    string portName_;
};

#endif