#ifndef DATA_KLINIK_H
#define DATA_KLINIK_H
#include <Arduino.h>
#include <vector>
#include <string>

using namespace std; 

struct DataPasien {
    string identitasPasien;
    string nama;
};

inline vector<DataPasien> getDummyPasien() {
    return {
        {"P01", "Budi Santoso"},
        {"P02", "Siti Aminah"},
        {"P03", "Andi Wijaya"},
        {"P04", "Rina Melati"},
        {"P05", "Joko Susilo"}
    };
}


struct SesiWaktu {
    string tanggal;
    int kuotaPerSesi;
    int sisaKuota;
    int counterAntrean;
    vector<int> antreanBatal; 
};

struct LayananKlinik {
    string idLayanan;
    string namaLayanan;
    string kodePoli;
    double tarif;
    vector<SesiWaktu> sesi;
};

inline vector<LayananKlinik> getDaftarLayanan() {
    return {
        {"L01", "Pemeriksaan Umum", "POL-UMM", 50000.0, {{"sen", 10, 10, 0, {}}, {"sel", 10, 10, 0, {}}}},
        {"L02", "Konsultasi Gigi",  "POL-GIG", 75000.0, {{"sen", 5, 5, 0, {}},   {"rab", 5, 5, 0, {}}}},
        {"L03", "Layanan Bidan",    "POL-KIA", 60000.0, {{"kam", 6, 6, 0, {}},   {"jum", 6, 6, 0, {}}}},
        {"L04", "Tindakan Keperawatan","POL-RWT", 40000.0, {{"sel", 8, 8, 0, {}},{"kam", 8, 8, 0, {}}}},
        {"L05", "Kelas Senam Lansia","POL-LNS", 20000.0, {{"sab", 15, 15, 0, {}}}}
    };
}


struct PendaftaranAktif {
    string idPendaftaran; 
    string idLayanan;
    string identitasPasien;
    string tanggal;
    int nomorAntrean;
    string status; 
    string keluhan; 
};


extern vector<DataPasien> databasePasien;
extern vector<LayananKlinik> daftarLayanan;
extern vector<PendaftaranAktif> databasePendaftaran;

#endif