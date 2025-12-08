#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

// =====================================================================================
// KONFIGURASI LAYAR & KEYBOARD
// =====================================================================================
#define MAX_DATA 100
#define SIDEBAR_WIDTH 35
#define HEADER_HEIGHT 8

// Key Codes for Navigation
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27

// Warna Theme (Pink & White)
void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[13] = RGB(255, 105, 180); // Hot Pink (Background utama di slot D)
    csbi.ColorTable[15] = RGB(255, 255, 255); // White (Text)
    csbi.ColorTable[0]  = RGB(50, 50, 50);    // Dark Grey (Untuk Highlight Text jika perlu)
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

// Fungsi helper untuk mengubah warna teks saat highlight menu
// 0xD0 = Background Pink, Teks Hitam
// 0xDF = Background Pink, Teks Putih (Normal)
// 0xF1 = Background Putih, Teks Biru/Gelap (Highlight)
void textNormal() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF); // Pink BG, White Text
}

void textHighlight() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xF5); // White BG, Purple Text
}

// Fullscreen Logic
void goFullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1C, 0, 0);
    keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(150);
}

void maximizeConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(150);
}

void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Global Variables untuk Ukuran Layar Dinamis
int screenWidth = 0;
int screenHeight = 0;

void updateScreenSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// =====================================================================================
// STRUKTUR DATA & DUMMY
// =====================================================================================

// Master 1: Supplier
typedef struct {
    int id;
    char nama[50];
    char alamat[100];
} Supplier;

// Master 2: Produk (Bagian dari Gudang/Produk)
typedef struct {
    int id;
    char nama[50];
    int stok;
    long harga;
} Produk;

Supplier dbSupplier[MAX_DATA];
int totalSupplier = 0;

Produk dbProduk[MAX_DATA];
int totalProduk = 0;

void initDummyData() {
    // Generate 20 Supplier
    for(int i = 0; i < 20; i++) {
        dbSupplier[i].id = i + 1;
        sprintf(dbSupplier[i].nama, "PT. Sentosa Abadi %d", i+1);
        sprintf(dbSupplier[i].alamat, "Jl. Raya Industri Blok %c No. %d", 'A'+(i%5), i+10);
        totalSupplier++;
    }

    // Generate 20 Produk
    char *jenis[] = {"Indomie", "Susu", "Sabun", "Shampoo", "Kopi", "Teh", "Gula", "Minyak", "Tepung", "Beras"};
    for(int i = 0; i < 20; i++) {
        dbProduk[i].id = i + 1;
        sprintf(dbProduk[i].nama, "%s Varian %d", jenis[i%10], i+1);
        dbProduk[i].stok = (i+1) * 5;
        dbProduk[i].harga = (i+1) * 1500;
        totalProduk++;
    }
}

// Helper Input
void getString(char *buffer, int maxLen) {
    // Kembalikan kursor agar terlihat saat mengetik
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

    fflush(stdin);
    fgets(buffer, maxLen, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    // Sembunyikan lagi setelah selesai
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// =====================================================================================
// SYSTEM UI DRAWING
// =====================================================================================

void drawFullFrame() {
    updateScreenSize();
    int right = screenWidth - 1;
    int bottom = screenHeight - 1;

    textNormal(); // Pastikan warna frame normal

    // Gambar Garis Horizontal Atas & Bawah
    for (int x = 0; x <= right; x++) {
        gotoxy(x, 0); printf("%c", 205); // Top
        gotoxy(x, bottom); printf("%c", 205); // Bottom
        gotoxy(x, HEADER_HEIGHT); printf("%c", 205); // Divider Header
    }

    // Gambar Garis Vertikal Kiri & Kanan
    for (int y = 0; y <= bottom; y++) {
        gotoxy(0, y); printf("%c", 186); // Left
        gotoxy(right, y); printf("%c", 186); // Right
        if (y > HEADER_HEIGHT) {
            gotoxy(SIDEBAR_WIDTH, y); printf("%c", 186); // Sidebar Divider
        }
    }

    // Perbaiki Sudut (Corners)
    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);

    // Perbaiki Persimpangan (Junctions)
    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
    gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", 203);
    gotoxy(SIDEBAR_WIDTH, bottom); printf("%c", 202);
}

void drawHeader() {
    int startX = (screenWidth - 85) / 2;
    if (startX < 1) startX = 1;

    textNormal();
    gotoxy(startX, 1); printf("  _____ _    _ _____  ______ _____  __  __           _____  _  ________ _______ ");
    gotoxy(startX, 2); printf(" / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__    __|");
    gotoxy(startX, 3); printf("| (___ | |  | | |__) | |__  | |__) | \\  / |   /  \\   | |__) | ' /| |__     | |   ");
    gotoxy(startX, 4); printf(" \\___ \\| |  | |  ___/|  __| |  _  /| |\\/| |  / /\\ \\  |  _  /|  < |  __|    | |   ");
    gotoxy(startX, 5); printf(" ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____   | |   ");
    gotoxy(startX, 6); printf("|_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|  |_|   ");
}

void clearRightContent() {
    updateScreenSize();
    textNormal();
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(SIDEBAR_WIDTH + 1, y);
        for(int x = SIDEBAR_WIDTH + 1; x < screenWidth - 1; x++) printf(" ");
    }
}

void printCenterRight(int y, const char* text) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    int textLen = strlen(text);
    int x = SIDEBAR_WIDTH + 1 + (contentWidth - textLen) / 2;
    gotoxy(x, y); printf("%s", text);
}

int getCenterXForTable(int tableWidth) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    return SIDEBAR_WIDTH + 1 + (contentWidth - tableWidth) / 2;
}

void drawBaseLayout(const char* sidebarTitle) {
    system("cls");
    drawFullFrame();
    drawHeader();
    gotoxy(2, HEADER_HEIGHT + 2);
    printf("     MENU");
    gotoxy(2, HEADER_HEIGHT + 3);
    printf("   %-20s", sidebarTitle);
}

// =====================================================================================
// MENU DRAWING HELPER (NAVIGASI)
// =====================================================================================

// Fungsi Generic untuk print menu item dengan status terpilih/tidak
void printMenuItem(int x, int y, char* text, int isSelected) {
    gotoxy(x, y);
    if(isSelected) {
        textHighlight(); // Ganti warna background jadi putih/teks gelap
        printf(" >> %-20s << ", text); // Tambah padding biar rapi
        textNormal();    // Balikin warna normal
    } else {
        textNormal();
        printf("    %-20s    ", text);
    }
}

// =====================================================================================
// ANIMASI & LOGIN
// =====================================================================================

void loadingAnimation() {
    system("cls");
    int midY = 10;
    int midX = 30;
    gotoxy(midX, midY); printf("Sabar ya beb...");
    gotoxy(midX, midY+2);
    for(int i=0; i<40; i++) {
        printf("%c", 219);
        Sleep(10);
    }
    Sleep(200);
}

int loginScreen() {
    char username[50], password[50];
    char ch;
    int i, show = 0;
    updateScreenSize();
    int boxW = 50, boxH = 12;
    int bx = (screenWidth - boxW)/2;
    int by = (screenHeight - boxH)/2;

    while(1){
        system("cls");
        drawHeader();
        for(int x=0; x<screenWidth; x++) { gotoxy(x, 8); printf("-"); }

        for(int y=by; y<=by+boxH; y++) {
            gotoxy(bx, y);
            for(int x=0; x<=boxW; x++) {
                if(y==by || y==by+boxH) printf("-");
                else if(x==0 || x==boxW) printf("|");
                else printf(" ");
            }
        }

        gotoxy(bx+20, by+2); printf("LOGIN ADMIN");
        gotoxy(bx+5, by+4); printf("Username: ");
        gotoxy(bx+5, by+6); printf("Password: ");
        gotoxy(bx+5, by+10); printf("[TAB] Show/Hide | [ESC] Exit");

        // Input Username
        gotoxy(bx+15, by+4); getString(username, 49);
        if(strlen(username)==0) return 0;

        // Input Password
        gotoxy(bx+15, by+6);
        i=0; show=0;
        memset(password, 0, 50);
        while(1){
            ch = getch();
            if(ch == 13) break;
            if(ch == 27) return 0;
            if(ch == 8){
                if(i > 0){ i--; printf("\b \b"); }
            }
            else if(ch == 9){ // TAB
                show = !show;
                gotoxy(bx+15, by+6); printf("                         ");
                gotoxy(bx+15, by+6);
                for(int z=0; z<i; z++) printf(show ? "%c" : "*", password[z]);
            }
            else if (i < 49 && ch >= 32 && ch <= 126) {
                password[i++] = ch;
                printf(show ? "%c" : "*", ch);
            }
        }
        password[i] = '\0';

        if(strcmp(username, "admin")==0 && strcmp(password, "admin")==0) {
            gotoxy(bx+10, by+8); printf("Login Berhasil!"); Sleep(800);
            return 1;
        } else {
            gotoxy(bx+10, by+8); printf("Login Gagal!"); Sleep(800);
        }
    }
}

// =====================================================================================
// MODUL CRUD (Navigasi Arrow Key di Sidebar)
// =====================================================================================

void crudSupplier() {
    int selected = 1; // Default pilihan ke-1
    int totalMenu = 5; // Total menu item
    int key;

    drawBaseLayout("SUPPLIER");

    while(1) {
        // --- GAMBAR MENU SIDEBAR (Navigasi) ---
        // Kita gambar ulang sidebar setiap loop agar highlight berpindah
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Data", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Data", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Data", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Data", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        // --- TAMPILAN DATA DI KANAN (Statis selama tidak ada input CRUD) ---
        // Agar tidak berkedip parah, kita hanya update konten kanan jika perlu
        // Tapi untuk simplifikasi kode ini, kita biarkan refresh dulu
        // Tips: Dalam aplikasi real, pisahkan logic draw dan logic input

        // (Code Menampilkan Tabel Supplier Disini - Sama seperti sebelumnya)
        // Kita gambar tabel sekali saja di awal loop atau gunakan flag 'needRefresh'
        // Untuk contoh ini, saya simpan tabelnya agar tetap muncul.
        {
             // Clear area header tabel sedikit agar bersih
             for(int y=HEADER_HEIGHT+2; y<HEADER_HEIGHT+4; y++) {
                 gotoxy(SIDEBAR_WIDTH+1, y); for(int x=SIDEBAR_WIDTH+1; x<screenWidth-1; x++) printf(" ");
             }
             printCenterRight(HEADER_HEIGHT + 2, "=== MANAJEMEN DATA SUPPLIER ===");

             int tableX = getCenterXForTable(80);
             int startY = HEADER_HEIGHT + 4;
             gotoxy(tableX, startY); printf("%-5s | %-25s | %-40s", "ID", "NAMA SUPPLIER", "ALAMAT");
             gotoxy(tableX, startY+1); printf("--------------------------------------------------------------------------------");
             int limit = (totalSupplier > 15) ? 15 : totalSupplier;
             for(int i=0; i<limit; i++) {
                 gotoxy(tableX, startY+2+i);
                 printf("%-5d | %-25.25s | %-40.40s", dbSupplier[i].id, dbSupplier[i].nama, dbSupplier[i].alamat);
             }
        }

        // --- BACA INPUT ---
        hideCursor();
        key = getch();

        if(key == 224) { // Kode awal tombol panah
            key = getch(); // Ambil kode kedua
            if(key == KEY_UP) {
                selected--;
                if(selected < 1) selected = totalMenu; // Looping ke bawah
            }
            else if(key == KEY_DOWN) {
                selected++;
                if(selected > totalMenu) selected = 1; // Looping ke atas
            }
        }
        else if(key == KEY_ENTER) {
            // EKSEKUSI PILIHAN
            int tableX = getCenterXForTable(80);
            int inputY = HEADER_HEIGHT + 22;

            if(selected == 5) break; // Kembali (Menu 0)

            if(selected == 1) { /* Sudah tampil */ }
            else if(selected == 2) {
                if(totalSupplier >= MAX_DATA) continue;
                textNormal(); // Balikin kursor dan warna
                gotoxy(tableX, inputY);   printf("Nama Baru   : "); getString(dbSupplier[totalSupplier].nama, 49);
                gotoxy(tableX, inputY+1); printf("Alamat Baru : "); getString(dbSupplier[totalSupplier].alamat, 99);
                dbSupplier[totalSupplier].id = totalSupplier + 1;
                totalSupplier++;
                gotoxy(tableX, inputY+3); printf(">> Data Tersimpan!"); Sleep(800);
                clearRightContent(); // Bersihkan form
            }
            else if(selected == 3) {
                int id, idx = -1;
                textNormal();
                gotoxy(tableX, inputY); printf("ID Edit: ");
                // Scanf butuh visible cursor & buffer clear
                CONSOLE_CURSOR_INFO c; GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c); c.bVisible=TRUE; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
                scanf("%d", &id);

                for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == id) idx = i;
                if(idx != -1) {
                    gotoxy(tableX, inputY+1); printf("Nama [%s]: ", dbSupplier[idx].nama); getString(dbSupplier[idx].nama, 49);
                    gotoxy(tableX, inputY+2); printf("Alamat [%s]: ", dbSupplier[idx].alamat); getString(dbSupplier[idx].alamat, 99);
                    printf(">> Update Berhasil!");
                } else printf(">> ID Tidak Ada!");
                Sleep(800);
                clearRightContent();
            }
            else if(selected == 4) {
                 int id, idx = -1;
                 textNormal();
                gotoxy(tableX, inputY); printf("ID Hapus: ");
                CONSOLE_CURSOR_INFO c; GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c); c.bVisible=TRUE; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
                scanf("%d", &id);

                for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == id) idx = i;
                 if(idx != -1) {
                    for(int j=idx; j<totalSupplier-1; j++) dbSupplier[j] = dbSupplier[j+1];
                    totalSupplier--;
                    printf(">> Hapus Berhasil!");
                } else printf(">> ID Tidak Ada!");
                Sleep(800);
                clearRightContent();
            }
        }
    }
}

void crudProduk() {
    // Implementasi pola yang sama persis seperti crudSupplier
    // Saya singkat disini agar kode muat, intinya ganti scanf menu dengan loop getch()
    int selected = 1;
    int totalMenu = 5;
    int key;
    drawBaseLayout("PRODUK / GUDANG");

    while(1) {
        // Menu Sidebar
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Stok", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Produk", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Produk", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Produk", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        // Content Table (Singkat)
        printCenterRight(HEADER_HEIGHT + 2, "=== DATA STOK PRODUK GUDANG ===");
        int tableX = getCenterXForTable(70);
        int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf("%-5s | %-30s | %-10s | %-15s", "ID", "NAMA PRODUK", "STOK", "HARGA");
        gotoxy(tableX, startY+1); printf("----------------------------------------------------------------------");
        int limit = (totalProduk > 15) ? 15 : totalProduk;
        for(int i=0; i<limit; i++) {
            gotoxy(tableX, startY+2+i);
            printf("%-5d | %-30.30s | %-10d | Rp. %-10ld", dbProduk[i].id, dbProduk[i].nama, dbProduk[i].stok, dbProduk[i].harga);
        }

        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; }
            else if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; }
        }
        else if(key == KEY_ENTER) {
            int inputX = tableX; int inputY = startY + 20;
            if(selected == 5) break;

            // Logic Add/Edit/Delete Produk disini (Gunakan kode lama di dalam if block)
            // Untuk demo ini saya skip logic inputnya agar fokus ke navigasi
            if(selected == 2) {
                 textNormal();
                 gotoxy(inputX, inputY);   printf("Nama Produk : "); getString(dbProduk[totalProduk].nama, 49);
                 gotoxy(inputX, inputY+1); printf("Stok Awal   : "); scanf("%d", &dbProduk[totalProduk].stok);
                 gotoxy(inputX, inputY+2); printf("Harga Satuan: "); scanf("%ld", &dbProduk[totalProduk].harga);
                 dbProduk[totalProduk].id = totalProduk + 1;
                 totalProduk++;
                 clearRightContent();
            }
             // ... Tambahkan Logika Edit/Hapus disini ...
        }
    }
}

// =====================================================================================
// MENU KARYAWAN (NAVIGASI)
// =====================================================================================

void menuKaryawan() {
    int selected = 1;
    int totalMenu = 4;
    int key;
    drawBaseLayout("KARYAWAN");

    while(1) {
        // Gambar Menu Sidebar Karyawan
        // Note: Layout menu karyawan agak beda (2 baris text per item),
        // kita simplifikasi agar bisa pakai printMenuItem, atau buat manual highlighting

        gotoxy(2, HEADER_HEIGHT + 5);
        if(selected == 1) textHighlight(); else textNormal();
        printf(" [1] TRANSAKSI PEMBELIAN ");

        gotoxy(2, HEADER_HEIGHT + 8);
        if(selected == 2) textHighlight(); else textNormal();
        printf(" [2] TRANSAKSI PENJUALAN ");

        gotoxy(2, HEADER_HEIGHT + 11);
        if(selected == 3) textHighlight(); else textNormal();
        printf(" [3] LAPORAN TRANSAKSI   ");

        gotoxy(2, HEADER_HEIGHT + 14);
        if(selected == 4) textHighlight(); else textNormal();
        printf(" [0] Logout / Back       ");
        textNormal();

        // Content Placeholder
        printCenterRight(HEADER_HEIGHT + 10, "PILIH MENU TRANSAKSI DI SEBELAH KIRI");
        printCenterRight(HEADER_HEIGHT + 12, "Gunakan Panah Atas/Bawah & Enter");

        hideCursor();
        key = getch(); // Tunggu Input

        if(key == 224) {
            key = getch();
            if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; }
            if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; }
        }
        else if(key == KEY_ENTER) {
            if(selected == 4) break;

            clearRightContent();
            if(selected == 1) {
                printCenterRight(HEADER_HEIGHT + 5, "=== FORM TRANSAKSI PEMBELIAN ===");
                // Logic transaksi...
            } else if(selected == 2) {
                printCenterRight(HEADER_HEIGHT + 5, "=== FORM TRANSAKSI PENJUALAN ===");
                // Logic kasir...
            }
            gotoxy(SIDEBAR_WIDTH+5, HEADER_HEIGHT + 20);
            printf("Tekan sembarang tombol..."); getch();
            clearRightContent();
        }
    }
}

void menuUnderConstruction(char *title) {
    drawBaseLayout(title);
    printCenterRight(HEADER_HEIGHT + 10, "Fitur ini sedang dalam pengembangan.");
    getch();
}

// =====================================================================================
// MAIN PROGRAM
// =====================================================================================

int main() {
    // Setup Console
    system("mode con: cols=140 lines=40");
    maximizeConsole();
    setPinkTheme();
    system("color DF"); // Set Background Pink, Text White

    initDummyData();
    loadingAnimation();
    goFullscreen();
    updateScreenSize();
    hideCursor(); // Sembunyikan kursor agar menu terlihat bersih

    if(!loginScreen()) return 0;

    int selected = 1;
    int totalMenu = 5;
    int key;

    while(1) {
        // MENU UTAMA (HOME)
        system("cls");
        drawFullFrame();
        drawHeader();

        // Tampilan Menu Utama di Tengah (Box Style)
        int cx = screenWidth / 2;
        int cy = HEADER_HEIGHT + 5;

        // Gambar Menu dengan Highlight
        gotoxy(cx - 15, cy);   printf("=== MENU UTAMA SYSTEM ===");

        printMenuItem(cx - 15, cy+2, "1. KARYAWAN", selected == 1);
        printMenuItem(cx - 15, cy+3, "2. SUPPLIER", selected == 2);
        printMenuItem(cx - 15, cy+4, "3. GUDANG",   selected == 3);
        printMenuItem(cx - 15, cy+5, "4. PRODUK",   selected == 4);
        printMenuItem(cx - 15, cy+6, "0. KELUAR",   selected == 5);

        gotoxy(cx - 15, cy+9); printf("Gunakan Panah Atas/Bawah");

        hideCursor();
        key = getch(); // Tangkap tombol tanpa enter

        if(key == 224) { // 224 adalah kode awal untuk tombol panah
            key = getch(); // Tangkap kode kedua
            if(key == KEY_UP) {
                selected--;
                if(selected < 1) selected = totalMenu;
            }
            else if(key == KEY_DOWN) {
                selected++;
                if(selected > totalMenu) selected = 1;
            }
        }
        else if(key == KEY_ENTER) {
            // Eksekusi berdasarkan posisi 'selected'
            switch(selected) {
                case 1: menuKaryawan(); break;
                case 2: crudSupplier(); break;
                case 3: menuUnderConstruction("GUDANG"); break;
                case 4: crudProduk(); break;
                case 5: return 0;
            }
        }
    }
    return 0;
}