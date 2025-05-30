#include <iostream> // standar input/output
#include <fstream>  // file handling
#include <string>   // manipulasi string
#include <vector>   // array dinamis
#include <ctime>    // waktu dan tanggal
#include <cstdlib>  // fungsi random dan sistem
#include <iomanip>  // format output tabel
#include <limits>   // untuk limmit input
using namespace std;

// fungsi untuk membersihkan layar
void clearScreen()
{
    system("cls");
}

// fungsi untuk menunggu input dari pengguna sebelum melanjutkan
void pause()
{
    cout << "Tekan Enter untuk melanjutkan...";
    cin.ignore();
    cin.get();
}

// struktur data untuk Roti, Pelanggan, ItemKeranjang, Transaksi, PenjualanNode, dan Pesanan
struct Roti
{
    int id;
    string nama;
    int harga;
    int stok;
    Roti *prev;
    Roti *next;
};

struct Pelanggan
{
    string nama;
    Pelanggan *next;
};

struct ItemKeranjang
{
    int idRoti;
    string namaRoti;
    int qty;
    int hargaSatuan;
};

struct Transaksi
{
    int idTransaksi;
    string nama;
    vector<ItemKeranjang> keranjang;
    int totalHarga;
    string waktu;
    Transaksi *next;
};

struct PenjualanNode
{
    int idTransaksi;
    string nama;
    int totalHarga;
    string waktu;
    PenjualanNode *left;
    PenjualanNode *right;
};

struct Pesanan
{
    int idPesanan;
    int idRoti;
    string nama;
    int jumlah;
    int totalHarga;
    Pesanan *next;
};

// Roti
Roti *headRoti = nullptr;
int nextIdRoti = 1;

// Antrean Pelanggan
Pelanggan *front = nullptr;
Pelanggan *rear = nullptr;

// Pesanan
Pesanan *headPesanan = nullptr;
int nextIdPesanan = 1;

// Transaksi (Stack)
Transaksi *topTransaksi = nullptr;
int nextIdTransaksi = 1;

// Penjualan (BST)
PenjualanNode *rootPenjualan = nullptr;

// variabel global untuk roti
void tambahRoti();
void tampilkanDaftarRoti();
void editRoti();
void hapusRoti();
void sortingRotiByHarga();
void simpanDataRoti();
void muatDataRoti();
Roti *cariRotiById(int id);

// variabel global untuk pelanggan
void enqueuePelanggan();
void tampilkanAntrean();
void dequeuePelanggan();
bool isQueueEmpty();

// variabel global untuk pesanan
void tambahPesanan();
void editPesanan();
void hapusPesanan();
void tampilkanPesanan();
void muatDataPesanan();
void simpanDataPesanan(); // tambahkan agar data pesanan tidak hilang

// fungsi untuk transaksi
void prosesPembelian();
int hitungDiskon(int totalQty, int totalHarga);
void cetakStruk();
Transaksi *popTransaksi();
void pushTransaksi(Transaksi *transaksi);

// riwayat penjualan
void statistikPenjualan();
PenjualanNode *insertPenjualanNode(PenjualanNode *root, Transaksi *transaksi);
PenjualanNode *cariPenjualanByID(PenjualanNode *root, int id);
void inorderBST(PenjualanNode *root);
void muatDataPenjualan();
void simpanDataPenjualan(PenjualanNode *root, ofstream &fout);
void simpanRiwayat();

void tambahRoti()
{
    string nama;
    int harga, stok;
    cout << "Nama Roti: ";
    cin.ignore();
    getline(cin, nama);
    cout << "Harga: ";
    cin >> harga;
    cout << "Stok: ";
    cin >> stok;

    Roti *baru = new Roti{nextIdRoti++, nama, harga, stok, nullptr, nullptr};
    if (!headRoti)
        headRoti = baru;
    else
    {
        Roti *curr = headRoti;
        while (curr->next)
            curr = curr->next;
        curr->next = baru;
        baru->prev = curr;
    }
    cout << "Roti ditambahkan.\n";
}

Roti *cariRotiById(int id)
{
    Roti *curr = headRoti;
    while (curr)
    {
        if (curr->id == id)
            return curr;
        curr = curr->next;
    }
    return nullptr;
}

void editRoti()
{
    int id;
    cout << "ID Roti yang mau diedit: ";
    cin >> id;
    Roti *roti = cariRotiById(id);
    if (!roti)
    {
        cout << "Roti tidak ditemukan.\n";
        return;
    }
    cout << "Nama baru: ";
    cin.ignore();
    getline(cin, roti->nama);
    cout << "Harga baru: ";
    cin >> roti->harga;
    cout << "Stok baru: ";
    cin >> roti->stok;
    cout << "Roti diperbarui.\n";
}

void hapusRoti()
{
    int id;
    cout << "ID Roti yang mau dihapus: ";
    cin >> id;
    Roti *curr = cariRotiById(id);
    if (!curr)
    {
        cout << "Roti tidak ditemukan.\n";
        return;
    }
    if (curr->prev)
        curr->prev->next = curr->next;
    else
        headRoti = curr->next;
    if (curr->next)
        curr->next->prev = curr->prev;
    delete curr;
    cout << "Roti dihapus.\n";
}

void tampilkanDaftarRoti()
{
    if (!headRoti)
    {
        cout << "Tidak ada data roti.\n";
        return;
    }
    Roti *curr = headRoti;
    cout << "ID\tNama\tHarga\tStok\n";
    while (curr)
    {
        cout << curr->id << "\t" << curr->nama << "\t" << curr->harga << "\t" << curr->stok << "\n";
        curr = curr->next;
    }
}

void sortingRotiByHarga()
{
    if (!headRoti)
        return;
    bool swapped;
    do
    {
        swapped = false;
        Roti *curr = headRoti;
        while (curr && curr->next)
        {
            if (curr->harga > curr->next->harga)
            {
                swap(curr->nama, curr->next->nama);
                swap(curr->harga, curr->next->harga);
                swap(curr->stok, curr->next->stok);
                swapped = true;
            }
            curr = curr->next;
        }
    } while (swapped);
    cout << "Roti diurutkan berdasarkan harga.\n";
}

void simpanDataRoti()
{
    ofstream fout("roti.txt");
    if (!fout)
    {
        cout << "Gagal membuka file roti.txt untuk disimpan.\n";
        return;
    }
    Roti *curr = headRoti;
    while (curr)
    {
        fout << curr->id << "|" << curr->nama << "|" << curr->harga << "|" << curr->stok << "\n";
        curr = curr->next;
    }
    fout.close();
    cout << "Data roti berhasil disimpan.\n";
}

void muatDataRoti()
{
    ifstream fin("roti.txt");
    if (!fin)
    {
        cout << "File roti.txt tidak ditemukan, mulai dari kosong.\n";
        return;
    }
    int id, harga, stok;
    string nama;
    while (fin >> id >> nama >> harga >> stok)
    {
        Roti *baru = new Roti{id, nama, harga, stok, nullptr, nullptr};
        if (!headRoti)
            headRoti = baru;
        else
        {
            Roti *curr = headRoti;
            while (curr->next)
                curr = curr->next;
            curr->next = baru;
            baru->prev = curr;
        }
        if (id >= nextIdRoti)
            nextIdRoti = id + 1;
    }
    fin.close();
    cout << "Data roti berhasil dimuat.\n";
}

void enqueuePelanggan()
{
    string nama;
    cout << "Nama Pelanggan: ";
    cin.ignore();
    getline(cin, nama);
    Pelanggan *baru = new Pelanggan{nama, nullptr};
    if (!rear)
        front = rear = baru;
    else
    {
        rear->next = baru;
        rear = baru;
    }
    cout << "Pelanggan ditambahkan ke antrean.\n";
}

void tampilkanAntrean()
{
    if (!front)
    {
        cout << "Antrean kosong.\n";
        return;
    }
    Pelanggan *curr = front;
    int nomor = 1;
    while (curr)
    {
        cout << nomor++ << ". " << curr->nama << "\n";
        curr = curr->next;
    }
}

void dequeuePelanggan()
{
    if (!front)
    {
        cout << "Antrean kosong.\n";
        return;
    }
    Pelanggan *hapus = front;
    cout << "Memanggil: " << hapus->nama << "\n";
    front = front->next;
    if (!front)
        rear = nullptr;
    delete hapus;
}

void simpanDataPesanan()
{
    ofstream fout("pesanan.txt");
    if (!fout)
    {
        cout << "Gagal membuka file pesanan.txt untuk disimpan.\n";
        return;
    }
    Pesanan *curr = headPesanan;
    while (curr)
    {
        fout << curr->idPesanan << "|" << curr->idRoti << "|" << curr->nama << "|"
             << curr->jumlah << "|" << curr->totalHarga << "\n";
        curr = curr->next;
    }
    fout.close();
    cout << "Data pesanan berhasil disimpan.\n";
}

void tambahPesanan()
{
    int idRoti, jumlah;
    string nama;
    cout << "ID Roti: ";
    cin >> idRoti;
    Roti *roti = cariRotiById(idRoti);
    if (!roti)
    {
        cout << "Roti tidak ditemukan.\n";
        return;
    }
    cout << "Nama Pelanggan: ";
    cin.ignore();
    getline(cin, nama);
    cout << "Jumlah: ";
    cin >> jumlah;

    if (roti->stok < jumlah)
    {
        cout << "Stok tidak cukup.\n";
        return;
    }

    roti->stok -= jumlah;
    Pesanan *baru = new Pesanan{nextIdPesanan++, idRoti, nama, jumlah, jumlah * roti->harga, headPesanan};
    headPesanan = baru;
    cout << "Pesanan ditambahkan.\n";
}

void editPesanan()
{
    int id;
    cout << "ID Pesanan: ";
    cin >> id;
    Pesanan *p = headPesanan;
    while (p && p->idPesanan != id)
        p = p->next;
    if (!p)
    {
        cout << "Pesanan tidak ditemukan.\n";
        return;
    }
    cout << "Nama baru: ";
    cin.ignore();
    getline(cin, p->nama);
    cout << "Jumlah baru: ";
    cin >> p->jumlah;
    Roti *roti = cariRotiById(p->idRoti);
    p->totalHarga = p->jumlah * roti->harga;
    cout << "Pesanan diperbarui.\n";
}

void hapusPesanan()
{
    int id;
    cout << "ID Pesanan: ";
    cin >> id;
    Pesanan *curr = headPesanan, *prev = nullptr;
    while (curr && curr->idPesanan != id)
    {
        prev = curr;
        curr = curr->next;
    }
    if (!curr)
    {
        cout << "Pesanan tidak ditemukan.\n";
        return;
    }
    if (!prev)
        headPesanan = curr->next;
    else
        prev->next = curr->next;
    delete curr;
    cout << "Pesanan dihapus.\n";
}

void tampilkanPesanan()
{
    if (!headPesanan)
    {
        cout << "Tidak ada pesanan.\n";
        return;
    }
    Pesanan *curr = headPesanan;
    while (curr)
    {
        cout << "ID: " << curr->idPesanan << " | Roti: " << curr->idRoti
             << " | Nama: " << curr->nama << " | Jumlah: " << curr->jumlah
             << " | Total: " << curr->totalHarga << "\n";
        curr = curr->next;
    }
}

void muatDataPesanan()
{
    ifstream fin("pesanan.txt");
    if (!fin)
    {
        cout << "File pesanan.txt tidak ditemukan.\n";
        return;
    }
    int idPesanan, idRoti, jumlah, totalHarga;
    string nama;
    while (fin >> idPesanan >> idRoti >> nama >> jumlah >> totalHarga)
    {
        Pesanan *baru = new Pesanan{idPesanan, idRoti, nama, jumlah, totalHarga, headPesanan};
        headPesanan = baru;
        if (idPesanan >= nextIdPesanan)
            nextIdPesanan = idPesanan + 1;
    }
    fin.close();
    cout << "Data pesanan berhasil dimuat.\n";
}

void simpanRiwayatRec(PenjualanNode *node, ofstream &fout)
{
    if (!node)
        return;
    simpanRiwayatRec(node->left, fout);
    fout << node->idTransaksi << "|" << node->nama << "|" << node->totalHarga << "|" << node->waktu << "\n";
    simpanRiwayatRec(node->right, fout);
}

void pushTransaksi(Transaksi *transaksi)
{
    transaksi->next = topTransaksi;
    topTransaksi = transaksi;
}

Transaksi *popTransaksi()
{
    if (!topTransaksi)
    {
        cout << "Tidak ada transaksi untuk di-undo.\n";
        return nullptr;
    }
    Transaksi *hapus = topTransaksi;
    topTransaksi = topTransaksi->next;
    cout << "Undo transaksi ID " << hapus->idTransaksi << " berhasil.\n";
    return hapus;
}

int hitungDiskon(int totalQty, int totalHarga)
{
    if (totalQty > 10)
        return totalHarga * 0.10; // diskon 10%
    return 0;
}

void prosesPembelian()
{
    if (!front)
    {
        cout << "Tidak ada pelanggan dalam antrean.\n";
        return;
    }
    string namaPelanggan = front->nama;
    dequeuePelanggan();

    vector<ItemKeranjang> keranjang;
    int totalQty = 0, totalHarga = 0, id, qty;
    char lanjut;

    do
    {
        tampilkanDaftarRoti();
        cout << "Pilih ID Roti: ";
        cin >> id;
        Roti *roti = cariRotiById(id);
        if (!roti)
        {
            cout << "Roti tidak ditemukan.\n";
            continue;
        }
        cout << "Jumlah: ";
        cin >> qty;
        if (qty > roti->stok)
        {
            cout << "Stok tidak cukup.\n";
            continue;
        }
        roti->stok -= qty;
        keranjang.push_back({id, roti->nama, qty, roti->harga});
        totalQty += qty;
        totalHarga += qty * roti->harga;

        cout << "Tambah item lain? (y/n): ";
        cin >> lanjut;
    } while (lanjut == 'y' || lanjut == 'Y');

    int totalBayar = totalHarga - hitungDiskon(totalQty, totalHarga);
    time_t now = time(0);
    string waktu = ctime(&now);
    Transaksi *baru = new Transaksi{nextIdTransaksi++, namaPelanggan, keranjang, totalBayar, waktu, nullptr};
    pushTransaksi(baru);
    rootPenjualan = insertPenjualanNode(rootPenjualan, baru);
    cout << "Transaksi berhasil!\n";
}

PenjualanNode *insertPenjualanNode(PenjualanNode *root, Transaksi *transaksi)
{
    if (!root)
    {
        PenjualanNode *node = new PenjualanNode{transaksi->idTransaksi, transaksi->nama,
                                                transaksi->totalHarga, transaksi->waktu, nullptr, nullptr};
        return node;
    }
    if (transaksi->idTransaksi < root->idTransaksi)
        root->left = insertPenjualanNode(root->left, transaksi);
    else
        root->right = insertPenjualanNode(root->right, transaksi);
    return root;
}

void simpanRiwayat()
{
    ofstream fout("penjualan.txt");
    if (!fout)
    {
        cout << "Gagal membuka file penjualan.txt.\n";
        return;
    }
    simpanRiwayatRec(rootPenjualan, fout);
    fout.close();
    cout << "Riwayat penjualan berhasil disimpan.\n";
}

void muatDataPenjualan()
{
    ifstream fin("penjualan.txt");
    if (!fin)
    {
        cout << "File penjualan.txt tidak ditemukan.\n";
        return;
    }
    int id, total;
    string nama, waktu;
    while (fin >> id >> nama >> total)
    {
        getline(fin, waktu); // sisa line = waktu
        Transaksi *dummy = new Transaksi{id, nama, {}, total, waktu, nullptr};
        rootPenjualan = insertPenjualanNode(rootPenjualan, dummy);
        delete dummy;
        if (id >= nextIdTransaksi)
            nextIdTransaksi = id + 1;
    }
    fin.close();
    cout << "Data penjualan berhasil dimuat.\n";
}

void inorderBST(PenjualanNode *root)
{
    if (!root)
        return;
    inorderBST(root->left);
    cout << "ID: " << root->idTransaksi << " | Nama: " << root->nama
         << " | Total: " << root->totalHarga << " | Waktu: " << root->waktu;
    inorderBST(root->right);
}

void statistikPenjualan()
{
    if (!rootPenjualan)
    {
        cout << "Belum ada data penjualan.\n";
        return;
    }
    cout << "=== Laporan Penjualan ===\n";
    inorderBST(rootPenjualan);
}

void menuRoti()
{
    int pilihan;
    do
    {
        clearScreen();
        cout << "=== Menu Roti ===\n";
        cout << "1. Tambah Roti\n";
        cout << "2. Edit Roti\n";
        cout << "3. Hapus Roti\n";
        cout << "4. Tampilkan Daftar Roti\n";
        cout << "5. Sorting Roti by Harga\n";
        cout << "0. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            tambahRoti();
            pause();
            break;
        case 2:
            editRoti();
            pause();
            break;
        case 3:
            hapusRoti();
            pause();
            break;
        case 4:
            tampilkanDaftarRoti();
            pause();
            break;
        case 5:
            sortingRotiByHarga();
            pause();
            break;
        }
    } while (pilihan != 0);
}

void menuPesanan()
{
    int pilihan;
    do
    {
        clearScreen();
        cout << "=== Menu Pesanan ===\n";
        cout << "1. Tambah Pesanan\n";
        cout << "2. Edit Pesanan\n";
        cout << "3. Hapus Pesanan\n";
        cout << "4. Tampilkan Semua Pesanan\n";
        cout << "0. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            tambahPesanan();
            pause();
            break;
        case 2:
            editPesanan();
            pause();
            break;
        case 3:
            hapusPesanan();
            pause();
            break;
        case 4:
            tampilkanPesanan();
            pause();
            break;
        }
    } while (pilihan != 0);
}

void menuAntrian()
{
    int pilihan;
    do
    {
        clearScreen();
        cout << "=== Menu Antrean ===\n";
        cout << "1. Tambah Antrean\n";
        cout << "2. Lihat Antrean\n";
        cout << "3. Panggil Antrean Berikutnya\n";
        cout << "0. Kembali\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            enqueuePelanggan();
            pause();
            break;
        case 2:
            tampilkanAntrean();
            pause();
            break;
        case 3:
            dequeuePelanggan();
            pause();
            break;
        }
    } while (pilihan != 0);
}

void menuUtama()
{
    int pilihan;
    do
    {
        clearScreen();
        cout << "=== Sistem Kasir Toko Roti ===\n";
        cout << "1. Kelola Data Roti\n";
        cout << "2. Kelola Pesanan\n";
        cout << "3. Kelola Antrean\n";
        cout << "4. Proses Pembelian\n";
        cout << "5. Undo Transaksi\n";
        cout << "6. Statistik Penjualan\n";
        cout << "7. Simpan Data\n";
        cout << "8. Muat Data\n";
        cout << "0. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;

        switch (pilihan)
        {
        case 1:
            menuRoti();
            pause();
            break;
        case 2:
            menuPesanan();
            pause();
            break;
        case 3:
            menuAntrian();
            pause();
            break;
        case 4:
            prosesPembelian();
            pause();
            break;
        case 5:
            popTransaksi();
            pause();
            break;
        case 6:
            statistikPenjualan();
            pause();
            break;
        case 7:
            simpanDataRoti();
            simpanDataPesanan();
            simpanRiwayat();
            pause();
            break;
        case 8:
            muatDataRoti();
            muatDataPesanan();
            muatDataPenjualan();
            pause();
            break;
        }
    } while (pilihan != 0);
}

int main()
{
    muatDataRoti();
    muatDataPesanan();
    muatDataPenjualan();
    menuUtama();
    simpanDataRoti();
    simpanDataPesanan();
    simpanRiwayat();
    return 0;
}