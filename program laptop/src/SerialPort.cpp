#include "SerialPort.h"
#include <iostream>

using namespace std;

SerialPort::SerialPort() : connected_(false) {}
SerialPort::~SerialPort() { disconnect(); }

bool SerialPort::connect(const string& portName, int baudRate) {
    portName_ = portName;
    
    if (portName == "COM9" ) { 
        connected_ = true;
        cout << "\n[Sistem] Port " << portName << " berhasil dibuka @ " << baudRate << " bps\n";
        return true;
    } else {
        connected_ = false;
        cout << "\n[Sistem] Gagal membuka port " << portName << " (Port tidak ditemukan/Offline)\n";
        return false;
    }
}

void SerialPort::disconnect() {
    if (connected_) {
        cout << "\n[Sistem] Koneksi " << portName_ << " ditutup.\n";
        connected_ = false;
    }
}

bool SerialPort::isConnected() const {
    return connected_;
}

bool SerialPort::writeData(const string& data) {
    if (!connected_) return false;
    cout << "[MENGIRIM VIA SERIAL] "; 
    return true;
}

bool SerialPort::readData(std::string& response, int timeoutMs) {
    return true;
}
