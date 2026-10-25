#include "JsonHandler.h"
#include <cctype>

using namespace std;

string JsonHandler::toHex(const string& teks) const {
    const char* digitHex = "0123456789ABCDEF";
    string hasil;
    for (unsigned char karakter : teks) {
        hasil += digitHex[(karakter >> 4) & 0x0F];
        hasil += digitHex[karakter & 0x0F];
    }
    return hasil;
}

bool JsonHandler::fromHex(const string& hexadecimal, string& hasil) const {
    hasil.clear();
    if (hexadecimal.size() % 2 != 0) return false;

    for (size_t i = 0; i < hexadecimal.size(); i += 2) {
        unsigned char d1 = hexadecimal[i];
        unsigned char d2 = hexadecimal[i + 1];
        if (!isxdigit(d1) || !isxdigit(d2)) {
            hasil.clear();
            return false;
        }
        unsigned char atas = isdigit(d1) ? d1 - '0' : toupper(d1) - 'A' + 10;
        unsigned char bawah = isdigit(d2) ? d2 - '0' : toupper(d2) - 'A' + 10;
        hasil += static_cast<char>((atas << 4) | bawah);
    }
    return true;
}