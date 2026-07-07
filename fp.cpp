#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <cctype> 
#include <sstream>

using namespace std;

void registerUser();
void loginUser();
void simpanDataKeFile(); 

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
        cout << R"(

   _____ ______ _____  __      _______  _____ ______ 
  / ____|  ____|  __ \ \ \    / /_   _|/ ____|  ____|
 | (___ | |__  | |__) | \ \  / /  | | | |    | |__   
  \___ \|  __| |  _  /   \ \/ /   | | | |    |  __|  
  ____) | |____| | \ \    \  /   _| |_| |____| |____ 
 |_____/|______|_|  \_\    \/   |_____|\_____|______|

=======================================================
               SISTEM MANAJEMEN SERVIS PC
=======================================================
                 MENU AUTENTIKASI
-------------------------------------------------------
  [1] Login
  [2] Register
  [3] Keluar
-------------------------------------------------------
)";
cout << "Masukkan Pilihan : ";

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
        simpanDataKeFile();
        cout << "\nUndo Berhasil! Data dikembalikan dan tersimpan.\n";
    }
}

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
// FITUR UTAMA (CRUD, SEARCHING, SORTING, FILE, CETAK NOTA)
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
    simpanDataKeFile();
    cout << "\nPesanan berhasil dicatat dan otomatis tersimpan!\n";
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

// ==========================================================
// VERSI BARU: updatePesanan()
// Perubahan:
// - Tidak lagi memilih field mana yang mau diubah lewat menu (1/2/3)
// - User akan ditanya SEMUA field secara berurutan: Nama -> Detail -> Status
// - Kalau tidak mau ubah field tertentu, tinggal tekan Enter (kosongkan) untuk skip
// ==========================================================
void updatePesanan() {
    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang ingin diupdate: ");
    int index = -1;

    // Cari dulu posisi (index) data berdasarkan ID yang diinput
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == idCari) {
            index = i;
            break;
        }
    }

    // Kalau ID tidak ketemu, langsung berhenti
    if (index == -1) {
        cout << "ID tidak ditemukan.\n";
        return;
    }

    // Buang sisa newline ('\n') yang tertinggal di buffer setelah cin >> pada ambilInputAngka()
    // Ini WAJIB ada, kalau tidak, getline() pertama akan langsung terbaca kosong
    cin.ignore();

    cout << "\n--- UPDATE DATA PESANAN (ID: " << daftarPesanan[index].id_pesanan << ") ---\n";
    cout << "Kosongkan (langsung tekan Enter) jika field tidak ingin diubah.\n";

    string inputBaru;

    // ---------------------------------------------------
    // 1. UPDATE NAMA PELANGGAN
    // ---------------------------------------------------
    cout << "\nNama saat ini   : " << daftarPesanan[index].nama_pelanggan << "\n";
    cout << "Nama baru       : ";
    getline(cin, inputBaru);

    if (!inputBaru.empty()) {
        daftarPesanan[index].nama_pelanggan = inputBaru;
        jadikanHurufBesar(daftarPesanan[index].nama_pelanggan);
        cout << "-> Nama berhasil diperbarui.\n";
    } else {
        cout << "-> Nama dilewati (tetap sama).\n";
    }

    // ---------------------------------------------------
    // 2. UPDATE DETAIL SERVIS
    // ---------------------------------------------------
    cout << "\nDetail saat ini : " << daftarPesanan[index].detail_servis << "\n";
    cout << "Detail baru     : ";
    getline(cin, inputBaru);

    if (!inputBaru.empty()) {
        daftarPesanan[index].detail_servis = inputBaru;
        jadikanHurufBesar(daftarPesanan[index].detail_servis);
        cout << "-> Detail berhasil diperbarui.\n";
    } else {
        cout << "-> Detail dilewati (tetap sama).\n";
    }

    // ---------------------------------------------------
    // 3. UPDATE STATUS SELESAI
    // ---------------------------------------------------
    cout << "\nStatus saat ini : "
         << (daftarPesanan[index].status_selesai ? "SELESAI" : "BELUM") << "\n";
    cout << "Ubah status jadi SELESAI? (ketik y untuk ya, Enter untuk lewati): ";
    getline(cin, inputBaru);

    if (!inputBaru.empty() && (inputBaru[0] == 'y' || inputBaru[0] == 'Y')) {
        daftarPesanan[index].status_selesai = true;
        cout << "-> Status berhasil diperbarui menjadi SELESAI.\n";
    } else {
        cout << "-> Status dilewati (tetap sama).\n";
    }

    // Simpan semua perubahan ke file sekali di akhir
    simpanDataKeFile();
    cout << "\nSemua perubahan (jika ada) telah disimpan ke file.\n";
}

void hapusPesanan() {
    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang dihapus: ");
    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == idCari) {
            pushUndo(daftarPesanan[i]);
            for (int j = i; j < jumlahPesanan - 1; j++) daftarPesanan[j] = daftarPesanan[j + 1];
            jumlahPesanan--;
            simpanDataKeFile();
            cout << "Pesanan berhasil dihapus dan tersimpan!\n"; return;
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

// FUNGSI BARU: MENU 10
void selesaikanPesananDanCetakNota() {
    if (jumlahPesanan == 0) {
        cout << "\nBelum ada data pesanan di sistem.\n";
        return;
    }

    int idCari = ambilInputAngka("\nMasukkan ID Pesanan yang sudah selesai: ");
    bool ketemu = false;

    for (int i = 0; i < jumlahPesanan; i++) {
        if (daftarPesanan[i].id_pesanan == idCari) {
            // 1. Ubah status menjadi selesai dan simpan
            daftarPesanan[i].status_selesai = true;
            simpanDataKeFile();

            // 2. Buat dan Cetak File Nota
            string namaFile = "Nota_Pesanan_" + to_string(daftarPesanan[i].id_pesanan) + ".txt";
            ofstream notaFile(namaFile);
            
if (notaFile.is_open()) {
    notaFile << "------------------------------------------\n";
    notaFile << "                 STARCOMP                 \n";
    notaFile << "      Jl. Amikom No. 1, Yogyakarta        \n";
    notaFile << "------------------------------------------\n";
    notaFile << " TGL : 02-07-2026      JAM : 12:15        \n";
    notaFile << "------------------------------------------\n";
    notaFile << " ID PESANAN  : " << daftarPesanan[i].id_pesanan << "\n";
    notaFile << " PELANGGAN   : " << daftarPesanan[i].nama_pelanggan << "\n";
    notaFile << " KASUS/SERVIS: " << daftarPesanan[i].detail_servis << "\n";
    notaFile << "------------------------------------------\n";
    notaFile << " STATUS      : SELESAI                  \n";
    notaFile << "------------------------------------------\n";
    notaFile << " Terima kasih telah berkunjung.           \n";
    notaFile << " PC siap untuk diambil.                   \n";
    notaFile << "------------------------------------------\n";
    notaFile.close();
    
    cout << "\n[BERHASIL] Status pesanan diubah menjadi SELESAI.\n";
    cout << "Nota model struk telah dicetak ke: " << namaFile << "\n";
} else {
                cout << "\n[GAGAL] File nota tidak dapat dibuat.\n";
            }
            
            ketemu = true;
            break;
        }
    }

    if (!ketemu) {
        cout << "ID Pesanan tidak ditemukan.\n";
    }
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
        cout << R"(

        .------------------------------------------------.
        |  ____________________________________________  |
        | |                                            | |
        | |        SISTEM SERVIS & RAKIT PC            | |
        | |____________________________________________| |
        |________________________________________________|
                     ||               ||
                     ||_______________||
                   __||_______________||__
                  |_______________________|

)";
        cout << "==============================================\n";
        cout << "   SISTEM MANAJEMEN RAKIT & SERVIS PC \n";
        cout << "==============================================\n";
        cout << "1. Terima Pesanan Servis Baru\n2. Tampilkan Daftar Pesanan Servis\n"
             << "3. Update Status / Detail Servis\n4. Batalkan/Hapus Pesanan\n"
             << "5. Cari Data Pelanggan (Linear Search)\n6. Urutkan Nama Pelanggan (Bubble Sort)\n"
             << "7. Pulihkan Pesanan Terhapus (Stack Undo)\n8. Antrian Pengerjaan (Queue)\n"
             << "9. Tampilkan Log Pengerjaan (Linked List)\n"
             << "10. Selesaikan Pesanan & Cetak Nota\n" // <--- MENU 10 DITAMBAHKAN
             << "0. Keluar & Simpan Data\n"
             << "----------------------------------------------\n";
        
        // Pilihan input diubah menjadi (0-10)
        pilihan = ambilInputAngka("Pilih menu operasional (0-10): ");

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
            
            // MENU 10 DIPANGGIL DI SINI
            case 10: 
                bersihkanLayar(); 
                selesaikanPesananDanCetakNota(); 
                catatLog("Menyelesaikan pesanan dan mencetak nota."); 
                jedaLayar(); 
                break;
                
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
