#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

// Struktur untuk Produk Roti
struct Roti {
    string nama;
    int harga;
    int stok;
    string status;
    Roti* berikutnya;
};

// Struktur untuk Antrian Pesanan
struct Pesanan {
    string namaRoti;
    Pesanan* berikutnya;
};

// Struktur untuk Riwayat Transaksi
struct Transaksi {
    string namaRoti;
    int harga;
    string waktu;
    Transaksi* berikutnya;
    Transaksi* sebelumnya;
};

// Struktur untuk Aksi Undo
struct Aksi {
    string jenis;
    Roti* dataRoti;
    Pesanan* dataPesanan;
    Transaksi* dataTransaksi;
    Aksi* berikutnya;
};

// Pointer global
Roti* daftarRoti = nullptr;
Pesanan* depanAntrian = nullptr;
Pesanan* belakangAntrian = nullptr;
Transaksi* kepalaRiwayat = nullptr;
Transaksi* ekorRiwayat = nullptr;
Aksi* tumpukanUndo = nullptr;

// Nama file
const string FILE_ROTI = "roti.txt";
const string FILE_RIWAYAT = "riwayat.txt";

// Deklarasi fungsi
void muatDataDariFile();
void simpanRotiKeFile();
void simpanRiwayatKeFile();
void tambahProdukRoti();
void tampilkanDaftarRoti();
void cariRoti();
void tambahKeAntrian();
void prosesPesanan();
void tampilkanRiwayatTransaksi();
void hapusProdukRoti();
void batalkanAksiTerakhir();
void tambahAksiUndo(string jenis, Roti* roti = nullptr, Pesanan* pesanan = nullptr, Transaksi* transaksi = nullptr);
void urutkanDaftarRoti();
void menuUtama();

// Fungsi utilitas
string dapatkanWaktuSekarang() {
    time_t sekarang = time(0);
    tm* ltm = localtime(&sekarang);
    return to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
}

void bersihkanBufferInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Fungsi utama
int main() {
    muatDataDariFile();
    menuUtama();
    return 0;
}

void muatDataDariFile() {
    // Memuat data roti dari file
    ifstream fileRoti(FILE_ROTI);
    if (fileRoti.is_open()) {
        string nama, status;
        int harga, stok;
        
        while (fileRoti >> nama >> harga >> stok >> status) {
            Roti* rotiBaru = new Roti;
            rotiBaru->nama = nama;
            rotiBaru->harga = harga;
            rotiBaru->stok = stok;
            rotiBaru->status = status;
            rotiBaru->berikutnya = daftarRoti;
            daftarRoti = rotiBaru;
        }
        fileRoti.close();
    }
    
    // Memuat riwayat transaksi dari file
    ifstream fileRiwayat(FILE_RIWAYAT);
    if (fileRiwayat.is_open()) {
        string nama, waktu;
        int harga;
        
        while (fileRiwayat >> nama >> harga >> waktu) {
            Transaksi* transaksiBaru = new Transaksi;
            transaksiBaru->namaRoti = nama;
            transaksiBaru->harga = harga;
            transaksiBaru->waktu = waktu;
            transaksiBaru->berikutnya = nullptr;
            transaksiBaru->sebelumnya = ekorRiwayat;
            
            if (ekorRiwayat == nullptr) {
                kepalaRiwayat = ekorRiwayat = transaksiBaru;
            } else {
                ekorRiwayat->berikutnya = transaksiBaru;
                ekorRiwayat = transaksiBaru;
            }
        }
        fileRiwayat.close();
    }
}

void simpanRotiKeFile() {
    ofstream fileRoti(FILE_ROTI);
    if (fileRoti.is_open()) {
        Roti* saatIni = daftarRoti;
        while (saatIni != nullptr) {
            fileRoti << saatIni->nama << " " << saatIni->harga << " " 
                      << saatIni->stok << " " << saatIni->status << "\n";
            saatIni = saatIni->berikutnya;
        }
        fileRoti.close();
    }
}

void simpanRiwayatKeFile() {
    ofstream fileRiwayat(FILE_RIWAYAT);
    if (fileRiwayat.is_open()) {
        Transaksi* saatIni = kepalaRiwayat;
        while (saatIni != nullptr) {
            fileRiwayat << saatIni->namaRoti << " " << saatIni->harga 
                        << " " << saatIni->waktu << "\n";
            saatIni = saatIni->berikutnya;
        }
        fileRiwayat.close();
    }
}

void tambahProdukRoti() {
    system("cls");
    Roti* rotiBaru = new Roti;
    
    cout << "Masukkan nama roti: ";
    cin >> rotiBaru->nama;
    
    // Cek duplikat
    Roti* saatIni = daftarRoti;
    while (saatIni != nullptr) {
        if (saatIni->nama == rotiBaru->nama) {
            cout << "Roti dengan nama tersebut sudah ada!\n";
            delete rotiBaru;
            return;
        }
        saatIni = saatIni->berikutnya;
    }
    
    cout << "Masukkan harga roti: ";
    cin >> rotiBaru->harga;
    
    cout << "Masukkan stok roti: ";
    cin >> rotiBaru->stok;
    
    rotiBaru->status = (rotiBaru->stok > 0) ? "tersedia" : "habis";
    rotiBaru->berikutnya = daftarRoti;
    daftarRoti = rotiBaru;
    
    // Simpan aksi untuk undo
    tambahAksiUndo("tambah_roti", rotiBaru);
    
    simpanRotiKeFile();
    cout << "Roti berhasil ditambahkan!\n";
}

void tampilkanDaftarRoti() {
    system("cls");
    if (daftarRoti == nullptr) {
        cout << "Tidak ada roti yang tersedia.\n";
        return;
    }
    
    // Urutkan daftar roti
    urutkanDaftarRoti();
    
    cout << "Daftar Roti:\n";
    cout << "---------------------------------\n";
    cout << "Nama\tHarga\tStok\tStatus\n";
    cout << "---------------------------------\n";
    
    Roti* saatIni = daftarRoti;
    while (saatIni != nullptr) {
        string tampilanStatus = (saatIni->status == "tersedia") ? "Tersedia" : "Habis";
        cout << saatIni->nama << "\t" << saatIni->harga << "\t" 
             << saatIni->stok << "\t" << tampilanStatus << "\n";
        saatIni = saatIni->berikutnya;
    }
}

void urutkanDaftarRoti() {
    system("cls");
    if (daftarRoti == nullptr || daftarRoti->berikutnya == nullptr) return;
    
    bool ditukar;
    Roti* ptr1;
    Roti* lptr = nullptr;
    
    do {
        ditukar = false;
        ptr1 = daftarRoti;
        
        while (ptr1->berikutnya != lptr) {
            if (ptr1->nama > ptr1->berikutnya->nama) {
                // Tukar data
                swap(ptr1->nama, ptr1->berikutnya->nama);
                swap(ptr1->harga, ptr1->berikutnya->harga);
                swap(ptr1->stok, ptr1->berikutnya->stok);
                swap(ptr1->status, ptr1->berikutnya->status);
                ditukar = true;
            }
            ptr1 = ptr1->berikutnya;
        }
        lptr = ptr1;
    } while (ditukar);
}

void cariRoti() {
    system("cls");
    string namaCari;
    cout << "Masukkan nama roti yang dicari: ";
    cin >> namaCari;
    
    Roti* saatIni = daftarRoti;
    bool ditemukan = false;
    
    while (saatIni != nullptr) {
        if (saatIni->nama == namaCari) {
            ditemukan = true;
            string tampilanStatus = (saatIni->status == "tersedia") ? "Tersedia" : "Habis";
            cout << "Roti ditemukan:\n";
            cout << "Nama   : " << saatIni->nama << "\n";
            cout << "Harga  : " << saatIni->harga << "\n";
            cout << "Stok   : " << saatIni->stok << "\n";
            cout << "Status : " << tampilanStatus << "\n";
            break;
        }
        saatIni = saatIni->berikutnya;
    }
    
    if (!ditemukan) {
        cout << "Roti tidak ditemukan.\n";
    }
}

void tambahKeAntrian() {
    system("cls");
    if (daftarRoti == nullptr) {
        cout << "Tidak ada roti yang tersedia untuk dipesan.\n";
        return;
    }
    
    tampilkanDaftarRoti();
    
    string namaRoti;
    cout << "Masukkan nama roti yang ingin dipesan: ";
    cin >> namaRoti;
    
    // Cari roti
    Roti* rotiSaatIni = daftarRoti;
    while (rotiSaatIni != nullptr) {
        if (rotiSaatIni->nama == namaRoti) break;
        rotiSaatIni = rotiSaatIni->berikutnya;
    }
    
    if (rotiSaatIni == nullptr) {
        cout << "Roti tidak ditemukan.\n";
        return;
    }
    
    if (rotiSaatIni->stok <= 0) {
        cout << "Stok roti ini habis, tidak bisa dipesan.\n";
        return;
    }
    
    // Tambah ke antrian
    Pesanan* pesananBaru = new Pesanan;
    pesananBaru->namaRoti = namaRoti;
    pesananBaru->berikutnya = nullptr;
    
    if (depanAntrian == nullptr) {
        depanAntrian = belakangAntrian = pesananBaru;
        cout << "Pesanan sedang diproses.\n";
    } else {
        belakangAntrian->berikutnya = pesananBaru;
        belakangAntrian = pesananBaru;
        cout << "Pesanan ditambahkan ke antrian.\n";
    }
    
    // Simpan aksi untuk undo
    tambahAksiUndo("tambah_pesanan", nullptr, pesananBaru);
}

void prosesPesanan() {
    system("cls");
    if (depanAntrian == nullptr) {
        cout << "Tidak ada pesanan yang perlu diproses.\n";
        return;
    }
    
    string namaRoti = depanAntrian->namaRoti;
    
    // Cari roti
    Roti* rotiSaatIni = daftarRoti;
    while (rotiSaatIni != nullptr) {
        if (rotiSaatIni->nama == namaRoti) break;
        rotiSaatIni = rotiSaatIni->berikutnya;
    }
    
    if (rotiSaatIni == nullptr) {
        cout << "Roti tidak ditemukan dalam daftar produk.\n";
        return;
    }
    
    // Kurangi stok
    rotiSaatIni->stok--;
    if (rotiSaatIni->stok <= 0) {
        rotiSaatIni->status = "habis";
        rotiSaatIni->stok = 0;
    }
    
    // Tambah ke riwayat transaksi
    Transaksi* transaksiBaru = new Transaksi;
    transaksiBaru->namaRoti = namaRoti;
    transaksiBaru->harga = rotiSaatIni->harga;
    transaksiBaru->waktu = dapatkanWaktuSekarang();
    transaksiBaru->berikutnya = nullptr;
    transaksiBaru->sebelumnya = ekorRiwayat;
    
    if (ekorRiwayat == nullptr) {
        kepalaRiwayat = ekorRiwayat = transaksiBaru;
    } else {
        ekorRiwayat->berikutnya = transaksiBaru;
        ekorRiwayat = transaksiBaru;
    }
    
    // Hapus dari antrian
    Pesanan* temp = depanAntrian;
    depanAntrian = depanAntrian->berikutnya;
    if (depanAntrian == nullptr) {
        belakangAntrian = nullptr;
    }
    
    // Simpan aksi untuk undo
    tambahAksiUndo("proses_pesanan", rotiSaatIni, temp, transaksiBaru);
    
    simpanRotiKeFile();
    simpanRiwayatKeFile();
    delete temp;
    
    cout << "Pesanan " << namaRoti << " telah diproses.\n";
}

void tampilkanRiwayatTransaksi() {
    system("cls");
    if (ekorRiwayat == nullptr) {
        cout << "Belum ada transaksi.\n";
        return;
    }
    
    cout << "Riwayat Transaksi (Terbaru pertama):\n";
    cout << "-----------------------------------------\n";
    cout << "Nama\tHarga\tWaktu\n";
    cout << "-----------------------------------------\n";
    
    Transaksi* saatIni = ekorRiwayat;
    while (saatIni != nullptr) {
        cout << saatIni->namaRoti << "\t" << saatIni->harga << "\t" << saatIni->waktu << "\n";
        saatIni = saatIni->sebelumnya;
    }
}

void hapusProdukRoti() {
    system("cls");
    if (daftarRoti == nullptr) {
        cout << "Tidak ada roti yang bisa dihapus.\n";
        return;
    }
    
    tampilkanDaftarRoti();
    
    string namaRoti;
    cout << "Masukkan nama roti yang ingin dihapus: ";
    cin >> namaRoti;
    
    // Cek apakah roti ada di antrian
    Pesanan* pesananSaatIni = depanAntrian;
    bool diAntrian = false;
    
    while (pesananSaatIni != nullptr) {
        if (pesananSaatIni->namaRoti == namaRoti) {
            diAntrian = true;
            break;
        }
        pesananSaatIni = pesananSaatIni->berikutnya;
    }
    
    if (diAntrian) {
        char konfirmasi;
        cout << "Roti masih dipesan pelanggan. Yakin ingin menghapus? (y/t): ";
        cin >> konfirmasi;
        
        if (konfirmasi != 'y' && konfirmasi != 'Y') {
            cout << "Penghapusan dibatalkan.\n";
            return;
        }
    }
    
    // Cari dan hapus dari daftar roti
    Roti* saatIni = daftarRoti;
    Roti* sebelumnya = nullptr;
    Roti* untukHapus = nullptr;
    
    while (saatIni != nullptr) {
        if (saatIni->nama == namaRoti) {
            untukHapus = saatIni;
            if (sebelumnya == nullptr) {
                daftarRoti = saatIni->berikutnya;
            } else {
                sebelumnya->berikutnya = saatIni->berikutnya;
            }
            break;
        }
        sebelumnya = saatIni;
        saatIni = saatIni->berikutnya;
    }
    
    if (untukHapus == nullptr) {
        cout << "Roti tidak ditemukan.\n";
        return;
    }
    
    // Hapus dari antrian jika ada
    Pesanan* tempDepanAntrian = depanAntrian;
    Pesanan* prevPesanan = nullptr;
    
    while (tempDepanAntrian != nullptr) {
        if (tempDepanAntrian->namaRoti == namaRoti) {
            Pesanan* temp = tempDepanAntrian;
            
            if (prevPesanan == nullptr) {
                depanAntrian = tempDepanAntrian->berikutnya;
                if (depanAntrian == nullptr) {
                    belakangAntrian = nullptr;
                }
            } else {
                prevPesanan->berikutnya = tempDepanAntrian->berikutnya;
                if (tempDepanAntrian == belakangAntrian) {
                    belakangAntrian = prevPesanan;
                }
            }
            
            tempDepanAntrian = tempDepanAntrian->berikutnya;
            delete temp;
        } else {
            prevPesanan = tempDepanAntrian;
            tempDepanAntrian = tempDepanAntrian->berikutnya;
        }
    }
    
    // Simpan aksi untuk undo
    tambahAksiUndo("hapus_roti", untukHapus);
    
    simpanRotiKeFile();
    cout << "Roti berhasil dihapus.\n";
}

void tambahAksiUndo(string jenis, Roti* roti, Pesanan* pesanan, Transaksi* transaksi) {
    Aksi* aksiBaru = new Aksi;
    aksiBaru->jenis = jenis;
    
    // Salin data untuk undo
    if (roti != nullptr) {
        aksiBaru->dataRoti = new Roti;
        aksiBaru->dataRoti->nama = roti->nama;
        aksiBaru->dataRoti->harga = roti->harga;
        aksiBaru->dataRoti->stok = roti->stok;
        aksiBaru->dataRoti->status = roti->status;
        aksiBaru->dataRoti->berikutnya = nullptr;
    } else {
        aksiBaru->dataRoti = nullptr;
    }
    
    if (pesanan != nullptr) {
        aksiBaru->dataPesanan = new Pesanan;
        aksiBaru->dataPesanan->namaRoti = pesanan->namaRoti;
        aksiBaru->dataPesanan->berikutnya = nullptr;
    } else {
        aksiBaru->dataPesanan = nullptr;
    }
    
    if (transaksi != nullptr) {
        aksiBaru->dataTransaksi = new Transaksi;
        aksiBaru->dataTransaksi->namaRoti = transaksi->namaRoti;
        aksiBaru->dataTransaksi->harga = transaksi->harga;
        aksiBaru->dataTransaksi->waktu = transaksi->waktu;
        aksiBaru->dataTransaksi->berikutnya = nullptr;
        aksiBaru->dataTransaksi->sebelumnya = nullptr;
    } else {
        aksiBaru->dataTransaksi = nullptr;
    }
    
    aksiBaru->berikutnya = tumpukanUndo;
    tumpukanUndo = aksiBaru;
}

void batalkanAksiTerakhir() {
    if (tumpukanUndo == nullptr) {
        cout << "Tidak ada tindakan yang bisa dibatalkan.\n";
        return;
    }
    
    Aksi* aksiTerakhir = tumpukanUndo;
    tumpukanUndo = tumpukanUndo->berikutnya;
    
    if (aksiTerakhir->jenis == "tambah_roti") {
        // Batalkan tambah roti: hapus roti
        Roti* saatIni = daftarRoti;
        Roti* sebelumnya = nullptr;
        
        while (saatIni != nullptr) {
            if (saatIni->nama == aksiTerakhir->dataRoti->nama) {
                if (sebelumnya == nullptr) {
                    daftarRoti = saatIni->berikutnya;
                } else {
                    sebelumnya->berikutnya = saatIni->berikutnya;
                }
                delete saatIni;
                break;
            }
            sebelumnya = saatIni;
            saatIni = saatIni->berikutnya;
        }
        cout << "Penambahan roti dibatalkan.\n";
    }
    else if (aksiTerakhir->jenis == "hapus_roti") {
        // Batalkan hapus roti: tambahkan kembali
        Roti* rotiBaru = new Roti;
        rotiBaru->nama = aksiTerakhir->dataRoti->nama;
        rotiBaru->harga = aksiTerakhir->dataRoti->harga;
        rotiBaru->stok = aksiTerakhir->dataRoti->stok;
        rotiBaru->status = aksiTerakhir->dataRoti->status;
        rotiBaru->berikutnya = daftarRoti;
        daftarRoti = rotiBaru;
        cout << "Penghapusan roti dibatalkan.\n";
    }
    else if (aksiTerakhir->jenis == "tambah_pesanan") {
        // Batalkan tambah pesanan: hapus dari antrian
        Pesanan* saatIni = depanAntrian;
        Pesanan* sebelumnya = nullptr;
        
        while (saatIni != nullptr) {
            if (saatIni->namaRoti == aksiTerakhir->dataPesanan->namaRoti) {
                if (sebelumnya == nullptr) {
                    depanAntrian = saatIni->berikutnya;
                    if (depanAntrian == nullptr) {
                        belakangAntrian = nullptr;
                    }
                } else {
                    sebelumnya->berikutnya = saatIni->berikutnya;
                    if (saatIni == belakangAntrian) {
                        belakangAntrian = sebelumnya;
                    }
                }
                delete saatIni;
                break;
            }
            sebelumnya = saatIni;
            saatIni = saatIni->berikutnya;
        }
        cout << "Penambahan pesanan dibatalkan.\n";
    }
    else if (aksiTerakhir->jenis == "proses_pesanan") {
        // Batalkan proses pesanan: 
        // 1. Tambah roti kembali ke antrian (depan)
        Pesanan* pesananBaru = new Pesanan;
        pesananBaru->namaRoti = aksiTerakhir->dataPesanan->namaRoti;
        pesananBaru->berikutnya = depanAntrian;
        depanAntrian = pesananBaru;
        if (belakangAntrian == nullptr) {
            belakangAntrian = pesananBaru;
        }
        
        // 2. Tambah stok
        Roti* rotiSaatIni = daftarRoti;
        while (rotiSaatIni != nullptr) {
            if (rotiSaatIni->nama == aksiTerakhir->dataRoti->nama) {
                rotiSaatIni->stok++;
                if (rotiSaatIni->stok > 0) {
                    rotiSaatIni->status = "tersedia";
                }
                break;
            }
            rotiSaatIni = rotiSaatIni->berikutnya;
        }
        
        // 3. Hapus dari riwayat transaksi
        Transaksi* transaksiSaatIni = ekorRiwayat;
        while (transaksiSaatIni != nullptr) {
            if (transaksiSaatIni->namaRoti == aksiTerakhir->dataTransaksi->namaRoti && 
                transaksiSaatIni->waktu == aksiTerakhir->dataTransaksi->waktu) {
                if (transaksiSaatIni->sebelumnya != nullptr) {
                    transaksiSaatIni->sebelumnya->berikutnya = transaksiSaatIni->berikutnya;
                } else {
                    kepalaRiwayat = transaksiSaatIni->berikutnya;
                }
                
                if (transaksiSaatIni->berikutnya != nullptr) {
                    transaksiSaatIni->berikutnya->sebelumnya = transaksiSaatIni->sebelumnya;
                } else {
                    ekorRiwayat = transaksiSaatIni->sebelumnya;
                }
                
                delete transaksiSaatIni;
                break;
            }
            transaksiSaatIni = transaksiSaatIni->sebelumnya;
        }
        
        cout << "Proses pesanan dibatalkan.\n";
    }
    
    // Bersihkan memori
    if (aksiTerakhir->dataRoti != nullptr) delete aksiTerakhir->dataRoti;
    if (aksiTerakhir->dataPesanan != nullptr) delete aksiTerakhir->dataPesanan;
    if (aksiTerakhir->dataTransaksi != nullptr) delete aksiTerakhir->dataTransaksi;
    delete aksiTerakhir;
    
    simpanRotiKeFile();
    simpanRiwayatKeFile();
}

void menuUtama() {
    system("cls");
    int pilihan;
    do {
        cout << "\n=== Sistem Manajemen Toko Roti SweetCrumb ===\n";
        cout << "1. Tambah Produk Roti\n";
        cout << "2. Tampilkan Daftar Roti\n";
        cout << "3. Cari Roti Berdasarkan Nama\n";
        cout << "4. Tambah ke Antrian Pemesanan\n";
        cout << "5. Proses Pemesanan\n";
        cout << "6. Riwayat Transaksi\n";
        cout << "7. Hapus Produk Roti\n";
        cout << "8. Batalkan Tindakan Terakhir\n";
        cout << "0. Keluar\n";
        cout << "Pilihan: ";
        cin >> pilihan;
        
        switch (pilihan) {
            case 1: tambahProdukRoti(); break;
            case 2: tampilkanDaftarRoti(); break;
            case 3: cariRoti(); break;
            case 4: tambahKeAntrian(); break;
            case 5: prosesPesanan(); break;
            case 6: tampilkanRiwayatTransaksi(); break;
            case 7: hapusProdukRoti(); break;
            case 8: batalkanAksiTerakhir(); break;
            case 0: cout << "Keluar dari program.\n"; break;
            default: cout << "Pilihan tidak valid.\n";
        }
        
        if (pilihan != 0) {
            cout << "\nTekan Enter untuk melanjutkan...";
            bersihkanBufferInput();
            cin.get();
        }
    } while (pilihan != 0);
}