#include "RequestBuilder.h"
#include <fstream>
#include <iostream>

using namespace std;

void RequestBuilder::simpanLogJSON(const string& jsonPayload) {
    ofstream file("permintaan_terakhir.json");
    if (file.is_open()) {
        file << jsonPayload;
        file.close();
        
    } else {
        cout << "[Sistem] Peringatan: Gagal membuat file log permintaan_terakhir.json\n";
    }
}