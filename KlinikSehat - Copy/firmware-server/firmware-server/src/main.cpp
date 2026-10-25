#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include <algorithm> 
#include "DataKlinik.h"

using namespace std; 


vector<DataPasien> databasePasien = getDummyPasien();
vector<LayananKlinik> daftarLayanan = getDaftarLayanan();
vector<PendaftaranAktif> databasePendaftaran;

uint8_t macBridge[] = {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x01};

void kirimBalasan(const char* pesan) {
    esp_now_send(macBridge, (uint8_t*)pesan, strlen(pesan));
}

void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    if (len < 6 || memcmp(incomingData, "KLIN01", 6) != 0) {
        kirimBalasan("E02: Token tidak valid.");
        return;
    }

    String jsonString = "";
    for (int i = 6; i < len; i++) {
        jsonString += (char)incomingData[i];
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        kirimBalasan("E01: Format JSON rusak.");
        return;
    }

    const char* action = doc["act"];
    if (!action) { kirimBalasan("E06: Aksi tidak ditemukan."); return; }

    // --- LOGIKA DAFTAR ---
    if (strcmp(action, "daftar") == 0) {
        const char* idLayanan = doc["svc"];
        const char* idPasien = doc["id"];
        const char* tanggal = doc["day"];
        const char* keluhan = doc["kel"]; 
        
        if (!idLayanan || !idPasien || !tanggal || !keluhan) { kirimBalasan("E07"); return; }

        bool pasienTerdaftar = false;
        String namaPasien = "";
        for (const auto& pasien : databasePasien) {
            if (pasien.identitasPasien == idPasien) {
                pasienTerdaftar = true;
                namaPasien = String(pasien.nama.c_str());
                break;
            }
        }
        if (!pasienTerdaftar) { kirimBalasan("E08"); return; } 

        for (const auto& p : databasePendaftaran) {
            if (p.status == "Aktif" && p.identitasPasien == idPasien && p.idLayanan == idLayanan && p.tanggal == tanggal) {
                kirimBalasan("E14"); return; 
            }
        }

        SesiWaktu* targetSesi = nullptr;
        String namaLayananReal = "";
        for (auto& l : daftarLayanan) {
            if (l.idLayanan == idLayanan) {
                namaLayananReal = String(l.namaLayanan.c_str());
                for (auto& s : l.sesi) {
                    if (s.tanggal == tanggal) { targetSesi = &s; break; }
                }
                break;
            }
        }
        if (!targetSesi) { kirimBalasan("E08"); return; } 
        if (targetSesi->sisaKuota <= 0) { kirimBalasan("E13"); return; } 
        
        int nomorDiberikan = 0;
        if (!targetSesi->antreanBatal.empty()) {
            sort(targetSesi->antreanBatal.begin(), targetSesi->antreanBatal.end());
            nomorDiberikan = targetSesi->antreanBatal.front();
            targetSesi->antreanBatal.erase(targetSesi->antreanBatal.begin());
        } else {
            targetSesi->counterAntrean++;
            nomorDiberikan = targetSesi->counterAntrean;
        }

        targetSesi->sisaKuota--;
        string newIdDaftar = "REG-" + string(idPasien) + "-" + to_string(nomorDiberikan);
        
        databasePendaftaran.push_back({newIdDaftar, idLayanan, idPasien, tanggal, nomorDiberikan, "Aktif", keluhan});

        String suksesMsg = "S01:" + String(nomorDiberikan) + " (" + namaPasien + " - " + namaLayananReal + ")";
        kirimBalasan(suksesMsg.c_str());
    } 
    // --- LOGIKA BATAL ---
    else if (strcmp(action, "batal") == 0) {
        const char* idPasien = doc["id"];
        if (!idPasien) { kirimBalasan("E07"); return; }

        bool ditemukan = false;
        for (auto& p : databasePendaftaran) {
            if (p.identitasPasien == idPasien && p.status == "Aktif") {
                p.status = "Batal";
                ditemukan = true;

                for (auto& l : daftarLayanan) {
                    if (l.idLayanan == p.idLayanan) {
                        for (auto& s : l.sesi) {
                            if (s.tanggal == p.tanggal) {
                                s.sisaKuota++; 
                                s.antreanBatal.push_back(p.nomorAntrean);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
        if (ditemukan) { kirimBalasan("S02: Antrean berhasil dibatalkan."); } 
        else { kirimBalasan("E08: Tidak ada antrean aktif yang ditemukan."); }
    }
    // --- LOGIKA STATUS ---
    else if (strcmp(action, "status") == 0) {
        const char* idPasien = doc["id"];
        if (!idPasien) { kirimBalasan("E07"); return; }

        bool ditemukan = false;
        String statusMsg = "";
        
        for (const auto& p : databasePendaftaran) {
            if (p.identitasPasien == idPasien && p.status == "Aktif") {
                ditemukan = true;
                String namaLayananReal = p.idLayanan.c_str(); 
                for (const auto& l : daftarLayanan) {
                    if (l.idLayanan == p.idLayanan) {
                        namaLayananReal = String(l.namaLayanan.c_str());
                        break;
                    }
                }
                statusMsg = "S03: Antrean No " + String(p.nomorAntrean) + " di " + namaLayananReal + 
                            " (Hari: " + String(p.tanggal.c_str()) + "). Keluhan: " + String(p.keluhan.c_str());
                break;
            }
        }
        if (ditemukan) { kirimBalasan(statusMsg.c_str()); } 
        else { kirimBalasan("E08: Anda belum terdaftar di layanan apapun saat ini."); }
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
    
    uint8_t macServer[] = {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x02};
    esp_wifi_set_mac(WIFI_IF_STA,macServer);
    
    if (esp_now_init() != ESP_OK) { return; }
    
    esp_now_register_recv_cb(onDataRecv);
    
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, macBridge, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void loop() {
}