#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX_DATA 100
#define SIDEBAR_WIDTH 35
#define HEADER_HEIGHT 8
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_TAB 9
#define COLOR_HOT_PINK 13
#define COLOR_WHITE 15

typedef struct {
    int id;
    char nama[50];
    char alamat[100];
} Supplier;

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

int screenWidth = 0;
int screenHeight = 0;

void setPinkTheme();
void textNormal();
void textHighlight();
void goFullscreen();
void maximizeConsole();
void gotoxy(int x, int y);
void updateScreenSize();
void initDummyData();
void setCursorVisible(BOOL visible);
void hideCursor();
void getString(char *buffer, int maxLen);
void drawFullFrame();
void drawHeader();
void clearRightContent();
void printCenterRight(int y, const char* text);
int getCenterXForTable(int tableWidth);
void drawBaseLayout(const char* sidebarTitle);
void printMenuItem(int x, int y, char* text, int isSelected);
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight);
void loadingAnimation();
void getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible);
int loginScreen();
void displaySupplierTable(int tableX);

void displayProdukTable(int tableX);
void crudSupplier();
void crudProduk();
void menuUnderConstruction(char *title);
void drawMainMenu(int selected);

void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[COLOR_HOT_PINK] = RGB(255, 105, 180); // D: Hot Pink
    csbi.ColorTable[COLOR_WHITE] = RGB(255, 255, 255); // F: White
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void textNormal() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF);
}
void textHighlight() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF);
}
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
void updateScreenSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
void initDummyData() {
    totalSupplier = 0;
    totalProduk = 0;
}
void setCursorVisible(BOOL visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void hideCursor() {
    setCursorVisible(FALSE);
}

void getString(char *buffer, int maxLen) {
    setCursorVisible(TRUE);
    fflush(stdin);
    fgets(buffer, maxLen, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    setCursorVisible(FALSE);
}

void drawFullFrame() {
    updateScreenSize();
    int right = screenWidth - 1;
    int bottom = screenHeight - 1;
    textNormal();

    for (int x = 0; x <= right; x++) {
        gotoxy(x, 0); printf("%c", 205);
        gotoxy(x, bottom); printf("%c", 205);
        gotoxy(x, HEADER_HEIGHT); printf("%c", 205);
    }


    for (int y = 0; y <= bottom; y++) {
        gotoxy(0, y); printf("%c", 186);
        gotoxy(right, y); printf("%c", 186);
        if (y > HEADER_HEIGHT) {
            gotoxy(SIDEBAR_WIDTH, y); printf("%c", 186);
        }
    }

    // Corners
    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);

    // Crossings
    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
    gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", 203);
    gotoxy(SIDEBAR_WIDTH, bottom); printf("%c", 202);
}

// Menggambar header utama (judul aplikasi)
void drawHeader() {
    int startX = (screenWidth - 85) / 2;
    if (startX < 1) startX = 1;
    textNormal();
    // ASCII art untuk SUPERMARKET
    gotoxy(startX, 1); printf("  _____ _   _ _____  ______ _____  __  __       _____  _  ________ _______ ");
    gotoxy(startX, 2); printf(" / ____| | | |  __ \\|  ____|  __ \\|  \\/  |   /\\    |  __ \\| |/ /  ____|__   __|");
    gotoxy(startX, 3); printf("| (___ | | | | |__) | |__  | |__) | \\  / |  /  \\   | |__) | ' /| |__      | |   ");
    gotoxy(startX, 4); printf(" \\___ \\| | | |  ___/|  __| |  _  /| |\\/| | / /\\ \\  |  _  /|  < |  __|     | |   ");
    gotoxy(startX, 5); printf(" ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____    | |   ");
    gotoxy(startX, 6); printf("|_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|   |_|   ");
}

// Membersihkan area konten kanan
void clearRightContent() {
    updateScreenSize();
    textNormal();
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(SIDEBAR_WIDTH + 1, y);
        for(int x = SIDEBAR_WIDTH + 1; x < screenWidth - 1; x++) printf(" ");
    }
}

// Mencetak teks di tengah area konten kanan pada baris tertentu
void printCenterRight(int y, const char* text) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    int textLen = strlen(text);
    int x = SIDEBAR_WIDTH + 1 + (contentWidth - textLen) / 2;
    gotoxy(x, y); printf("%s", text);
}

// Menghitung posisi X untuk tabel agar tabel berada di tengah area konten kanan
int getCenterXForTable(int tableWidth) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    return SIDEBAR_WIDTH + 1 + (contentWidth - tableWidth) / 2;
}

// Menggambar layout dasar (bingkai, header, sidebar title)
void drawBaseLayout(const char* sidebarTitle) {
    system("cls");
    drawFullFrame();
    drawHeader();
    gotoxy(2, HEADER_HEIGHT + 2);
    printf("     MENU");
    gotoxy(2, HEADER_HEIGHT + 3);
    printf("   %-20s", sidebarTitle);
}

// Mencetak item menu (dengan atau tanpa penanda '>> <<')
void printMenuItem(int x, int y, char* text, int isSelected) {
    gotoxy(x, y);
    if(isSelected) {
        textNormal();
        printf(" >> %-20s << ", text);
    } else {
        textNormal();
        printf("    %-20s    ", text);
    }
}

// Menggambar kotak sederhana untuk form di area konten kanan
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight) {
    clearRightContent();
    *boxWidth = 60;
    *boxHeight = 15; // Dibuat lebih tinggi untuk menampung input yang lebih banyak
    *startX = SIDEBAR_WIDTH + 5;
    *startY = HEADER_HEIGHT + 3;

    textNormal();
    // Title
    gotoxy(*startX + 2, *startY); printf(" %s ", title);

    // Frame (Menggunakan karakter bingkai)
    // Horizontal lines
    for(int x=*startX; x<*startX+*boxWidth; x++) {
        gotoxy(x, *startY); printf("%c", 205);
        gotoxy(x, *startY+*boxHeight); printf("%c", 205);
    }
    // Vertical lines
    for(int y=*startY+1; y<*startY+*boxHeight; y++) {
        gotoxy(*startX, y); printf("%c", 186);
        gotoxy(*startX+*boxWidth, y); printf("%c", 186);
    }
    // Corners
    gotoxy(*startX, *startY); printf("%c", 201);
    gotoxy(*startX+*boxWidth, *startY); printf("%c", 187);
    gotoxy(*startX, *startY+*boxHeight); printf("%c", 200);
    gotoxy(*startX+*boxWidth, *startY+*boxHeight); printf("%c", 188);

}

// Menampilkan animasi loading
void loadingAnimation() {
    system("cls");
    updateScreenSize();
    int midY = screenHeight / 2;
    int midX = screenWidth / 2;
    gotoxy(midX - 5, midY); printf("Loading...");
    gotoxy(midX - 20, midY+2);
    for(int i=0; i<40; i++) {
        printf("%c", 219);
        Sleep(10);
    }
    Sleep(200);
}

// Mendapatkan kata sandi tersembunyi/terlihat
void getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible) {
    int i = 0;
    int key;
    setCursorVisible(TRUE);

    while(1) {
        if(i < maxLen - 1) {
            key = getch();
        } else {
            key = getch();
            if(key != 8 && key != KEY_ENTER && key != KEY_TAB) continue;
        }

        if (key == KEY_ENTER || key == 0) { // Selesai input
            buffer[i] = '\0';
            break;
        } else if (key == 8) { // Backspace
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                gotoxy(inputX + i, inputY); printf(" "); // Hapus karakter terakhir
                gotoxy(inputX + i, inputY); // Pindah kursor kembali
            }
        } else if (key == KEY_TAB) { // Toggle lihat/sembunyikan
            *isVisible = !(*isVisible);
            gotoxy(inputX, inputY);
            for(int j=0; j<maxLen-1; j++) printf(" "); // Hapus tampilan lama
            gotoxy(inputX, inputY);
            for(int j=0; j<i; j++) {
                printf("%c", (*isVisible) ? buffer[j] : '*');
            }
        }
        else if (i < maxLen - 1) {
            buffer[i] = (char)key;
            printf("%c", (*isVisible) ? buffer[i] : '*');
            i++;
        }
    }
    setCursorVisible(FALSE);
}

// Layar Login
int loginScreen() {
    char username[20] = "";
    char password[20] = "";
    int loginX, loginY, boxW, boxH;

    while(1) {
        system("cls");
        drawFullFrame();
        drawHeader();

        // Posisi Box
        updateScreenSize();
        boxW = 50;
        boxH = 10;
        loginX = (screenWidth - boxW) / 2;
        loginY = (screenHeight - boxH) / 2;

        textNormal();

        // --- MENGGAMBAR KOTAK LOGIN ---
        for(int x=loginX; x<loginX+boxW; x++) {
            gotoxy(x, loginY); printf("%c", 205);
            gotoxy(x, loginY+boxH); printf("%c", 205);
        }
        for(int y=loginY+1; y<loginY+boxH; y++) {
            gotoxy(loginX, y); printf("%c", 186);
            gotoxy(loginX+boxW, y); printf("%c", 186);
        }
        gotoxy(loginX, loginY); printf("%c", 201);
        gotoxy(loginX+boxW, loginY); printf("%c", 187);
        gotoxy(loginX, loginY+boxH); printf("%c", 200);
        gotoxy(loginX+boxW, loginY+boxH); printf("%c", 188);

        // --- JUDUL (LOGIN) ---
        gotoxy(loginX + (boxW/2) - (int)strlen("LOGIN")/2, loginY - 2);
        printf("LOGIN");

        // --- AREA INPUT ---
        int labelX = loginX + 5;
        int inputX = loginX + 17;
        int labelY = loginY + 3;

        // Label
        gotoxy(labelX, labelY);    printf("Username :");
        gotoxy(labelX, labelY+1); printf("Password :");
        gotoxy(labelX, labelY+2); printf("Tekan Tab untuk lihat password"); // Teks tambahan

        // Reset input fields
        gotoxy(inputX, labelY);   printf("                   ");
        gotoxy(inputX, labelY+1); printf("                   ");

        // Fokus ke Username
        setCursorVisible(TRUE);

        // Input Username
        gotoxy(inputX, labelY);
        getString(username, 19);

        // Input Password
        int isVisible = 0; // 0=sembunyi (default), 1=lihat

        gotoxy(inputX, labelY+1);
        getPassword(password, 19, inputX, labelY+1, &isVisible);

        // Cek kredensial
        if (strcmp(username, "123") == 0 && strcmp(password, "123") == 0) {
            gotoxy(loginX + 5, loginY + 8);
            printf("Login Berhasil! Memuat Menu Utama...");
            Sleep(1000);
            setCursorVisible(FALSE);
            return 1; // Sukses
        } else {
            gotoxy(loginX + 5, loginY + 8);
            printf("Nama Pengguna atau Kata Sandi Salah! (ESC untuk Keluar)");
            setCursorVisible(FALSE);

            // Tunggu input untuk keluar
            while(1) {
                int key = getch();
                if (key == KEY_ESC) return 0; // Keluar
                if (key == KEY_ENTER) break; // Coba lagi
            }
        }
    }
}


// Menampilkan tabel data Supplier
void displaySupplierTable(int tableX) {
    const int TABLE_WIDTH = 70;
    int startY = HEADER_HEIGHT + 4;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA SUPPLIER ===");

    // Clear area tabel
    for(int i=0; i<18; i++) {
        gotoxy(tableX, startY+i);
        for(int x=0; x<TABLE_WIDTH; x++) printf(" ");
    }

    // Header Tabel
    textNormal();
    gotoxy(tableX, startY); printf(" %-5s | %-30s | %-30s ", "ID", "NAMA SUPPLIER", "ALAMAT");
    gotoxy(tableX, startY+1); printf(" %-5s-%-30s-%-30s ", "-----", "------------------------------", "------------------------------");
    textNormal();

    if (totalSupplier == 0) {
        gotoxy(tableX + (TABLE_WIDTH / 2) - 10, startY + 3);
        printf(">> DATA MASIH KOSONG <<");
    } else {
        int limit = (totalSupplier > 15) ? 15 : totalSupplier;
        for(int i=0; i<limit; i++) {
            gotoxy(tableX, startY+2+i);
            printf(" %-5d | %-30.30s | %-30.30s ", dbSupplier[i].id, dbSupplier[i].nama, dbSupplier[i].alamat);
        }
    }
}

// **FUNGSI BARU DITAMBAHKAN: Menampilkan tabel data Produk**
void displayProdukTable(int tableX) {
    const int TABLE_WIDTH = 74;
    int startY = HEADER_HEIGHT + 4;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA STOK PRODUK ===");

    // Clear area tabel
    for(int i=0; i<18; i++) {
        gotoxy(tableX, startY+i);
        for(int x=0; x<TABLE_WIDTH; x++) printf(" ");
    }

    // Header Tabel
    textNormal();
    gotoxy(tableX, startY); printf(" %-5s | %-30s | %-10s | %-20s ", "ID", "NAMA PRODUK", "STOK", "HARGA (Rp)");
    gotoxy(tableX, startY+1); printf(" %-5s-%-30s-%-10s-%-20s ", "-----", "------------------------------", "----------", "--------------------");
    textNormal();

    if (totalProduk == 0) {
        gotoxy(tableX + (TABLE_WIDTH / 2) - 10, startY + 3);
        printf(">> DATA MASIH KOSONG <<");
    } else {
        int limit = (totalProduk > 15) ? 15 : totalProduk;
        for(int i=0; i<limit; i++) {
            gotoxy(tableX, startY+2+i);
            printf(" %-5d | %-30.30s | %-10d | Rp. %-15ld ", dbProduk[i].id, dbProduk[i].nama, dbProduk[i].stok, dbProduk[i].harga);
        }
    }
}


// Fungsi CRUD untuk Supplier
void crudSupplier() {
    int selected = 1;
    int totalMenu = 5;
    int key;

    drawBaseLayout("MANAJEMEN SUPPLIER");

    const int TABLE_WIDTH = 70;
    int tableX = getCenterXForTable(TABLE_WIDTH);

    int formBoxX, formBoxY, boxW, boxH;
    int formX, formInputX, formY;

    while(1) {
        // Tampilkan menu
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Supplier", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Supplier", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Supplier", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Supplier", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        displaySupplierTable(tableX);

        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; }
            else if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; }
        }
        else if(key == KEY_ENTER) {
            formBoxX = 0; formBoxY = 0; boxW = 0; boxH = 0;
            formX = SIDEBAR_WIDTH + 7;
            formInputX = SIDEBAR_WIDTH + 25;
            formY = HEADER_HEIGHT + 5;

            if(selected == 5) break;

            // --- TAMBAH SUPPLIER ---
            else if(selected == 2) {
                if(totalSupplier >= MAX_DATA) {
                    printCenterRight(HEADER_HEIGHT + 20, "DATABASE SUPPLIER PENUH!"); Sleep(1000);
                    clearRightContent();
                    continue;
                }
                drawFormBox("TAMBAH SUPPLIER BARU", &formBoxX, &formBoxY, &boxW, &boxH);
                setCursorVisible(TRUE);

                int newId = 1;
                if(totalSupplier > 0) newId = dbSupplier[totalSupplier-1].id + 1;

                dbSupplier[totalSupplier].id = newId;
                gotoxy(formX, formY+2); printf("ID Otomatis  : %d", dbSupplier[totalSupplier].id);

                gotoxy(formX, formY+4); printf("Nama Supplier : "); gotoxy(formInputX, formY+4); getString(dbSupplier[totalSupplier].nama, 49);
                if(strlen(dbSupplier[totalSupplier].nama) == 0) {
                    printCenterRight(formY+12, "Input dibatalkan (Nama kosong).");
                    Sleep(1000); clearRightContent(); continue;
                }

                gotoxy(formX, formY+6); printf("Alamat        : "); gotoxy(formInputX, formY+6); getString(dbSupplier[totalSupplier].alamat, 99);
                if(strlen(dbSupplier[totalSupplier].alamat) == 0) {
                    printCenterRight(formY+12, "Input dibatalkan (Alamat kosong).");
                    Sleep(1000); clearRightContent(); continue;
                }

                totalSupplier++;
                setCursorVisible(FALSE);
                gotoxy(formX, formY+10); printf(">> SUKSES! Supplier berhasil disimpan.");
                Sleep(1000);
                clearRightContent();
            }
            else if (selected == 3) {
                if(totalSupplier == 0) { clearRightContent(); continue; }
                drawFormBox("EDIT DATA SUPPLIER", &formBoxX, &formBoxY, &boxW, &boxH);

                int idTarget, idx = -1;
                gotoxy(formX, formY+2); printf("Masukkan ID yang akan diedit: ");
                setCursorVisible(TRUE);
                if (scanf("%d", &idTarget) != 1) idTarget = -1;
                while ((getchar()) != '\n');
                setCursorVisible(FALSE);

                for(int i=0; i<totalSupplier; i++) {
                    if(dbSupplier[i].id == idTarget) { idx = i; break; }
                }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("ID Supplier: %d", dbSupplier[idx].id);
                    gotoxy(formX, formY+6); printf("Nama Lama  : %s", dbSupplier[idx].nama);
                    gotoxy(formX, formY+7); printf("Nama Baru  : ");
                    gotoxy(formX, formY+9); printf("Alamat Lama: %s", dbSupplier[idx].alamat);
                    gotoxy(formX, formY+10); printf("Alamat Baru: ");
                    setCursorVisible(TRUE);
                    gotoxy(formInputX, formY+7); getString(dbSupplier[idx].nama, 49);
                    gotoxy(formInputX, formY+10); getString(dbSupplier[idx].alamat, 99);
                    setCursorVisible(FALSE);
                    gotoxy(formX, formY+13); printf(">> Data berhasil diperbarui!");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID %d Tidak Ditemukan!", idTarget);
                }
                Sleep(1500);
                clearRightContent();
            }

            else if (selected == 4) {
                if(totalSupplier == 0) { clearRightContent(); continue; }
                drawFormBox("HAPUS DATA SUPPLIER", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;
                gotoxy(formX, formY+2); printf("Masukkan ID untuk dihapus: ");
                setCursorVisible(TRUE);
                if (scanf("%d", &idTarget) != 1) idTarget = -1;
                while ((getchar()) != '\n');
                setCursorVisible(FALSE);
                for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == idTarget) { idx = i; break; }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("Menghapus Supplier ID %d: %s...", dbSupplier[idx].id, dbSupplier[idx].nama);
                    for(int j=idx; j<totalSupplier-1; j++) {
                        dbSupplier[j] = dbSupplier[j+1];
                    }
                    totalSupplier--;
                    gotoxy(formX, formY+6); printf(">> Data Terhapus Permanen.");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID Tidak Ditemukan!");
                }
                Sleep(1500);
                clearRightContent();
            }
        }
    }
}
void crudProduk() {
    int selected = 1;
    int totalMenu = 5;
    int key;
    drawBaseLayout("MANAJEMEN PRODUK");
    const int TABLE_WIDTH = 74; // Sesuaikan lebar tabel untuk rapikan
    int tableX = getCenterXForTable(TABLE_WIDTH);
    int formBoxX, formBoxY, boxW, boxH;
    int formX, formInputX, formY;
    while(1) {
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Stok", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Produk", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Produk", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Produk", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);
        displayProdukTable(tableX);
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; }
            else if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; }
        }
        else if(key == KEY_ENTER) {
            formBoxX = 0; formBoxY = 0; boxW = 0; boxH = 0;
            formX = SIDEBAR_WIDTH + 7;
            formInputX = SIDEBAR_WIDTH + 25;
            formY = HEADER_HEIGHT + 5;

            if(selected == 5) break;
            else if(selected == 2) {
                 if(totalProduk >= MAX_DATA) {
                    printCenterRight(HEADER_HEIGHT + 20, "DATABASE PRODUK PENUH!"); Sleep(1000);
                    clearRightContent();
                    continue;
                 }
                 drawFormBox("TAMBAH PRODUK BARU", &formBoxX, &formBoxY, &boxW, &boxH);
                 setCursorVisible(TRUE);
                 int newId = 1;
                 if(totalProduk > 0) newId = dbProduk[totalProduk-1].id + 1;
                 dbProduk[totalProduk].id = newId;
                 gotoxy(formX, formY+2); printf("ID Otomatis  : %d", dbProduk[totalProduk].id);
                 gotoxy(formX, formY+4); printf("Nama Produk : "); gotoxy(formInputX, formY+4); getString(dbProduk[totalProduk].nama, 49);
                 if(strlen(dbProduk[totalProduk].nama) == 0) {
                     printCenterRight(formY+12, "Input dibatalkan (Nama kosong).");
                     Sleep(1000); clearRightContent(); continue;
                 }
                 gotoxy(formX, formY+6); printf("Stok Awal    : "); gotoxy(formInputX, formY+6);
                 if (scanf("%d", &dbProduk[totalProduk].stok) != 1) dbProduk[totalProduk].stok = 0;
                 while ((getchar()) != '\n');
                 gotoxy(formX, formY+8); printf("Harga Satuan: "); gotoxy(formInputX, formY+8);
                 if (scanf("%ld", &dbProduk[totalProduk].harga) != 1) dbProduk[totalProduk].harga = 0;
                 while ((getchar()) != '\n');
                 totalProduk++;
                 setCursorVisible(FALSE);
                 gotoxy(formX, formY+10); printf(">> SUKSES! Produk berhasil disimpan.");
                 Sleep(1000);
                 clearRightContent();
            }
            else if (selected == 3) {
                if(totalProduk == 0) { clearRightContent(); continue; }
                drawFormBox("EDIT DATA PRODUK", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;
                gotoxy(formX, formY+2); printf("Masukkan ID yang akan diedit: ");
                setCursorVisible(TRUE);
                if (scanf("%d", &idTarget) != 1) idTarget = -1;
                while ((getchar()) != '\n');
                setCursorVisible(FALSE);
                for(int i=0; i<totalProduk; i++) {
                    if(dbProduk[i].id == idTarget) { idx = i; break; }
                }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("Nama Lama  : %s", dbProduk[idx].nama);
                    gotoxy(formX, formY+5); printf("Nama Baru  : ");
                    gotoxy(formX, formY+7); printf("Stok Lama  : %d", dbProduk[idx].stok);
                    gotoxy(formX, formY+8); printf("Stok Baru  : ");
                    gotoxy(formX, formY+10); printf("Harga Lama : Rp. %ld", dbProduk[idx].harga);
                    gotoxy(formX, formY+11); printf("Harga Baru : ");
                    setCursorVisible(TRUE);
                    gotoxy(formInputX, formY+5); getString(dbProduk[idx].nama, 49);
                    gotoxy(formInputX, formY+8);
                    int tempStok;
                    if (scanf("%d", &tempStok) == 1) dbProduk[idx].stok = tempStok;
                    while ((getchar()) != '\n');
                    gotoxy(formInputX, formY+11);
                    long tempHarga;
                    if (scanf("%ld", &tempHarga) == 1) dbProduk[idx].harga = tempHarga;
                    while ((getchar()) != '\n');
                    setCursorVisible(FALSE);
                    gotoxy(formX, formY+13); printf(">> Data berhasil diperbarui!");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID %d Tidak Ditemukan!", idTarget);
                }
                Sleep(1500);
                clearRightContent();
            }
            else if (selected == 4) {
                if(totalProduk == 0) { clearRightContent(); continue; }
                drawFormBox("HAPUS DATA PRODUK", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;
                gotoxy(formX, formY+2); printf("Masukkan ID untuk dihapus: ");
                setCursorVisible(TRUE);
                if (scanf("%d", &idTarget) != 1) idTarget = -1;
                while ((getchar()) != '\n');
                setCursorVisible(FALSE);
                for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == idTarget) { idx = i; break; }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("Menghapus Produk ID %d: %s...", dbProduk[idx].id, dbProduk[idx].nama);
                    for(int j=idx; j<totalProduk-1; j++) {
                        dbProduk[j] = dbProduk[j+1];
                    }
                    totalProduk--;
                    gotoxy(formX, formY+6); printf(">> Data Terhapus Permanen.");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID Tidak Ditemukan!");
                }
                Sleep(1500);
                clearRightContent();
            }
        }
    }
}
void menuUnderConstruction(char *title) {
    drawBaseLayout(title);
    clearRightContent();
    int midY = HEADER_HEIGHT + 10;
    printCenterRight(midY, "==================================================");
    printCenterRight(midY + 1, "FITUR INI MASIH DALAM TAHAP PENGEMBANGAN");
    printCenterRight(midY + 2, "==================================================");
    printCenterRight(midY + 4, "Tekan tombol apapun untuk kembali ke Menu Utama...");
    getch();
    clearRightContent();
}
void drawMainMenu(int selected) {
    system("cls");
    drawFullFrame();
    drawHeader();
    int cx = screenWidth / 2;
    int cy = HEADER_HEIGHT + 5;
    gotoxy(cx - 15, cy);    printf("=== MENU UTAMA SYSTEM ===");
    printMenuItem(cx - 15, cy+2, "1. KARYAWAN", selected == 1);
    printMenuItem(cx - 15, cy+3, "2. SUPPLIER", selected == 2);
    printMenuItem(cx - 15, cy+4, "3. GUDANG",    selected == 3);
    printMenuItem(cx - 15, cy+5, "4. PRODUK",    selected == 4);
    printMenuItem(cx - 15, cy+6, "0. KELUAR",    selected == 5);

    gotoxy(cx - 15, cy+9); printf("Gunakan Panah Atas/Bawah");
}
int main() {

    system("mode con: cols=140 lines=40");
    maximizeConsole();
    setPinkTheme();
    system("color DF");
    initDummyData();
    loadingAnimation();
    goFullscreen();
    updateScreenSize();
    hideCursor();

    // Data Dummy Produk
    totalProduk = 15;
    dbProduk[0] = (Produk){1, "Susu Sapi Soleh", 10, 5000L};
    dbProduk[1] = (Produk){2, "Coklat Dubai", 5, 65000L};
    dbProduk[2] = (Produk){3, "Beras HONDA", 10, 50000L};
    dbProduk[3] = (Produk){4, "Keju Jawa", 15, 2000000L};
    dbProduk[4] = (Produk){5, "Mie Instan Rasa Soto", 50, 2500L};
    dbProduk[5] = (Produk){6, "Kopi Bubuk Arabica Premium", 30, 45000L};
    dbProduk[6] = (Produk){7, "Telur Ayam Omega (per box)", 25, 38000L};
    dbProduk[7] = (Produk){8, "Minyak Goreng Sawit 2L", 40, 32000L};
    dbProduk[8] = (Produk){9, "Sabun Mandi Cair Lemon", 60, 18000L};
    dbProduk[9] = (Produk){10, "Pasta Gigi Charcoal", 45, 12500L};
    dbProduk[10] = (Produk){11, "Deterjen Bubuk Matik 1kg", 20, 48000L};
    dbProduk[11] = (Produk){12, "Kerupuk Udang Super", 80, 15000L};
    dbProduk[12] = (Produk){13, "Air Mineral 600ml (per karton)", 15, 52000L};
    dbProduk[13] = (Produk){14, "Saus Tomat Pedas Jumbo", 35, 22000L};
    dbProduk[14] = (Produk){15, "Roti Tawar Gandum Utuh", 25, 17500L};

    totalSupplier = 15;
    dbSupplier[0] = (Supplier){101, "PT. Makmur Sejahtera", "Jl. Merdeka No. 10, Jakarta"};
    dbSupplier[1] = (Supplier){102, "CV. Cepat Tanggap", "Perumahan Indah Blok C, Bandung"};
    dbSupplier[2] = (Supplier){103, "Distributor Utama SBY", "Ruko 2000, Surabaya"};
    dbSupplier[3] = (Supplier){104, "UD. Barokah Pangan", "Pasar Induk Kramat Jati, Jakarta"};
    dbSupplier[4] = (Supplier){105, "PT. Tirtamulia Beverage", "Kawasan Industri MM2100, Bekasi"};
    dbSupplier[5] = (Supplier){106, "CV. Sinar Abadi Elektronik", "Jalan Raya Kopo No. 50, Bandung"};
    dbSupplier[6] = (Supplier){107, "Pabrik Kertas Nusantara", "Sentul City Blok A, Bogor"};
    dbSupplier[7] = (Supplier){108, "Distri Sentra Farmasi", "Jalan Kesehatan No. 12, Semarang"};
    dbSupplier[8] = (Supplier){109, "Koprasi Petani Sejahtera", "Desa Sukamaju, Yogyakarta"};
    dbSupplier[9] = (Supplier){110, "PT. Karya Baja Mandiri", "Gedung Baja Lantai 5, Surabaya"};
    dbSupplier[10] = (Supplier){111, "Importir Bahan Baku Asia", "Pelabuhan Tanjung Priok, Jakarta"};
    dbSupplier[11] = (Supplier){112, "CV. Produk Rumah Tangga", "Jalan Kapas No. 22, Malang"};
    dbSupplier[12] = (Supplier){113, "PT. Logistics Express", "Gudang 7, Cikarang"};
    dbSupplier[13] = (Supplier){114, "UD. Ikan Segar Samudra", "Pekalongan Utara, Jawa Tengah"};
    dbSupplier[14] = (Supplier){115, "PT. Solusi Kemasan", "Ruko Niaga, Tangerang"};

    if (!loginScreen()) {
        return 0;
    }
    int selected = 1;
    int totalMenu = 5;
    int key;
    drawMainMenu(selected);
    while(1) {
        drawMainMenu(selected);
        hideCursor();
        key = getch();

        if(key == 224) {
            key = getch();
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
            switch(selected) {
                case 1: menuUnderConstruction("KARYAWAN"); break;
                case 2: crudSupplier(); break; // Mengganti ke fungsi CRUD Supplier yang baru
                case 3: menuUnderConstruction("GUDANG"); break;
                case 4: crudProduk(); break;
                case 5: return 0;
            }
            drawMainMenu(selected);
        }
    }
    return 0;
}