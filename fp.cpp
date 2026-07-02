#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <cctype> 
#include <sstream>

using namespace std;

void registerUser();
void loginUser();


// ==========================================================
// 1. STRUKTUR DATA DASAR & GLOBAL VARIABEL
// ==========================================================
struct PesananPC {
    int id_pesanan;
    string nama_pelanggan;
    string detail_servis;
    bool status_selesai;
};

const int MAX = 100;
PesananPC daftarPesanan[MAX]; 
int jumlahPesanan = 0;          
const string FILE_NAME = "data_servis_pc.txt";

// 2. LINKED LIST (LOG)
struct NodeLog { string pesan; NodeLog* next; };
NodeLog* headLog = NULL;

// 3. STACK (UNDO)
struct StackUndo { PesananPC data[MAX]; int top; };
StackUndo stackHapus = {{}, -1}; 

// 4. QUEUE (ANTRIAN)
struct QueueAntrian { PesananPC data[MAX]; int depan = -1, belakang = -1; };
QueueAntrian antrianServis;

// LOGIN & REGISTER
const string FILE_USER = "user.txt";
bool sudahLogin = false;

// Menu utama
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
            case 1:
                loginUser();
                break;

            case 2:
                registerUser();
                break;

            case 3:
                exit(0);

            default:
                cout << "\nMenu tidak tersedia!\n";
        }
    }
}


void registerUser() {
    string username, password;
    bool ada = false;

    cout << "\n===== REGISTER =====\n";
    cout << "Username : ";
    cin >> username;
    cout << "Password : ";
    cin >> password;

    ifstream baca(FILE_USER);

    string user, pass, baris;

    while (getline(baca, baris)) {
        stringstream ss(baris);

        getline(ss, user, '|');
        getline(ss, pass);

        if (user == username) {
            ada = true;
            break;
        }
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
            cout << "\nDatabase user belum ada!\n";
            return;
        }

        string user, pass, baris;

        while (getline(baca, baris)) {
            stringstream ss(baris);

            getline(ss, user, '|');
            getline(ss, pass);

            if (user == username && pass == password) {
                sudahLogin = true;
                cout << "\nLogin berhasil.\n";
                baca.close();
                return;
            }
        }

        baca.close();

        cout << "\nUsername atau Password tidak sesuai!\n";
    }
}


// ==========================================================
// FUNGSI UTILITAS & VALIDASI ANTI-HURUF
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
        cout << "[EROR] Input harus berupa angka/menu yang tersedia!\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

void jadikanHurufBesar(string &teks) {
    for (int i = 0; i < teks.length(); i++) {
        teks[i] = toupper(teks[i]);
    }
}

// ==========================================================
// FITUR STRUKTUR DATA (LINKED LIST, STACK, QUEUE)
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

// Tambahan: Fungsi untuk menampilkan isi antrian (Queue) secara realtime
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
// FITUR UTAMA (CRUD, SEARCHING, SORTING, FILE)
// ==========================================================
void tambahPesanan() {
    if (jumlahPesanan >= MAX) return;
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
    
    daftarPesanan[jumlahPesanan++] = p;
    cout << "\nPesanan berhasil dicatat!\n";
}

void tampilkanPesanan() {
    if (jumlahPesanan == 0) { cout << "\nBelum ada data pesanan.\n"; return; }
    cout << "\n================ DAFTAR PESANAN SERVIS ================\n";
    for (int i = 0; i < jumlahPesanan; i++) {
        cout << "ID: " << daftarPesanan[i].id_pesanan << " | " << daftarPesanan[i].nama_pelanggan 
             << " | " << daftarPesanan[i].detail_servis 
             << " | " << (daftarPesanan[i].status_selesai ? "[SELESAI]" : "[BELUM]") << "\n";
    }
}

void updatePesanan() {
    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang ingin diupdate: ");
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == idCari) {
            int pil = ambilInputAngka("Ubah (1: Nama, 2: Detail, 3: Status Selesai): ");
            cin.ignore();
            if (pil == 1) { 
                cout << "Nama baru: "; getline(cin, daftarPesanan[i].nama_pelanggan); 
                jadikanHurufBesar(daftarPesanan[i].nama_pelanggan); 
            }
            else if (pil == 2) { 
                cout << "Detail baru: "; getline(cin, daftarPesanan[i].detail_servis); 
                jadikanHurufBesar(daftarPesanan[i].detail_servis); 
            }
            else if (pil == 3) { daftarPesanan[i].status_selesai = !daftarPesanan[i].status_selesai; }
            cout << "Data berhasil diperbarui!\n"; return;
        }
    }
    cout << "ID tidak ditemukan.\n";
}

void hapusPesanan() {
    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang dihapus: ");
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == idCari) {
            pushUndo(daftarPesanan[i]);
            for (int j = i; j < jumlahPesanan - 1; j++) daftarPesanan[j] = daftarPesanan[j + 1];
            jumlahPesanan--;
            cout << "Pesanan berhasil dihapus!\n"; return;
        }
    }
    cout << "ID tidak ditemukan.\n";
}

void urutkanPesanan() {
    for (int i = 0; i < jumlahPesanan - 1; i++) {
        for (int j = 0; j < jumlahPesanan - i - 1; j++) {
            if (daftarPesanan[j].nama_pelanggan > daftarPesanan[j + 1].nama_pelanggan) {
                swap(daftarPesanan[j], daftarPesanan[j + 1]);
            }
        }
    }
    cout << "\nData berhasil diurutkan (A-Z).\n"; tampilkanPesanan();
}

void cariPesanan() {
    if (jumlahPesanan == 0) { 
        cout << "\nBelum ada data pesanan untuk dicari.\n"; 
        jedaLayar();
        return; 
    }

    int opsi;
    do {
        bersihkanLayar();
        cout << "--- SUB-MENU PENCARIAN ---\n";
        cout << "1. Cari Berdasarkan Nama\n";
        cout << "2. Tampilkan Semua Data (Jika Lupa Nama/ID)\n";
        cout << "0. Kembali ke Menu Utama\n";
        opsi = ambilInputAngka("Pilih opsi (0-2): ");

        if (opsi == 1) {
            string keyword;
            bool ketemu;
            
            do {
                cout << "\nMasukkan Nama Pelanggan (ketik '0' untuk batal): ";
                cin >> ws; 
                getline(cin, keyword);

                if (keyword == "0") break; 

                jadikanHurufBesar(keyword);
                ketemu = false;

                cout << "\n--- HASIL PENCARIAN ---\n";
                for (int i = 0; i < jumlahPesanan; i++) {
                    if (daftarPesanan[i].nama_pelanggan.find(keyword) != string::npos) {
                        cout << "ID: " << daftarPesanan[i].id_pesanan 
                             << " | " << daftarPesanan[i].nama_pelanggan 
                             << " | " << daftarPesanan[i].detail_servis << "\n";
                        ketemu = true;
                    }
                }

                if (!ketemu) {
                    cout << "[!] Data tidak ditemukan. Silakan cari lagi.\n";
                } else {
                    jedaLayar();
                    return; 
                }

            } while (!ketemu); 
            
        } else if (opsi == 2) {
            tampilkanPesanan();
            jedaLayar();
        } else if (opsi != 0) {
            cout << "Opsi tidak valid!\n";
            jedaLayar();
        }
    } while (opsi != 0); 
}

void simpanDataKeFile() {
    ofstream file(FILE_NAME);
    if (!file.is_open()) return;
    for (int i = 0; i < jumlahPesanan; i++) {
        file << daftarPesanan[i].id_pesanan << "|" << daftarPesanan[i].nama_pelanggan << "|"
             << daftarPesanan[i].detail_servis << "|" << daftarPesanan[i].status_selesai << "\n";
    }
    file.close();
}

void muatDataDariFile() {
    ifstream file(FILE_NAME);
    if (!file.is_open()) return;
    jumlahPesanan = 0; string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        PesananPC p;
        int p1 = line.find('|'), p2 = line.find('|', p1 + 1), p3 = line.find('|', p2 + 1);
        if (p1 != (int)string::npos && p2 != (int)string::npos && p3 != (int)string::npos) {
            p.id_pesanan = stoi(line.substr(0, p1));
            p.nama_pelanggan = line.substr(p1 + 1, p2 - p1 - 1);
            p.detail_servis = line.substr(p2 + 1, p3 - p2 - 1);
            p.status_selesai = (line.substr(p3 + 1) == "1");
            daftarPesanan[jumlahPesanan++] = p;
        }
    }
    file.close();
}

bool masuk() {
    muatDataDariFile();
    catatLog("Sistem dijalankan, data dimuat.");
    int pilihan;
    do {
        bersihkanLayar();
        cout << "==============================================\n";
        cout << "   SISTEM MANAJEMEN RAKIT & SERVIS PC \n";
        cout << "==============================================\n";
        cout << "1. Terima Pesanan Servis Baru\n2. Tampilkan Daftar Pesanan Servis\n"
             << "3. Update Status / Detail Servis\n4. Batalkan/Hapus Pesanan\n"
             << "5. Cari Data Pelanggan (Linear Search)\n6. Urutkan Nama Pelanggan (Bubble Sort)\n"
             << "7. Pulihkan Pesanan Terhapus (Stack Undo)\n8. Antrian Pengerjaan (Queue)\n"
             << "9. Tampilkan Log Pengerjaan (Linked List)\n0. Keluar & Simpan Data\n"
             << "----------------------------------------------\n";
        
        pilihan = ambilInputAngka("Pilih menu operasional (0-9): ");

        switch (pilihan) {
            case 1: bersihkanLayar(); tambahPesanan(); catatLog("Tambah pesanan baru."); jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanPesanan(); catatLog("Melihat daftar pesanan."); jedaLayar(); break;
            case 3: bersihkanLayar(); updatePesanan(); catatLog("Update data pesanan."); jedaLayar(); break;
            case 4: bersihkanLayar(); hapusPesanan(); catatLog("Hapus data pesanan."); jedaLayar(); break;
            case 5: cariPesanan(); catatLog("Membuka menu pencarian data pelanggan."); break;
            case 6: bersihkanLayar(); urutkanPesanan(); catatLog("Mengurutkan nama (A-Z)."); jedaLayar(); break;
            case 7: bersihkanLayar(); popUndo(); catatLog("Melakukan Undo data."); jedaLayar(); break;
            case 8: 
                bersihkanLayar();
                cout << "==============================================\n";
                cout << "             MEJA KERJA TEKNISI               \n";
                cout << "==============================================\n";
                
                // Menampilkan daftar antrian yang aktif saat ini secara otomatis
                tampilkanAntrian();
                
                cout << "\n[MENU OPERASIONAL QUEUE]\n";
                cout << "1. Masukkan PC ke antrian rakit/servis\n";
                cout << "2. Kerjakan PC terdepan di antrian\n";
                cout << "0. Kembali ke Menu Utama\n";
                
                int subPilihan;
                subPilihan = ambilInputAngka("Pilih tindakan (0-2): ");
                
                if (subPilihan == 1) {
                    if (jumlahPesanan == 0) { 
                        cout << "Belum ada master data pesanan! Silakan isi Menu 1 terlebih dahulu.\n"; 
                    } else {
                        int id = ambilInputAngka("Masukkan ID Pesanan: ");
                        bool ada = false;
                        for(int i = 0; i < jumlahPesanan; i++) {
                            if(daftarPesanan[i].id_pesanan == id) { 
                                tambahKeAntrian(daftarPesanan[i]); 
                                catatLog("Memasukkan pesanan ID " + to_string(id) + " ke antrian.");
                                ada = true; 
                                break; 
                            }
                        }
                        if(!ada) cout << "ID Tidak ditemukan!\n";
                    }
                    jedaLayar();
                } 
                else if (subPilihan == 2) {
                    kerjakanDariAntrian(); 
                    catatLog("Memproses satu pesanan dari antrian depan.");
                    jedaLayar();
                }
                break;
            case 9: bersihkanLayar(); tampilkanLog(); jedaLayar(); break;
            case 0: bersihkanLayar(); simpanDataKeFile(); cout << "Data tersimpan. Keluar sistem.\n"; break;
            default: cout << "Menu tidak tersedia.\n"; jedaLayar();
        }
    } while (pilihan != 0);
    return 0;
}

// ==========================================================
// MAIN MENU (FUNGSI UTAMA)
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
