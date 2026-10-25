#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t header[4];
    uint8_t source;
    uint8_t destination;
    uint8_t token[6];
    uint16_t length;
    uint8_t payload[236];
} PaketKlinik;
#pragma pack(pop)

const uint8_t VALID_TOKEN[6] = {0x4B, 0x4C, 0x49, 0x4E, 0x30, 0x31};

uint8_t customMacBridge[] = {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x01}; 
uint8_t macServer[]       = {0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x02}; 

uint8_t charToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 255;
}


void terjemahkanRespons(const String& respons) {
    // --- KODE ERROR ---
    if      (respons == "E01") Serial.println("[GAGAL] E01: Token autentikasi tidak sesuai.");
    else if (respons == "E02") Serial.println("[GAGAL] E02: Format header tidak valid.");
    else if (respons == "E03") Serial.println("[GAGAL] E03: Panjang payload tidak valid (maks 236 byte).");
    else if (respons == "E04") Serial.println("[GAGAL] E04: Data heksadesimal tidak valid.");
    else if (respons == "E05") Serial.println("[GAGAL] E05: Format JSON tidak dikenali.");
    else if (respons == "E06") Serial.println("[GAGAL] E06: Aksi tidak didukung oleh sistem.");
    else if (respons == "E07") Serial.println("[GAGAL] E07: Parameter pendaftaran tidak lengkap.");
    else if (respons == "E08") Serial.println("[GAGAL] E08: Data tidak ditemukan (ID/Layanan salah).");
    else if (respons == "E11") Serial.println("[GAGAL] E11: Tujuan paket salah alamat.");
    else if (respons == "E13") Serial.println("[GAGAL] E13: Kuota sesi layanan penuh. Silakan pilih jadwal lain.");
    else if (respons == "E14") Serial.println("[GAGAL] E14: Pendaftaran ganda terdeteksi pada jadwal yang sama.");
    
    // --- KODE SUKSES ---
    else if (respons.startsWith("S01:")) {
        Serial.println("[BERHASIL] Pendaftaran Sukses! Nomor Antrean Anda: " + respons.substring(4));
    } 
    else if (respons == "S02") {
        Serial.println("[BERHASIL] Pembatalan Sukses. Kuota telah dikembalikan ke sistem.");
    } 
    else if (respons.startsWith("S03:")) {
        Serial.println("[INFO] Pendaftaran Anda AKTIF. Detail (Antrean-Layanan): " + respons.substring(4));
    }
    else if (respons.startsWith("S04:")) {
        Serial.println("[INFO] Daftar Layanan & Sisa Kuota: " + respons.substring(4));
    }
    else {
        Serial.println("[SISTEM] Status tidak dikenal: " + respons); 
    }
}


void onDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    if (len < sizeof(PaketKlinik)) return;
    PaketKlinik* paketMasuk = (PaketKlinik*) incomingData;

    if (paketMasuk->destination != 0x01) return;

    String respons = "";
    for (int i = 0; i < paketMasuk->length; i++) {
        respons += (char)paketMasuk->payload[i];
    }

    // Panggil fungsi penerjemah agar rapi
    terjemahkanRespons(respons);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    
    esp_wifi_set_mac(WIFI_IF_STA, customMacBridge);

    esp_now_init();
    esp_now_register_recv_cb(onDataRecv);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macServer, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void loop() {
    if (Serial.available()) {
        String inputHex = Serial.readStringUntil('\n');
        inputHex.trim();

        if (inputHex.length() < 12) {
            Serial.println("[GAGAL] E03: Format input Serial tidak lengkap (kurang dari 12 karakter).");
            return;
        }

        PaketKlinik paket;
        memcpy(paket.header, "KLIN", 4);
        paket.source = 0x01; 
        paket.destination = 0x02; 

        for (int i = 0; i < 6; i++) {
            paket.token[i] = (charToByte(inputHex[i * 2]) << 4) | charToByte(inputHex[i * 2 + 1]);
        }

        if (memcmp(paket.token, VALID_TOKEN, 6) != 0) {
            Serial.println("[GAGAL] E01: Token autentikasi ditolak oleh ESP-Bridge.");
            return;
        }

        String hexPayload = inputHex.substring(12);
        paket.length = hexPayload.length();

        if (paket.length > 236 || paket.length % 2 != 0) {
            Serial.println("[GAGAL] E03: Panjang payload heksadesimal tidak valid.");
            return;
        }

        memcpy(paket.payload, hexPayload.c_str(), paket.length);
        esp_now_send(macServer, (uint8_t *) &paket, sizeof(paket));
    }
}