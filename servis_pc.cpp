#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <iomanip>

using namespace std;

void registerUser();
void loginUser();

// ==========================================================
// 1. STRUKTUR DATA DASAR & GLOBAL VARIABEL
//    (Anggota 1: struct + array)
// ==========================================================
struct PesananPC {
    int id_pesanan;
    string nama_pelanggan;
    string detail_servis;
    double biaya_servis;
    bool status_selesai;
};

const int MAX = 100;
PesananPC daftarPesanan[MAX];
int jumlahPesanan = 0;
const string FILE_NAME = "data_servis_pc.txt";

// 2. LINKED LIST (LOG AKTIVITAS)
//    (Anggota 5: linked list + integrasi sistem)
struct NodeLog { string pesan; NodeLog* next; };
NodeLog* headLog = NULL;

// 3. STACK (UNDO)
//    (Anggota 3: stack & queue)
struct StackUndo { PesananPC data[MAX]; int top; };
StackUndo stackHapus = {{}, -1};

// 4. QUEUE (ANTRIAN)
//    (Anggota 3: stack & queue)
struct QueueAntrian { PesananPC data[MAX]; int depan = -1, belakang = -1; };
QueueAntrian antrianServis;

// LOGIN & REGISTER (bonus: multi-user login)
const string FILE_USER = "user.txt";
bool sudahLogin = false;
string usernameAktif = "";

// Menu utama (sebelum login)
void menuUtama() {
    int pilih;

    while (!sudahLogin) {
        cout << "\n===== MENU UTAMA =====\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Keluar\n";
        cout << "Pilihan : ";

        if (!(cin >> pilih)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "\nInput harus berupa angka!\n";
            continue;
        }

        switch (pilih) {
            case 1: loginUser(); break;
            case 2: registerUser(); break;
            case 3: exit(0);
            default: cout << "\nMenu tidak tersedia!\n";
        }
    }
}

void registerUser() {
    string username, password;
    bool ada = false;

    cout << "\n===== REGISTER =====\n";
    cout << "Username (min 4 karakter) : ";
    cin >> username;
    cout << "Password (min 4 karakter) : ";
    cin >> password;

    if (username.length() < 4 || password.length() < 4) {
        cout << "\nUsername/Password minimal 4 karakter!\n";
        return;
    }

    ifstream baca(FILE_USER);
    string user, pass, baris;

    while (getline(baca, baris)) {
        stringstream ss(baris);
        getline(ss, user, '|');
        getline(ss, pass);
        if (user == username) { ada = true; break; }
    }
    baca.close();

    if (ada) {
        cout << "\nUsername sudah digunakan!\n";
        return;
    }

    ofstream tulis(FILE_USER, ios::app);
    tulis << username << "|" << password << endl;
    tulis.close();

    cout << "\nRegister berhasil!\n";
}

void loginUser() {
    while (true) {
        string username, password;

        cout << "\n===== LOGIN =====\n";
        cout << "Username : ";
        cin >> username;
        cout << "Password : ";
        cin >> password;

        ifstream baca(FILE_USER);
        if (!baca.is_open()) {
            cout << "\nDatabase user belum ada, silakan Register dahulu!\n";
            return;
        }

        string user, pass, baris;
        while (getline(baca, baris)) {
            stringstream ss(baris);
            getline(ss, user, '|');
            getline(ss, pass);
            if (user == username && pass == password) {
                sudahLogin = true;
                usernameAktif = username;
                cout << "\nLogin berhasil. Selamat datang, " << username << "!\n";
                baca.close();
                return;
            }
        }
        baca.close();
        cout << "\nUsername atau Password tidak sesuai!\n";
    }
}

// ==========================================================
// FUNGSI UTILITAS & VALIDASI INPUT
// ==========================================================
void bersihkanLayar() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void jedaLayar() {
    cout << "\n----------------------------------------------\nTekan Enter untuk kembali...";
    cin.ignore(); cin.get();
}

int ambilInputAngka(string teksPrompt) {
    int angka;
    while (true) {
        cout << teksPrompt;
        if (cin >> angka) return angka;
        cout << "[EROR] Input harus berupa angka!\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

double ambilInputBiaya(string teksPrompt) {
    double biaya;
    while (true) {
        cout << teksPrompt;
        if (cin >> biaya && biaya >= 0) return biaya;
        cout << "[EROR] Biaya harus berupa angka dan tidak boleh negatif!\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

void jadikanHurufBesar(string &teks) {
    for (size_t i = 0; i < teks.length(); i++) {
        teks[i] = toupper(teks[i]);
    }
}

string formatRupiah(double nilai) {
    ostringstream oss;
    oss << "Rp " << fixed << setprecision(0) << nilai;
    return oss.str();
}

// ==========================================================
// (Anggota 2: pointer untuk akses/manipulasi data)
// Mengembalikan alamat elemen array (pointer), bukan salinan,
// sehingga fungsi lain bisa langsung mengubah data lewat pointer.
// ==========================================================
PesananPC* cariPointerByID(int id) {
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == id) {
            return &daftarPesanan[i];
        }
    }
    return NULL;
}

// ==========================================================
// LINKED LIST — LOG AKTIVITAS BENGKEL
// ==========================================================
void catatLog(string pesan) {
    NodeLog* nodeBaru = new NodeLog{pesan, NULL};
    if (!headLog) headLog = nodeBaru;
    else {
        NodeLog* temp = headLog;
        while (temp->next) temp = temp->next;
        temp->next = nodeBaru;
    }
}

void tampilkanLog() {
    if (!headLog) { cout << "\nBelum ada riwayat.\n"; return; }
    cout << "\n--- RIWAYAT AKTIVITAS BENGKEL (LINKED LIST) ---\n";
    NodeLog* temp = headLog; int nomor = 1;
    while (temp) {
        cout << nomor++ << ". " << temp->pesan << "\n";
        temp = temp->next;
    }
}

// ==========================================================
// STACK — UNDO PESANAN TERHAPUS
// ==========================================================
void pushUndo(PesananPC p) {
    if (stackHapus.top < MAX - 1) stackHapus.data[++stackHapus.top] = p;
}

void popUndo() {
    if (stackHapus.top < 0) { cout << "\nTidak ada data yang bisa dipulihkan.\n"; return; }
    if (jumlahPesanan < MAX) {
        daftarPesanan[jumlahPesanan++] = stackHapus.data[stackHapus.top--];
        cout << "\nUndo Berhasil! Data dikembalikan.\n";
    }
}

// ==========================================================
// QUEUE — ANTRIAN PENGERJAAN
// ==========================================================
void tampilkanAntrian() {
    cout << "\n--- DAFTAR ANTRIAN AKTIF SAAT INI ---\n";
    if (antrianServis.depan == -1 || antrianServis.depan > antrianServis.belakang) {
        cout << "[Kosong] Tidak ada PC di dalam antrian kerja.\n";
    } else {
        int nomor = 1;
        for (int i = antrianServis.depan; i <= antrianServis.belakang; i++) {
            cout << nomor++ << ". [ID: " << antrianServis.data[i].id_pesanan << "] "
                 << antrianServis.data[i].nama_pelanggan << " ("
                 << antrianServis.data[i].detail_servis << ")\n";
        }
    }
    cout << "-------------------------------------\n";
}

void tambahKeAntrian(PesananPC p) {
    if (antrianServis.belakang == MAX - 1) { cout << "Antrian Penuh!\n"; return; }
    if (antrianServis.depan == -1) antrianServis.depan = 0;
    antrianServis.data[++antrianServis.belakang] = p;
    cout << "\nPesanan '" << p.nama_pelanggan << "' berhasil masuk antrian kerja.\n";
}

void kerjakanDariAntrian() {
    if (antrianServis.depan == -1 || antrianServis.depan > antrianServis.belakang) {
        cout << "\nAntrian kosong. Tidak ada yang bisa dikerjakan.\n"; return;
    }
    cout << "\n[PROSES] Mengerjakan PC milik: " << antrianServis.data[antrianServis.depan].nama_pelanggan << "\n";
    antrianServis.depan++;
}

// ==========================================================
// CRUD — DATA PESANAN
//    (Anggota 1: CRUD + struct + array)
// ==========================================================
void tambahPesanan() {
    if (jumlahPesanan >= MAX) { cout << "\nData penuh, tidak bisa menambah pesanan lagi.\n"; return; }
    PesananPC p;
    p.id_pesanan = (jumlahPesanan > 0) ? daftarPesanan[jumlahPesanan - 1].id_pesanan + 1 : 1;
    p.status_selesai = false;

    cout << "\n--- TERIMA PESANAN BARU ---\nNama Pelanggan: ";
    cin >> ws;
    getline(cin, p.nama_pelanggan);
    jadikanHurufBesar(p.nama_pelanggan);

    cout << "Detail Servis : ";
    getline(cin, p.detail_servis);
    jadikanHurufBesar(p.detail_servis);

    p.biaya_servis = ambilInputBiaya("Estimasi Biaya (Rp): ");

    daftarPesanan[jumlahPesanan++] = p;
    cout << "\nPesanan berhasil dicatat dengan ID " << p.id_pesanan << "!\n";
}

void tampilkanPesanan() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan.\n"; return; }
    cout << "\n================ DAFTAR PESANAN SERVIS ================\n";
    for (int i = 0; i < jumlahPesanan; i++) {
        cout << "ID: " << daftarPesanan[i].id_pesanan << " | " << daftarPesanan[i].nama_pelanggan
             << " | " << daftarPesanan[i].detail_servis
             << " | " << formatRupiah(daftarPesanan[i].biaya_servis)
             << " | " << (daftarPesanan[i].status_selesai ? "[SELESAI]" : "[BELUM]") << "\n";
    }
}

void updatePesanan() {
    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang ingin diupdate: ");

    // Menggunakan pointer agar perubahan langsung mengenai data asli di array
    PesananPC* p = cariPointerByID(idCari);
    if (p == NULL) { cout << "ID tidak ditemukan.\n"; return; }

    int pil = ambilInputAngka("Ubah (1: Nama, 2: Detail, 3: Biaya, 4: Status Selesai): ");
    cin.ignore();
    if (pil == 1) {
        cout << "Nama baru: "; getline(cin, p->nama_pelanggan);
        jadikanHurufBesar(p->nama_pelanggan);
    } else if (pil == 2) {
        cout << "Detail baru: "; getline(cin, p->detail_servis);
        jadikanHurufBesar(p->detail_servis);
    } else if (pil == 3) {
        p->biaya_servis = ambilInputBiaya("Biaya baru (Rp): ");
    } else if (pil == 4) {
        p->status_selesai = !p->status_selesai;
    } else {
        cout << "Pilihan tidak valid.\n"; return;
    }
    cout << "Data berhasil diperbarui!\n";
}

void hapusPesanan() {
    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang dihapus: ");
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == idCari) {
            pushUndo(daftarPesanan[i]);
            for (int j = i; j < jumlahPesanan - 1; j++) daftarPesanan[j] = daftarPesanan[j + 1];
            jumlahPesanan--;
            cout << "Pesanan berhasil dihapus! (bisa dipulihkan lewat menu Undo)\n";
            return;
        }
    }
    cout << "ID tidak ditemukan.\n";
}

// ==========================================================
// SEARCHING
//    (Anggota 4: searching & sorting)
// ==========================================================

// Linear Search berdasarkan nama pelanggan
void cariNamaLinear() {
    string keyword;
    bool ketemu = false;

    cout << "\nMasukkan Nama Pelanggan: ";
    cin >> ws;
    getline(cin, keyword);
    jadikanHurufBesar(keyword);

    cout << "\n--- HASIL PENCARIAN (LINEAR SEARCH) ---\n";
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].nama_pelanggan.find(keyword) != string::npos) {
            cout << "ID: " << daftarPesanan[i].id_pesanan
                 << " | " << daftarPesanan[i].nama_pelanggan
                 << " | " << daftarPesanan[i].detail_servis
                 << " | " << formatRupiah(daftarPesanan[i].biaya_servis) << "\n";
            ketemu = true;
        }
    }
    if (!ketemu) cout << "[!] Data dengan nama tersebut tidak ditemukan.\n";
}

// Binary Search berdasarkan ID.
// Karena array asli belum tentu terurut (misal setelah proses Undo),
// dibuat salinan indeks yang diurutkan dulu memakai Selection Sort,
// baru dilakukan Binary Search di atas salinan indeks tersebut.
void cariIDBiner() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan.\n"; return; }

    int idTarget = ambilInputAngka("\nMasukkan ID Pesanan yang dicari: ");

    int indexUrut[MAX];
    for (int i = 0; i < jumlahPesanan; i++) indexUrut[i] = i;

    // Selection Sort berdasarkan ID (hanya pada salinan indeks)
    for (int i = 0; i < jumlahPesanan - 1; i++) {
        int idxMin = i;
        for (int j = i + 1; j < jumlahPesanan; j++) {
            if (daftarPesanan[indexUrut[j]].id_pesanan < daftarPesanan[indexUrut[idxMin]].id_pesanan) {
                idxMin = j;
            }
        }
        swap(indexUrut[i], indexUrut[idxMin]);
    }

    // Binary Search pada indeks yang sudah terurut
    int kiri = 0, kanan = jumlahPesanan - 1;
    int hasil = -1;
    while (kiri <= kanan) {
        int tengah = (kiri + kanan) / 2;
        int idTengah = daftarPesanan[indexUrut[tengah]].id_pesanan;
        if (idTengah == idTarget) { hasil = indexUrut[tengah]; break; }
        else if (idTengah < idTarget) kiri = tengah + 1;
        else kanan = tengah - 1;
    }

    cout << "\n--- HASIL PENCARIAN (BINARY SEARCH) ---\n";
    if (hasil == -1) {
        cout << "[!] Pesanan dengan ID " << idTarget << " tidak ditemukan.\n";
    } else {
        PesananPC &p = daftarPesanan[hasil];
        cout << "ID: " << p.id_pesanan << " | " << p.nama_pelanggan
             << " | " << p.detail_servis << " | " << formatRupiah(p.biaya_servis)
             << " | " << (p.status_selesai ? "[SELESAI]" : "[BELUM]") << "\n";
    }
}

void menuCari() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan untuk dicari.\n"; jedaLayar(); return; }

    int opsi;
    do {
        bersihkanLayar();
        cout << "--- SUB-MENU PENCARIAN ---\n";
        cout << "1. Cari Berdasarkan Nama (Linear Search)\n";
        cout << "2. Cari Berdasarkan ID (Binary Search)\n";
        cout << "0. Kembali ke Menu Utama\n";
        opsi = ambilInputAngka("Pilih opsi (0-2): ");

        if (opsi == 1) { cariNamaLinear(); jedaLayar(); }
        else if (opsi == 2) { cariIDBiner(); jedaLayar(); }
        else if (opsi != 0) { cout << "Opsi tidak valid!\n"; jedaLayar(); }
    } while (opsi != 0);
}

// ==========================================================
// SORTING
//    (Anggota 4: searching & sorting)
// ==========================================================

// Bubble Sort berdasarkan nama pelanggan (A-Z)
void urutkanNamaBubble() {
    for (int i = 0; i < jumlahPesanan - 1; i++) {
        for (int j = 0; j < jumlahPesanan - i - 1; j++) {
            if (daftarPesanan[j].nama_pelanggan > daftarPesanan[j + 1].nama_pelanggan) {
                swap(daftarPesanan[j], daftarPesanan[j + 1]);
            }
        }
    }
    cout << "\nData berhasil diurutkan berdasarkan Nama (Bubble Sort).\n";
    tampilkanPesanan();
}

// Insertion Sort berdasarkan biaya servis (termurah ke termahal)
void urutkanBiayaInsertion() {
    for (int i = 1; i < jumlahPesanan; i++) {
        PesananPC kunci = daftarPesanan[i];
        int j = i - 1;
        while (j >= 0 && daftarPesanan[j].biaya_servis > kunci.biaya_servis) {
            daftarPesanan[j + 1] = daftarPesanan[j];
            j--;
        }
        daftarPesanan[j + 1] = kunci;
    }
    cout << "\nData berhasil diurutkan berdasarkan Biaya (Insertion Sort).\n";
    tampilkanPesanan();
}

void menuUrutkan() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan untuk diurutkan.\n"; jedaLayar(); return; }

    int opsi;
    bersihkanLayar();
    cout << "--- SUB-MENU PENGURUTAN ---\n";
    cout << "1. Urutkan Nama Pelanggan A-Z (Bubble Sort)\n";
    cout << "2. Urutkan Biaya Termurah-Termahal (Insertion Sort)\n";
    cout << "0. Batal\n";
    opsi = ambilInputAngka("Pilih opsi (0-2): ");

    if (opsi == 1) urutkanNamaBubble();
    else if (opsi == 2) urutkanBiayaInsertion();
    else if (opsi != 0) cout << "Opsi tidak valid!\n";
}

// ==========================================================
// FITUR BONUS: STATISTIK & CETAK NOTA
// ==========================================================
void tampilkanStatistik() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan.\n"; return; }

    int selesai = 0, belum = 0;
    double totalPendapatan = 0, totalSemua = 0;

    for (int i = 0; i < jumlahPesanan; i++) {
        totalSemua += daftarPesanan[i].biaya_servis;
        if (daftarPesanan[i].status_selesai) {
            selesai++;
            totalPendapatan += daftarPesanan[i].biaya_servis;
        } else {
            belum++;
        }
    }

    cout << "\n============= STATISTIK BENGKEL =============\n";
    cout << "Total pesanan          : " << jumlahPesanan << "\n";
    cout << "Sudah selesai           : " << selesai << "\n";
    cout << "Belum selesai           : " << belum << "\n";
    cout << "Pendapatan (selesai)    : " << formatRupiah(totalPendapatan) << "\n";
    cout << "Rata-rata biaya/pesanan : " << formatRupiah(totalSemua / jumlahPesanan) << "\n";
    cout << "===============================================\n";
}

void cetakNota() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan.\n"; return; }

    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang akan dicetak nota-nya: ");
    PesananPC* p = cariPointerByID(idCari);
    if (p == NULL) { cout << "ID tidak ditemukan.\n"; return; }

    cout << "\n================ NOTA SERVIS PC ================\n";
    cout << "ID Pesanan     : " << p->id_pesanan << "\n";
    cout << "Nama Pelanggan : " << p->nama_pelanggan << "\n";
    cout << "Detail Servis  : " << p->detail_servis << "\n";
    cout << "Biaya Servis   : " << formatRupiah(p->biaya_servis) << "\n";
    cout << "Status         : " << (p->status_selesai ? "SELESAI" : "DALAM PROSES") << "\n";
    cout << "==================================================\n";
    cout << "Terima kasih telah menggunakan jasa servis kami!\n";
}

// ==========================================================
// FILE HANDLING (fstream)
//    (Anggota 2: file handling)
// ==========================================================
void simpanDataKeFile() {
    ofstream file(FILE_NAME);
    if (!file.is_open()) return;
    for (int i = 0; i < jumlahPesanan; i++) {
        file << daftarPesanan[i].id_pesanan << "|" << daftarPesanan[i].nama_pelanggan << "|"
             << daftarPesanan[i].detail_servis << "|" << daftarPesanan[i].biaya_servis << "|"
             << daftarPesanan[i].status_selesai << "\n";
    }
    file.close();
}

void muatDataDariFile() {
    ifstream file(FILE_NAME);
    if (!file.is_open()) return;
    jumlahPesanan = 0;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        PesananPC p;
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);
        size_t p4 = line.find('|', p3 + 1);
        if (p1 != string::npos && p2 != string::npos && p3 != string::npos && p4 != string::npos) {
            p.id_pesanan = stoi(line.substr(0, p1));
            p.nama_pelanggan = line.substr(p1 + 1, p2 - p1 - 1);
            p.detail_servis = line.substr(p2 + 1, p3 - p2 - 1);
            p.biaya_servis = stod(line.substr(p3 + 1, p4 - p3 - 1));
            p.status_selesai = (line.substr(p4 + 1) == "1");
            if (jumlahPesanan < MAX) daftarPesanan[jumlahPesanan++] = p;
        }
    }
    file.close();
}

// ==========================================================
// MENU OPERASIONAL (SETELAH LOGIN)
// ==========================================================
bool masuk() {
    muatDataDariFile();
    catatLog("Sistem dijalankan oleh " + usernameAktif + ", data dimuat.");
    int pilihan;
    do {
        bersihkanLayar();
        cout << "==============================================\n";
        cout << "   SISTEM MANAJEMEN RAKIT & SERVIS PC \n";
        cout << "   User aktif: " << usernameAktif << "\n";
        cout << "==============================================\n";
        cout << "1. Terima Pesanan Servis Baru\n2. Tampilkan Daftar Pesanan Servis\n"
             << "3. Update Status / Detail / Biaya Servis\n4. Batalkan/Hapus Pesanan\n"
             << "5. Cari Data Pesanan (Linear/Binary Search)\n6. Urutkan Data (Bubble/Insertion Sort)\n"
             << "7. Pulihkan Pesanan Terhapus (Stack Undo)\n8. Antrian Pengerjaan (Queue)\n"
             << "9. Tampilkan Log Pengerjaan (Linked List)\n10. Statistik Bengkel\n"
             << "11. Cetak Nota Servis\n0. Keluar & Simpan Data\n"
             << "----------------------------------------------\n";

        pilihan = ambilInputAngka("Pilih menu operasional (0-11): ");

        switch (pilihan) {
            case 1: bersihkanLayar(); tambahPesanan(); catatLog("Tambah pesanan baru."); jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanPesanan(); catatLog("Melihat daftar pesanan."); jedaLayar(); break;
            case 3: bersihkanLayar(); updatePesanan(); catatLog("Update data pesanan."); jedaLayar(); break;
            case 4: bersihkanLayar(); hapusPesanan(); catatLog("Hapus data pesanan."); jedaLayar(); break;
            case 5: menuCari(); catatLog("Membuka menu pencarian data pesanan."); break;
            case 6: menuUrutkan(); catatLog("Mengurutkan data pesanan."); jedaLayar(); break;
            case 7: bersihkanLayar(); popUndo(); catatLog("Melakukan Undo data."); jedaLayar(); break;
            case 8: {
                bersihkanLayar();
                cout << "==============================================\n";
                cout << "             MEJA KERJA TEKNISI               \n";
                cout << "==============================================\n";

                tampilkanAntrian();

                cout << "\n[MENU OPERASIONAL QUEUE]\n";
                cout << "1. Masukkan PC ke antrian rakit/servis\n";
                cout << "2. Kerjakan PC terdepan di antrian\n";
                cout << "0. Kembali ke Menu Utama\n";

                int subPilihan = ambilInputAngka("Pilih tindakan (0-2): ");

                if (subPilihan == 1) {
                    if (jumlahPesanan == 0) {
                        cout << "Belum ada master data pesanan! Silakan isi Menu 1 terlebih dahulu.\n";
                    } else {
                        int id = ambilInputAngka("Masukkan ID Pesanan: ");
                        PesananPC* p = cariPointerByID(id);
                        if (p != NULL) {
                            tambahKeAntrian(*p);
                            catatLog("Memasukkan pesanan ID " + to_string(id) + " ke antrian.");
                        } else {
                            cout << "ID Tidak ditemukan!\n";
                        }
                    }
                    jedaLayar();
                } else if (subPilihan == 2) {
                    kerjakanDariAntrian();
                    catatLog("Memproses satu pesanan dari antrian depan.");
                    jedaLayar();
                }
                break;
            }
            case 9: bersihkanLayar(); tampilkanLog(); jedaLayar(); break;
            case 10: bersihkanLayar(); tampilkanStatistik(); catatLog("Melihat statistik bengkel."); jedaLayar(); break;
            case 11: bersihkanLayar(); cetakNota(); catatLog("Mencetak nota servis."); jedaLayar(); break;
            case 0: bersihkanLayar(); simpanDataKeFile(); cout << "Data tersimpan. Keluar sistem.\n"; break;
            default: cout << "Menu tidak tersedia.\n"; jedaLayar();
        }
    } while (pilihan != 0);
    return 0;
}

// ==========================================================
// MAIN
// ==========================================================
int main() {
    menuUtama();
    if (sudahLogin) {
        masuk();
    } else {
        cout << "\nAnda harus login terlebih dahulu untuk mengakses sistem.\n";
    }
    return 0;
}
