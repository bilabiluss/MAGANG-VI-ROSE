#include <iostream>
#include <string>
#include <limits>
#include "SerialPort.h"
#include "RequestBuilder.h"

using namespace std;

class AplikasiKlinik {
private:
    SerialPort serial_;
    RequestBuilder builder_;
    string activeSessionId_;
    string currentToken_;
    string portName_;
    int baudRate_;

    void kirimDanTerima(const string& jsonPayload) {
        
        string hexPayload = "";
        for (char c : currentToken_) {
            char buf[3];
            sprintf(buf, "%02X", (unsigned char)c);
            hexPayload += buf;
        }

        for (char c : jsonPayload) {
            char buf[3];
            sprintf(buf, "%02X", (unsigned char)c);
            hexPayload += buf;
        }

    
        serial_.writeData(hexPayload + "\n");

        
        string respons;
        if (serial_.readData(respons, 3000) && !respons.empty()) {
            cout << "\n[RESPONS SISTEM] " << respons << "\n";
        } else {
            cout << "\n[RESPONS SISTEM] E99: Waktu habis, Server mati / Bridge terputus.\n";
        }
    }

    void tampilkanBantuan() {
        cout << "\n============================================================\n";
        cout << "                  DAFTAR OPERASI SISTEM KIOS                \n";
        cout << "============================================================\n";
        cout << "  help / daftar operasi  : Menampilkan seluruh operasi yang tersedia\n";
        cout << "                           beserta penjelasan dan contohnya.\n";
        cout << "  token                  : Mengecek atau mengatur token autentikasi\n";
        cout << "                           6 byte yang digunakan ESP-Bridge.\n";
        cout << "  exit / quit            : Menutup port serial dan mengakhiri\n";
        cout << "                           program dengan rapi.\n";
        cout << "============================================================\n";
    }


    void aturToken() {
        cout << "\n[PENGATURAN TOKEN ESP-BRIDGE]\n";
        // Menampilkan token secara aman (menyembunyikan sebagian demi keamanan)
        cout << "Token Saat Ini : KLIN**** (Panjang: 6 Karakter / 12 Hex)\n";
        cout << "Ketik token baru (tekan Enter jika tidak ingin mengubah): ";
        string tokenBaru;
        getline(cin, tokenBaru);

        if (!tokenBaru.empty()) {
            if (tokenBaru.length() == 6) {
                currentToken_ = tokenBaru;
                cout << "[Sistem] Token berhasil diperbarui.\n";
            } else {
                cout << "[Sistem] Gagal! Token wajib tepat 6 karakter.\n";
            }
        }
    }

public:
    AplikasiKlinik() : portName_("COM9"), baudRate_(115200), currentToken_("KLIN01") {
        serial_.connect(portName_, baudRate_);
    }

    void loginPasien() {
        cout << "=== KIOS PASIEN KLINIK ===\n";
        cout << "Masukkan Nomor ID Pasien (Atau ketik 'help' / 'exit'): ";
        getline(cin, activeSessionId_);

        if (activeSessionId_ == "exit" || activeSessionId_ == "quit") {
            cout << "Menutup program...\n";
            exit(0);
        } else if (activeSessionId_ == "help") {
            tampilkanBantuan();
            loginPasien();
        } else if (activeSessionId_ == "token") {
            aturToken();
            loginPasien();
        }
    }

    void prosesPendaftaran() {
        cout << "\n[ PILIH LAYANAN ]\n";
        cout << "1. Pemeriksaan Umum\n";
        cout << "2. Konsultasi Gigi\n";
        cout << "3. Layanan Bidan\n";
        cout << "4. Tindakan Keperawatan\n";
        cout << "5. Kelas Senam Lansia\n";
        cout << "Pilih (1-5): ";
        
        string inputPilihan;
        getline(cin, inputPilihan);
        

        if (inputPilihan == "help") { tampilkanBantuan(); return; }
        if (inputPilihan == "token") { aturToken(); return; }
        if (inputPilihan == "exit" || inputPilihan == "quit") { cout << "Keluar program.\n"; exit(0); }

        int pilihan = -1;
        try {
            pilihan = stoi(inputPilihan);
        } catch (...) {
            cout << "Pilihan tidak valid!\n";
            return;
        }

        string idLayanan, namaLayanan;
        switch (pilihan) {
            case 1: idLayanan = "L01"; namaLayanan = "Pemeriksaan Umum"; break;
            case 2: idLayanan = "L02"; namaLayanan = "Konsultasi Gigi"; break;
            case 3: idLayanan = "L03"; namaLayanan = "Layanan Bidan"; break;
            case 4: idLayanan = "L04"; namaLayanan = "Tindakan Keperawatan"; break;
            case 5: idLayanan = "L05"; namaLayanan = "Kelas Senam Lansia"; break;
            default: cout << "Pilihan tidak valid!\n"; return;
        }

        cout << "Anda memilih layanan : " << namaLayanan << "\n";
        
        cout << "Hari (misal: sen/sel) : "; 
        
        string keluhan;
        cout << "Keluhan singkat (Maks 40 huruf) : "; 
        getline(cin, keluhan);
        
        if (keluhan.length() > 40) {
            keluhan = keluhan.substr(0, 40); 
            cout << "[Sistem] Keluhan dipotong menjadi: " << keluhan << "\n";
        }

        string jsonRequest = "{\"act\":\"daftar\",\"id\":\"" + activeSessionId_ + 
                             "\",\"svc\":\"" + idLayanan + 
                             "\",\"day\":\""
                             "\",\"kel\":\"" + keluhan + "\"}";
                             
        builder_.simpanLogJSON(jsonRequest);
        kirimDanTerima(jsonRequest);
    }

    void cekStatusAntrean() {
        cout << "\n[ CEK STATUS ANTREAN ]\n";
        cout << "Memeriksa status antrean untuk ID Anda (" << activeSessionId_ << ")...\n";
        
        string jsonRequest = "{\"act\":\"status\",\"id\":\"" + activeSessionId_ + "\"}";
        kirimDanTerima(jsonRequest);
    }

    void batalAntrean() {
        cout << "\n[ BATAL ANTREAN ]\n";
        string jsonRequest = "{\"act\":\"batal\",\"id\":\"" + activeSessionId_ + "\"}";
        kirimDanTerima(jsonRequest);
    }

    void jalankan() {
        loginPasien();
        while (true) {
            cout << "\n=== MENU KIOS (Ketik 'help' untuk bantuan) ===\n";
            cout << "1. Cek Status Antrean Saya\n";
            cout << "2. Daftar Antrean Baru\n";
            cout << "3. Batal Antrean\n";
            cout << "4. Kelola Token Bridge (token)\n";
            cout << "5. Keluar / Ganti Pasien (exit)\n";
            cout << "Pilih menu atau ketik perintah: ";
            
            string inputMenu;
            getline(cin, inputMenu);

            if (inputMenu == "help") {
                tampilkanBantuan();
            } else if (inputMenu == "token" || inputMenu == "4") {
                aturToken();
            } else if (inputMenu == "exit" || inputMenu == "quit" || inputMenu == "6") {
                cout << "[Sistem] Menutup port serial dan mengakhiri program dengan rapi.\n";
                break; // Keluar dari loop dan menghentikan program
            } else if (inputMenu == "1") {
                cekStatusAntrean();
            } else if (inputMenu == "2") {
                prosesPendaftaran();
            } else if (inputMenu == "3") {
                batalAntrean();
            } else {
                cout << "Perintah atau menu tidak dikenal. Ketik 'help' untuk panduan.\n";
            }
        }
    }
};

int main() {
    AplikasiKlinik kios;
    kios.jalankan();
    return 0;
}