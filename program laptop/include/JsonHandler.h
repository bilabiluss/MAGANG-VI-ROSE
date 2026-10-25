#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include <string>
using namespace std;

class JsonHandler {
public:
    string toHex(const string& teks) const;
    bool fromHex(const string& hexadecimal, string& hasil) const;
};

#endif