#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

// =====================================================================================
// KONFIGURASI LAYAR & KEYBOARD
// =====================================================================================
#define MAX_DATA 200
#define SIDEBAR_WIDTH 35
#define HEADER_HEIGHT 8

// Key Codes
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27

// DEFINISI ROLE
#define ROLE_KARYAWAN 1
#define ROLE_SUPPLIER 2
#define ROLE_GUDANG 3
#define ROLE_SUPER_ADMIN 4

// Global Variables
int screenWidth = 0;
int screenHeight = 0;
int CURRENT_ROLE = 0;
int IS_RUNNING = 1;

// Warna Theme (Pink & White)
void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[13] = RGB(255, 105, 180);
    csbi.ColorTable[15] = RGB(255, 255, 255);
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void textNormal() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF); }
void textError()  { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xCF); }

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

void maximizeConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(150);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void getString(char *buffer, int maxLen) {
    CONSOLE_CURSOR_INFO c; GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
    c.bVisible = TRUE; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
    fflush(stdin); fgets(buffer, maxLen, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    c.bVisible = FALSE; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
}

void goFullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1C, 0, 0);
    keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(150);
}

// =====================================================================================
// STRUKTUR DATA: Master untuk tiap role
// =====================================================================================
typedef struct { int id; char nama[50]; char alamat[100]; } Supplier;
typedef struct { int id; char nama[50]; int stok; long harga; } Produk;
typedef struct { int id; char nama[50]; char jabatan[30]; char telp[20]; } Karyawan;
typedef struct { int id; char nama[50]; char alamat[100]; char telp[20]; } Pelanggan;
typedef struct { int id; char namaBarang[50]; int qty; } BarangSupplier;
typedef struct { int id; char namaGudang[50]; char lokasi[50]; } LokasiGudang;
typedef struct { int id; char username[30]; char roleName[30]; } User;

// Database arrays + counters
Supplier dbSupplier[MAX_DATA]; int totalSupplier = 0;
Produk dbProduk[MAX_DATA]; int totalProduk = 0;
Karyawan dbKaryawan[MAX_DATA]; int totalKaryawan = 0;
Pelanggan dbPelanggan[MAX_DATA]; int totalPelanggan = 0;
BarangSupplier dbBarangSupplier[MAX_DATA]; int totalBarangSupplier = 0;
LokasiGudang dbLokasiGudang[MAX_DATA]; int totalLokasiGudang = 0;
User dbUser[MAX_DATA]; int totalUser = 0;

// =====================================================================================
// UI DRAWING (sama dengan sebelumnya, sedikit disederhanakan)
// =====================================================================================
void drawFullFrame() {
    updateScreenSize();
    int r = screenWidth - 1, b = screenHeight - 1;
    textNormal();
    for (int x = 0; x <= r; x++) { gotoxy(x,0); printf("%c",205); gotoxy(x,b); printf("%c",205); gotoxy(x,HEADER_HEIGHT); printf("%c",205); }
    for (int y = 0; y <= b; y++) { gotoxy(0,y); printf("%c",186); gotoxy(r,y); printf("%c",186); if(y>HEADER_HEIGHT) {gotoxy(SIDEBAR_WIDTH,y); printf("%c",186);} }
    gotoxy(0,0); printf("%c",201); gotoxy(r,0); printf("%c",187); gotoxy(r,b); printf("%c",188); gotoxy(0,b); printf("%c",200);
    gotoxy(0,HEADER_HEIGHT); printf("%c",204); gotoxy(r,HEADER_HEIGHT); printf("%c",185);
    gotoxy(SIDEBAR_WIDTH,HEADER_HEIGHT); printf("%c",203); gotoxy(SIDEBAR_WIDTH,b); printf("%c",202);
}

void drawHeader() {
    int startX = (screenWidth - 85) / 2; if (startX < 1) startX = 1;
    textNormal();
    gotoxy(startX, 1); printf("  _____ _    _ _____  ______ _____  __  __           _____  _  ________ _______ ");
    gotoxy(startX, 2); printf(" / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__    __|");
    gotoxy(startX, 3); printf("| (___ | |  | | |__) | |__  | |__) | \\  / |   /  \\   | |__) | ' /| |__     | |   ");
    gotoxy(startX, 4); printf(" \\___ \\| |  | |  ___/|  __| |  _  /| |\\/| |  / /\\ \\  |  _  /|  < |  __|    | |   ");
    gotoxy(startX, 5); printf(" ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____   | |   ");
    gotoxy(startX, 6); printf("|_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|  |_|   ");
}

void clearRightContent() {
    updateScreenSize(); textNormal();
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(SIDEBAR_WIDTH + 1, y); for(int x = SIDEBAR_WIDTH + 1; x < screenWidth - 1; x++) printf(" ");
    }
}

void printMenuItem(int x, int y, char* text, int isSelected) {
    gotoxy(x, y);
    textNormal();
    if(isSelected) printf(" >> %-20s << ", text);
    else printf("    %-20s    ", text);
}

void drawFormBox(char* title) {
    clearRightContent();
    int sx = SIDEBAR_WIDTH + 5, sy = HEADER_HEIGHT + 3, w = 60, h = 10;
    textNormal(); gotoxy(sx, sy); printf(" %s ", title);
    for(int y=sy+1; y<sy+h; y++) { gotoxy(sx, y); printf("|"); gotoxy(sx+w, y); printf("|"); }
    gotoxy(sx, sy+h); for(int i=0; i<=w; i++) printf("-");
}

void printCenterRight(int y, const char* text) {
    int cw = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    int x = SIDEBAR_WIDTH + 1 + (cw - strlen(text)) / 2;
    gotoxy(x, y); printf("%s", text);
}

int getCenterXForTable(int tableWidth) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    return SIDEBAR_WIDTH + 1 + (contentWidth - tableWidth) / 2;
}

// =====================================================================================
// DUMMY DATA INITIALIZATION (20 records per master)
// =====================================================================================
void initDummyData() {
    totalSupplier = totalProduk = totalKaryawan = totalPelanggan = 0;
    totalBarangSupplier = totalLokasiGudang = totalUser = 0;

    // Supplier (20)
    for(int i=0;i<20;i++){
        dbSupplier[i].id = i+1;
        sprintf(dbSupplier[i].nama, "Supplier %02d", i+1);
        sprintf(dbSupplier[i].alamat, "Jalan Supplier No.%02d, Kota", i+1);
    }
    totalSupplier = 20;

    // Produk (20)
    for(int i=0;i<20;i++){
        dbProduk[i].id = i+1;
        sprintf(dbProduk[i].nama, "Produk %02d", i+1);
        dbProduk[i].stok = 10 + i;
        dbProduk[i].harga = 10000 + (i*500);
    }
    totalProduk = 20;

    // Karyawan (20)
    for(int i=0;i<20;i++){
        dbKaryawan[i].id = i+1;
        sprintf(dbKaryawan[i].nama, "Karyawan %02d", i+1);
        sprintf(dbKaryawan[i].jabatan, "Kasir");
        sprintf(dbKaryawan[i].telp, "0812%06d", 100000 + i);
    }
    totalKaryawan = 20;

    // Pelanggan (20)
    for(int i=0;i<20;i++){
        dbPelanggan[i].id = i+1;
        sprintf(dbPelanggan[i].nama, "Pelanggan %02d", i+1);
        sprintf(dbPelanggan[i].alamat, "Alamat Pelanggan %02d", i+1);
        sprintf(dbPelanggan[i].telp, "0821%06d", 200000 + i);
    }
    totalPelanggan = 20;

    // BarangSupplier (20)
    for(int i=0;i<20;i++){
        dbBarangSupplier[i].id = i+1;
        sprintf(dbBarangSupplier[i].namaBarang, "BarangSup %02d", i+1);
        dbBarangSupplier[i].qty = 5 + i;
    }
    totalBarangSupplier = 20;

    // LokasiGudang (20)
    for(int i=0;i<20;i++){
        dbLokasiGudang[i].id = i+1;
        sprintf(dbLokasiGudang[i].namaGudang, "Gudang %02d", i+1);
        sprintf(dbLokasiGudang[i].lokasi, "Zona %02d", i+1);
    }
    totalLokasiGudang = 20;

    // Users (SuperAdmin & sample users)
    totalUser = 0;
    dbUser[totalUser].id = ++totalUser; strcpy(dbUser[totalUser-1].username, "super"); strcpy(dbUser[totalUser-1].roleName, "SUPER_ADMIN");
    dbUser[totalUser].id = ++totalUser; strcpy(dbUser[totalUser-1].username, "gudang"); strcpy(dbUser[totalUser-1].roleName, "GUDANG");
    dbUser[totalUser].id = ++totalUser; strcpy(dbUser[totalUser-1].username, "supplier"); strcpy(dbUser[totalUser-1].roleName, "SUPPLIER");
    dbUser[totalUser].id = ++totalUser; strcpy(dbUser[totalUser-1].username, "kasir"); strcpy(dbUser[totalUser-1].roleName, "KARYAWAN");
}

// =====================================================================================
// ACCESS DENIED
// =====================================================================================
void accessDenied() {
    updateScreenSize();
    int mx = screenWidth/2;
    int my = screenHeight/2;
    textError();
    gotoxy(mx-15, my);   printf("===============================");
    gotoxy(mx-15, my+1); printf("|  AKSES DITOLAK / RESTRICTED |");
    gotoxy(mx-15, my+2); printf("===============================");
    textNormal();
    gotoxy(mx-15, my+3); printf("Role Anda tidak memiliki izin.");
    getch();
}

// =====================================================================================
// CRUD GENERIC: Implementasi untuk tiap master (view/add/edit/delete)
// =====================================================================================

// --------------------------- Supplier (sudah ada) ---------------------------
void crudSupplier() {
    int selected = 1; int totalMenu = 5; int key;
    int formLabelX = SIDEBAR_WIDTH + 7; int formInputX = SIDEBAR_WIDTH + 25; int formY = HEADER_HEIGHT + 5;
    drawFullFrame(); drawHeader();
    while(1) {
        drawFullFrame(); drawHeader(); drawBaseLayout: ;
        // Sidebar menu
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Data", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Data", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Data", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Data", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        // Table rendering
        textNormal(); gotoxy(SIDEBAR_WIDTH+3, HEADER_HEIGHT + 2); printf("DATA SUPPLIER: %d Record(s)", totalSupplier);
        int tableX = SIDEBAR_WIDTH + 3; int startY = HEADER_HEIGHT + 4;
        for(int i=0;i<18;i++){ gotoxy(tableX, startY+i); printf("                                                                                "); }
        gotoxy(tableX, startY); printf(" %-4s | %-25s | %-40s ", "ID", "NAMA SUPPLIER", "ALAMAT LENGKAP");
        if(totalSupplier == 0) { gotoxy(tableX + 20, startY + 2); printf(">> DATA MASIH KOSONG <<"); }
        else {
            int limit = (totalSupplier > 15) ? 15 : totalSupplier;
            for(int i=0;i<limit;i++){
                gotoxy(tableX, startY+2+i);
                printf(" %-4d | %-25.25s | %-40.40s ", dbSupplier[i].id, dbSupplier[i].nama, dbSupplier[i].alamat);
            }
        }

        hideCursor(); key = getch();
        if(key == 224) { key = getch(); if(key == KEY_UP) {selected--; if(selected < 1) selected = totalMenu;} else if(key == KEY_DOWN) {selected++; if(selected > totalMenu) selected = 1;} }
        else if(key == KEY_ENTER) {
            if(selected == 5) break;
            else if(selected == 2) {
                if(totalSupplier >= MAX_DATA) continue;
                drawFormBox("TAMBAH SUPPLIER");
                int newId = (totalSupplier>0)? dbSupplier[totalSupplier-1].id + 1 : 1;
                gotoxy(formLabelX, formY+2); printf("ID Otomatis : %d", newId);
                gotoxy(formLabelX, formY+4); printf("Nama        : "); gotoxy(formInputX, formY+4); getString(dbSupplier[totalSupplier].nama, 49);
                if(strlen(dbSupplier[totalSupplier].nama) == 0) { clearRightContent(); continue; }
                gotoxy(formLabelX, formY+6); printf("Alamat      : "); gotoxy(formInputX, formY+6); getString(dbSupplier[totalSupplier].alamat, 99);
                dbSupplier[totalSupplier].id = newId; totalSupplier++; clearRightContent();
            }
            else if(selected == 3 || selected == 4) {
                if(totalSupplier == 0) continue;
                drawFormBox(selected==3? "EDIT SUPPLIER" : "HAPUS SUPPLIER");
                int idTarget, idx=-1;
                gotoxy(formLabelX, formY+2); printf("Masukkan ID : ");
                CONSOLE_CURSOR_INFO c; GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c); c.bVisible=TRUE; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &c);
                scanf("%d", &idTarget); while((getchar())!='\n');
                for(int i=0;i<totalSupplier;i++) if(dbSupplier[i].id==idTarget) idx=i;
                if(idx!=-1){
                    if(selected==3){
                        gotoxy(formLabelX, formY+4); printf("Nama Baru   : "); gotoxy(formInputX, formY+4); getString(dbSupplier[idx].nama,49);
                        gotoxy(formLabelX, formY+6); printf("Alamat Baru : "); gotoxy(formInputX, formY+6); getString(dbSupplier[idx].alamat,99);
                    } else {
                        for(int j=idx;j<totalSupplier-1;j++) dbSupplier[j]=dbSupplier[j+1];
                        totalSupplier--;
                    }
                    gotoxy(formLabelX, formY+8); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            }
        }
    }
}

// --------------------------- Produk (Gudang) ---------------------------
void crudProduk() {
    int selected = 1, totalMenu = 5, key;
    int formLabelX = SIDEBAR_WIDTH + 7, formInputX = SIDEBAR_WIDTH + 25, formY = HEADER_HEIGHT + 5;
    while(1) {
        drawFullFrame(); drawHeader();
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Stok", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Produk", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Produk", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Produk", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        int tableX = getCenterXForTable(70); int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf(" %-5s | %-30s | %-10s | %-15s ", "ID", "NAMA PRODUK", "STOK", "HARGA");
        gotoxy(tableX, startY+1); printf("----------------------------------------------------------------------");
        int limit = (totalProduk > 15) ? 15 : totalProduk;
        for(int i=0;i<limit;i++){
            gotoxy(tableX, startY+2+i);
            printf(" %-5d | %-30.30s | %-10d | Rp. %-10ld ", dbProduk[i].id, dbProduk[i].nama, dbProduk[i].stok, dbProduk[i].harga);
        }

        hideCursor(); key = getch();
        if(key == 224) { key = getch(); if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; } else if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; } }
        else if(key == KEY_ENTER) {
            if(selected == 5) break;
            if(selected == 2) {
                 drawFormBox("TAMBAH PRODUK");
                 gotoxy(formLabelX, formY+4); printf("Nama Produk : "); gotoxy(formInputX, formY+4); getString(dbProduk[totalProduk].nama, 49);
                 gotoxy(formLabelX, formY+6); printf("Stok Awal   : "); gotoxy(formInputX, formY+6); scanf("%d", &dbProduk[totalProduk].stok); while ((getchar()) != '\n');
                 gotoxy(formLabelX, formY+8); printf("Harga Satuan: "); gotoxy(formInputX, formY+8); scanf("%ld", &dbProduk[totalProduk].harga); while ((getchar()) != '\n');
                 dbProduk[totalProduk].id = (totalProduk>0)? dbProduk[totalProduk-1].id + 1 : 1; totalProduk++; clearRightContent();
            } else if(selected==3 || selected==4) {
                if(totalProduk==0) continue;
                drawFormBox(selected==3?"EDIT PRODUK":"HAPUS PRODUK");
                int idTarget, idx=-1;
                gotoxy(formLabelX, formY+2); printf("Masukkan ID : "); scanf("%d",&idTarget); while((getchar())!='\n');
                for(int i=0;i<totalProduk;i++) if(dbProduk[i].id==idTarget) idx=i;
                if(idx!=-1){
                    if(selected==3){
                        gotoxy(formLabelX, formY+4); printf("Nama Baru : "); gotoxy(formInputX, formY+4); getString(dbProduk[idx].nama,49);
                        gotoxy(formLabelX, formY+6); printf("Stok Baru : "); gotoxy(formInputX, formY+6); scanf("%d",&dbProduk[idx].stok); while((getchar())!='\n');
                        gotoxy(formLabelX, formY+8); printf("Harga Baru: "); gotoxy(formInputX, formY+8); scanf("%ld",&dbProduk[idx].harga); while((getchar())!='\n');
                    } else {
                        for(int j=idx;j<totalProduk-1;j++) dbProduk[j]=dbProduk[j+1];
                        totalProduk--;
                    }
                    gotoxy(formLabelX, formY+10); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            }
        }
    }
}

// --------------------------- Karyawan (Karyawan role) ---------------------------
void crudKaryawan() {
    int selected = 1, totalMenu = 5, key;
    int formLabelX = SIDEBAR_WIDTH + 7, formInputX = SIDEBAR_WIDTH + 25, formY = HEADER_HEIGHT + 5;
    while(1){
        drawFullFrame(); drawHeader();
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Karyawan", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Karyawan", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Karyawan", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Karyawan", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        int tableX = SIDEBAR_WIDTH + 3; int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf(" %-4s | %-25s | %-15s | %-12s ", "ID","NAMA","JABATAN","TELP");
        int limit = (totalKaryawan>15)?15:totalKaryawan;
        for(int i=0;i<limit;i++){
            gotoxy(tableX, startY+2+i);
            printf(" %-4d | %-25.25s | %-15.15s | %-12.12s ", dbKaryawan[i].id, dbKaryawan[i].nama, dbKaryawan[i].jabatan, dbKaryawan[i].telp);
        }

        hideCursor(); key=getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==5) break;
            else if(selected==2){
                if(totalKaryawan>=MAX_DATA) continue;
                drawFormBox("TAMBAH KARYAWAN");
                gotoxy(formLabelX, formY+4); printf("Nama : "); gotoxy(formInputX, formY+4); getString(dbKaryawan[totalKaryawan].nama,49);
                gotoxy(formLabelX, formY+6); printf("Jabatan : "); gotoxy(formInputX, formY+6); getString(dbKaryawan[totalKaryawan].jabatan,29);
                gotoxy(formLabelX, formY+8); printf("Telp : "); gotoxy(formInputX, formY+8); getString(dbKaryawan[totalKaryawan].telp,19);
                dbKaryawan[totalKaryawan].id = (totalKaryawan>0)? dbKaryawan[totalKaryawan-1].id + 1 : 1; totalKaryawan++; clearRightContent();
            } else if(selected==3 || selected==4){
                if(totalKaryawan==0) continue;
                drawFormBox(selected==3?"EDIT KARYAWAN":"HAPUS KARYAWAN");
                int idTarget, idx=-1; gotoxy(formLabelX, formY+2); printf("Masukkan ID : "); scanf("%d",&idTarget); while((getchar())!='\n');
                for(int i=0;i<totalKaryawan;i++) if(dbKaryawan[i].id==idTarget) idx=i;
                if(idx!=-1){
                    if(selected==3){
                        gotoxy(formLabelX, formY+4); printf("Nama Baru : "); gotoxy(formInputX, formY+4); getString(dbKaryawan[idx].nama,49);
                        gotoxy(formLabelX, formY+6); printf("Jabatan Baru : "); gotoxy(formInputX, formY+6); getString(dbKaryawan[idx].jabatan,29);
                        gotoxy(formLabelX, formY+8); printf("Telp Baru : "); gotoxy(formInputX, formY+8); getString(dbKaryawan[idx].telp,19);
                    } else {
                        for(int j=idx;j<totalKaryawan-1;j++) dbKaryawan[j]=dbKaryawan[j+1];
                        totalKaryawan--;
                    }
                    gotoxy(formLabelX, formY+10); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            }
        }
    }
}

// --------------------------- Pelanggan (Karyawan role) ---------------------------
void crudPelanggan() {
    int selected = 1, totalMenu = 5, key;
    int formLabelX = SIDEBAR_WIDTH + 7, formInputX = SIDEBAR_WIDTH + 25, formY = HEADER_HEIGHT + 5;
    while(1){
        drawFullFrame(); drawHeader();
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Pelanggan", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Pelanggan", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Pelanggan", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Pelanggan", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        int tableX = SIDEBAR_WIDTH + 3; int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf(" %-4s | %-25s | %-40s | %-12s ", "ID","NAMA","ALAMAT","TELP");
        int limit = (totalPelanggan>15)?15:totalPelanggan;
        for(int i=0;i<limit;i++){
            gotoxy(tableX, startY+2+i);
            printf(" %-4d | %-25.25s | %-40.40s | %-12.12s ", dbPelanggan[i].id, dbPelanggan[i].nama, dbPelanggan[i].alamat, dbPelanggan[i].telp);
        }

        hideCursor(); key=getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==5) break;
            else if(selected==2){
                if(totalPelanggan>=MAX_DATA) continue;
                drawFormBox("TAMBAH PELANGGAN");
                gotoxy(formLabelX, formY+4); printf("Nama : "); gotoxy(formInputX, formY+4); getString(dbPelanggan[totalPelanggan].nama,49);
                gotoxy(formLabelX, formY+6); printf("Alamat : "); gotoxy(formInputX, formY+6); getString(dbPelanggan[totalPelanggan].alamat,99);
                gotoxy(formLabelX, formY+8); printf("Telp : "); gotoxy(formInputX, formY+8); getString(dbPelanggan[totalPelanggan].telp,19);
                dbPelanggan[totalPelanggan].id = (totalPelanggan>0)? dbPelanggan[totalPelanggan-1].id + 1 : 1; totalPelanggan++; clearRightContent();
            } else if(selected==3 || selected==4){
                if(totalPelanggan==0) continue;
                drawFormBox(selected==3?"EDIT PELANGGAN":"HAPUS PELANGGAN");
                int idTarget, idx=-1; gotoxy(formLabelX, formY+2); printf("Masukkan ID : "); scanf("%d",&idTarget); while((getchar())!='\n');
                for(int i=0;i<totalPelanggan;i++) if(dbPelanggan[i].id==idTarget) idx=i;
                if(idx!=-1){
                    if(selected==3){
                        gotoxy(formLabelX, formY+4); printf("Nama Baru : "); gotoxy(formInputX, formY+4); getString(dbPelanggan[idx].nama,49);
                        gotoxy(formLabelX, formY+6); printf("Alamat Baru : "); gotoxy(formInputX, formY+6); getString(dbPelanggan[idx].alamat,99);
                        gotoxy(formLabelX, formY+8); printf("Telp Baru : "); gotoxy(formInputX, formY+8); getString(dbPelanggan[idx].telp,19);
                    } else {
                        for(int j=idx;j<totalPelanggan-1;j++) dbPelanggan[j]=dbPelanggan[j+1];
                        totalPelanggan--;
                    }
                    gotoxy(formLabelX, formY+10); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            }
        }
    }
}

// --------------------------- BarangSupplier (Supplier role) ---------------------------
void crudBarangSupplier() {
    int selected = 1, totalMenu = 5, key;
    int formLabelX = SIDEBAR_WIDTH + 7, formInputX = SIDEBAR_WIDTH + 25, formY = HEADER_HEIGHT + 5;
    while(1){
        drawFullFrame(); drawHeader();
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Barang", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Barang", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Barang", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Barang", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        int tableX = SIDEBAR_WIDTH + 3; int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf(" %-4s | %-30s | %-8s ", "ID","NAMA BARANG","QTY");
        int limit = (totalBarangSupplier>15)?15:totalBarangSupplier;
        for(int i=0;i<limit;i++){
            gotoxy(tableX, startY+2+i);
            printf(" %-4d | %-30.30s | %-8d ", dbBarangSupplier[i].id, dbBarangSupplier[i].namaBarang, dbBarangSupplier[i].qty);
        }

        hideCursor(); key=getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==5) break;
            else if(selected==2){
                if(totalBarangSupplier>=MAX_DATA) continue;
                drawFormBox("TAMBAH BARANG SUPPLIER");
                gotoxy(formLabelX, formY+4); printf("Nama Barang : "); gotoxy(formInputX, formY+4); getString(dbBarangSupplier[totalBarangSupplier].namaBarang,49);
                gotoxy(formLabelX, formY+6); printf("Qty : "); gotoxy(formInputX, formY+6); scanf("%d",&dbBarangSupplier[totalBarangSupplier].qty); while((getchar())!='\n');
                dbBarangSupplier[totalBarangSupplier].id = (totalBarangSupplier>0)? dbBarangSupplier[totalBarangSupplier-1].id + 1 : 1; totalBarangSupplier++; clearRightContent();
            } else if(selected==3 || selected==4){
                if(totalBarangSupplier==0) continue;
                drawFormBox(selected==3?"EDIT BARANG":"HAPUS BARANG");
                int idTarget, idx=-1; gotoxy(formLabelX, formY+2); printf("Masukkan ID : "); scanf("%d",&idTarget); while((getchar())!='\n');
                for(int i=0;i<totalBarangSupplier;i++) if(dbBarangSupplier[i].id==idTarget) idx=i;
                if(idx!=-1){
                    if(selected==3){
                        gotoxy(formLabelX, formY+4); printf("Nama Baru : "); gotoxy(formInputX, formY+4); getString(dbBarangSupplier[idx].namaBarang,49);
                        gotoxy(formLabelX, formY+6); printf("Qty Baru : "); gotoxy(formInputX, formY+6); scanf("%d",&dbBarangSupplier[idx].qty); while((getchar())!='\n');
                    } else {
                        for(int j=idx;j<totalBarangSupplier-1;j++) dbBarangSupplier[j]=dbBarangSupplier[j+1];
                        totalBarangSupplier--;
                    }
                    gotoxy(formLabelX, formY+10); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            }
        }
    }
}

// --------------------------- LokasiGudang (Gudang role) ---------------------------
void crudLokasiGudang() {
    int selected = 1, totalMenu = 5, key;
    int formLabelX = SIDEBAR_WIDTH + 7, formInputX = SIDEBAR_WIDTH + 25, formY = HEADER_HEIGHT + 5;
    while(1){
        drawFullFrame(); drawHeader();
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Lokasi", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Lokasi", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Lokasi", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Lokasi", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        int tableX = SIDEBAR_WIDTH + 3; int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf(" %-4s | %-25s | %-25s ", "ID","NAMA GUDANG","LOKASI");
        int limit = (totalLokasiGudang>15)?15:totalLokasiGudang;
        for(int i=0;i<limit;i++){
            gotoxy(tableX, startY+2+i);
            printf(" %-4d | %-25.25s | %-25.25s ", dbLokasiGudang[i].id, dbLokasiGudang[i].namaGudang, dbLokasiGudang[i].lokasi);
        }

        hideCursor(); key=getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==5) break;
            else if(selected==2){
                if(totalLokasiGudang>=MAX_DATA) continue;
                drawFormBox("TAMBAH LOKASI GUDANG");
                gotoxy(formLabelX, formY+4); printf("Nama Gudang : "); gotoxy(formInputX, formY+4); getString(dbLokasiGudang[totalLokasiGudang].namaGudang,49);
                gotoxy(formLabelX, formY+6); printf("Lokasi : "); gotoxy(formInputX, formY+6); getString(dbLokasiGudang[totalLokasiGudang].lokasi,49);
                dbLokasiGudang[totalLokasiGudang].id = (totalLokasiGudang>0)? dbLokasiGudang[totalLokasiGudang-1].id + 1 : 1; totalLokasiGudang++; clearRightContent();
            } else if(selected==3 || selected==4){
                if(totalLokasiGudang==0) continue;
                drawFormBox(selected==3?"EDIT LOKASI":"HAPUS LOKASI");
                int idTarget, idx=-1; gotoxy(formLabelX, formY+2); printf("Masukkan ID : "); scanf("%d",&idTarget); while((getchar())!='\n');
                for(int i=0;i<totalLokasiGudang;i++) if(dbLokasiGudang[i].id==idTarget) idx=i;
                if(idx!=-1){
                    if(selected==3){
                        gotoxy(formLabelX, formY+4); printf("Nama Baru : "); gotoxy(formInputX, formY+4); getString(dbLokasiGudang[idx].namaGudang,49);
                        gotoxy(formLabelX, formY+6); printf("Lokasi Baru : "); gotoxy(formInputX, formY+6); getString(dbLokasiGudang[idx].lokasi,49);
                    } else {
                        for(int j=idx;j<totalLokasiGudang-1;j++) dbLokasiGudang[j]=dbLokasiGudang[j+1];
                        totalLokasiGudang--;
                    }
                    gotoxy(formLabelX, formY+10); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            }
        }
    }
}

// --------------------------- User Management (Super Admin) ---------------------------
void crudUserManagement() {
    int selected = 1, totalMenu = 5, key;
    int formLabelX = SIDEBAR_WIDTH + 7, formInputX = SIDEBAR_WIDTH + 25, formY = HEADER_HEIGHT + 5;
    while(1){
        drawFullFrame(); drawHeader();
        printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat User", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah User", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. Hapus User", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11,"4. Reset Users", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);

        int tableX = SIDEBAR_WIDTH + 3; int startY = HEADER_HEIGHT + 4;
        gotoxy(tableX, startY); printf(" %-4s | %-20s | %-15s ", "ID","USERNAME","ROLE");
        int limit = (totalUser>15)?15:totalUser;
        for(int i=0;i<limit;i++){
            gotoxy(tableX, startY+2+i);
            printf(" %-4d | %-20.20s | %-15.15s ", dbUser[i].id, dbUser[i].username, dbUser[i].roleName);
        }

        hideCursor(); key=getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==5) break;
            else if(selected==2){
                if(totalUser>=MAX_DATA) continue;
                drawFormBox("TAMBAH USER");
                gotoxy(formLabelX, formY+4); printf("Username : "); gotoxy(formInputX, formY+4); getString(dbUser[totalUser].username,29);
                gotoxy(formLabelX, formY+6); printf("RoleName : "); gotoxy(formInputX, formY+6); getString(dbUser[totalUser].roleName,29);
                dbUser[totalUser].id = (totalUser>0)? dbUser[totalUser-1].id + 1 : 1; totalUser++; clearRightContent();
            } else if(selected==3){
                if(totalUser==0) continue;
                drawFormBox("HAPUS USER");
                int idTarget, idx=-1; gotoxy(formLabelX, formY+2); printf("Masukkan ID : "); scanf("%d",&idTarget); while((getchar())!='\n');
                for(int i=0;i<totalUser;i++) if(dbUser[i].id==idTarget) idx=i;
                if(idx!=-1){
                    for(int j=idx;j<totalUser-1;j++) dbUser[j]=dbUser[j+1];
                    totalUser--;
                    gotoxy(formLabelX, formY+6); printf(">> Sukses!");
                } else { gotoxy(formLabelX, formY+4); printf(">> ID Tidak Ada!"); }
                Sleep(800); clearRightContent();
            } else if(selected==4){
                totalUser = 0;
                gotoxy(formLabelX, formY+6); printf(">> Semua user dihapus!");
                Sleep(800); clearRightContent();
            }
        }
    }
}

// =====================================================================================
// ANIMASI & LOGIN (sama seperti sebelumnya, sedikit disesuaikan)
// =====================================================================================
void loadingAnimation() {
    system("cls");
    int midY = 10;
    int midX = 30;
    gotoxy(midX, midY); printf("Sabar ya beb...");
    gotoxy(midX, midY+2);
    for(int i=0; i<40; i++) { printf("%c", 219); Sleep(10); }
    Sleep(200);
}

int roleSelectionScreen() {
    int selected = 1, key;
    while(1) {
        system("cls");
        drawHeader();
        updateScreenSize();
        int boxW = 40, boxH = 12;
        int bx = (screenWidth - boxW)/2, by = (screenHeight - boxH)/2;
        for(int y=by-2; y<=by+boxH+2; y++) {
            gotoxy(bx, y);
            for(int x=0; x<=boxW; x++) {
                if(y==by-2 || y==by+boxH+2) printf("-");
                else if(x==0 || x==boxW) printf("|");
                else printf(" ");
            }
        }
        gotoxy(bx + 9, by); printf("PILIH HAK AKSES");
        printMenuItem(bx + 5, by + 3, "1. KARYAWAN",    selected == 1);
        printMenuItem(bx + 5, by + 5, "2. SUPPLIER",    selected == 2);
        printMenuItem(bx + 5, by + 7, "3. GUDANG",      selected == 3);
        printMenuItem(bx + 5, by + 9, "4. SUPER ADMIN", selected == 4);
        printMenuItem(bx + 5, by + 11, "0. KELUAR PROGRAM", selected == 5);
        int totalMenu = 5;
        key = getch();
        if(key == 224) {
            key = getch();
            if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; }
            if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; }
        }
        else if(key == KEY_ENTER) {
            if(selected == 5) return 0;
            if(selected == 1) return ROLE_KARYAWAN;
            if(selected == 2) return ROLE_SUPPLIER;
            if(selected == 3) return ROLE_GUDANG;
            if(selected == 4) return ROLE_SUPER_ADMIN;
        }
    }
}

int loginScreen(int role) {
    char username[50], password[50], ch;
    int i, show = 0;
    updateScreenSize();
    int boxW = 50, boxH = 12;
    int bx = (screenWidth - boxW)/2, by = (screenHeight - boxH)/2;
    char roleName[20];
    char validUser[20], validPass[20];
    if (role == ROLE_SUPER_ADMIN) { strcpy(roleName, "SUPER ADMIN"); strcpy(validUser, "super"); strcpy(validPass, "super"); }
    else if (role == ROLE_GUDANG) { strcpy(roleName, "ADMIN GUDANG"); strcpy(validUser, "gudang"); strcpy(validPass, "gudang"); }
    else if (role == ROLE_SUPPLIER) { strcpy(roleName, "ADMIN SUPPLIER"); strcpy(validUser, "supplier"); strcpy(validPass, "supplier"); }
    else { strcpy(roleName, "KARYAWAN/KASIR"); strcpy(validUser, "kasir"); strcpy(validPass, "kasir"); }

    while(1){
        system("cls"); drawHeader();
        for(int y=by; y<=by+boxH; y++) {
            gotoxy(bx, y);
            for(int x=0; x<=boxW; x++) {
                if(y==by || y==by+boxH) printf("-");
                else if(x==0 || x==boxW) printf("|");
                else printf(" ");
            }
        }

        textNormal(); gotoxy(bx+15, by+2); printf(" LOGIN : %s ", roleName);
        gotoxy(bx+5, by+4); printf("Username: ");
        gotoxy(bx+5, by+6); printf("Password: ");
        gotoxy(bx+5, by+10); printf("[TAB] Show/Hide | [ESC] Back");
        textNormal();
        gotoxy(bx+15, by+4); getString(username, 49);
        if(strlen(username)==0) continue;
        gotoxy(bx+15, by+6);
        i=0; show=0; memset(password, 0, 50);
        while(1){
            ch = getch();
            if(ch == 13) break;
            if(ch == 27) return 0;
            if(ch == 8){ if(i > 0){ i--; printf("\b \b"); } }
            else if(ch == 9){
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
        if(strcmp(username, validUser)==0 && strcmp(password, validPass)==0) {
            gotoxy(bx+10, by+8); printf(">> Login Berhasil! <<"); Sleep(600);
            return 1;
        } else {
            textError();
            gotoxy(bx+10, by+8); printf(">> Login Gagal! <<   ");
            textNormal();
            Sleep(600);
        }
    }
}

// =====================================================================================
// MENU PER ROLE (memanggil 2 master CRUD minimal)
// =====================================================================================
void menuKaryawan() {
    drawFullFrame(); drawHeader();
    int selected = 1; int key; int totalMenu = 3;
    while(1){
        // Sidebar
        printMenuItem(2, HEADER_HEIGHT + 5, "1. KARYAWAN (Master)", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. PELANGGAN (Master)", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "0. Kembali", selected == 3);

        hideCursor(); key = getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==3) break;
            else if(selected==1) crudKaryawan();
            else if(selected==2) crudPelanggan();
        }
        drawFullFrame(); drawHeader();
    }
}

void menuSupplier() {
    drawFullFrame(); drawHeader();
    int selected = 1, key, totalMenu = 3;
    while(1){
        printMenuItem(2, HEADER_HEIGHT + 5, "1. SUPPLIER (Master)", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. BARANG SUPPLIER (Master)", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "0. Kembali", selected == 3);
        hideCursor(); key = getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==3) break;
            else if(selected==1) crudSupplier();
            else if(selected==2) crudBarangSupplier();
        }
        drawFullFrame(); drawHeader();
    }
}

void menuGudang() {
    drawFullFrame(); drawHeader();
    int selected = 1, key, totalMenu = 3;
    while(1){
        printMenuItem(2, HEADER_HEIGHT + 5, "1. PRODUK (Master)", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. LOKASI GUDANG (Master)", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "0. Kembali", selected == 3);
        hideCursor(); key = getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==3) break;
            else if(selected==1) crudProduk();
            else if(selected==2) crudLokasiGudang();
        }
        drawFullFrame(); drawHeader();
    }
}

void menuSuperAdmin() {
    drawFullFrame(); drawHeader();
    int selected = 1, key;
    int totalMenu = 6;
    while(1){
        printMenuItem(2, HEADER_HEIGHT + 5, "1. MANAGE SUPPLIER", selected == 1);
        printMenuItem(2, HEADER_HEIGHT + 7, "2. MANAGE PRODUK", selected == 2);
        printMenuItem(2, HEADER_HEIGHT + 9, "3. MANAGE USERS", selected == 3);
        printMenuItem(2, HEADER_HEIGHT + 11, "4. MANAGE KARYAWAN", selected == 4);
        printMenuItem(2, HEADER_HEIGHT + 13, "5. MANAGE PELANGGAN", selected == 5);
        printMenuItem(2, HEADER_HEIGHT + 15, "0. Kembali", selected == 6);

        hideCursor(); key=getch();
        if(key==224){ key=getch(); if(key==KEY_UP){ selected--; if(selected<1) selected=totalMenu; } else if(key==KEY_DOWN){ selected++; if(selected>totalMenu) selected=1; } }
        else if(key==KEY_ENTER){
            if(selected==6) break;
            else if(selected==1) crudSupplier();
            else if(selected==2) crudProduk();
            else if(selected==3) crudUserManagement();
            else if(selected==4) crudKaryawan();
            else if(selected==5) crudPelanggan();
        }
        drawFullFrame(); drawHeader();
    }
}

// =====================================================================================
// MAIN PROGRAM
// =====================================================================================
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

    while(IS_RUNNING) {
        CURRENT_ROLE = roleSelectionScreen();
        if(CURRENT_ROLE == 0) { IS_RUNNING = 0; break; }
        if(!loginScreen(CURRENT_ROLE)) continue;

        int selected = 1;
        int totalMenu = 7;
        int key;

        while(1) {
            system("cls");
            drawFullFrame();
            drawHeader();
            int cx = screenWidth / 2;
            int cy = HEADER_HEIGHT + 5;

            gotoxy(cx - 15, cy);
            if(CURRENT_ROLE == ROLE_SUPER_ADMIN) printf("=== PANEL SUPER ADMIN ===");
            else if(CURRENT_ROLE == ROLE_GUDANG) printf("=== MENU ADMIN GUDANG ===");
            else if(CURRENT_ROLE == ROLE_SUPPLIER) printf("=== MENU ADMIN SUPPLIER ===");
            else printf("=== MENU KARYAWAN ===");

            printMenuItem(cx - 15, cy+2, "1. KARYAWAN", selected == 1);
            printMenuItem(cx - 15, cy+3, "2. SUPPLIER", selected == 2);
            printMenuItem(cx - 15, cy+4, "3. GUDANG",   selected == 3);
            printMenuItem(cx - 15, cy+5, "4. PRODUK",   selected == 4);
            printMenuItem(cx - 15, cy+6, "5. SUPER ADMIN", selected == 5);
            printMenuItem(cx - 15, cy+8, "9. LOGOUT",   selected == 6);
            printMenuItem(cx - 15, cy+9, "0. KELUAR PROGRAM", selected == 7);

            gotoxy(cx - 15, cy+11); printf("Navigasi: Panah Atas/Bawah");

            hideCursor();
            key = getch();
            if(key == 224) {
                key = getch();
                if(key == KEY_UP) { selected--; if(selected < 1) selected = totalMenu; }
                else if(key == KEY_DOWN) { selected++; if(selected > totalMenu) selected = 1; }
            }
            else if(key == KEY_ENTER) {
                if(selected == 6) break;
                if(selected == 7) { IS_RUNNING = 0; break; }

                if(selected == 1) {
                    // Semua role bisa mengakses menu karyawan tampilan dasar (tapi edit/add hanya diizinkan pada SuperAdmin)
                    if(CURRENT_ROLE == ROLE_SUPER_ADMIN || CURRENT_ROLE == ROLE_KARYAWAN) menuKaryawan();
                    else accessDenied();
                } else if(selected == 2) {
                    if(CURRENT_ROLE == ROLE_SUPPLIER || CURRENT_ROLE == ROLE_SUPER_ADMIN) menuSupplier();
                    else accessDenied();
                } else if(selected == 3) {
                    if(CURRENT_ROLE == ROLE_GUDANG || CURRENT_ROLE == ROLE_SUPER_ADMIN) menuGudang();
                    else accessDenied();
                } else if(selected == 4) {
                    if(CURRENT_ROLE == ROLE_GUDANG || CURRENT_ROLE == ROLE_SUPER_ADMIN) crudProduk();
                    else accessDenied();
                } else if(selected == 5) {
                    if(CURRENT_ROLE == ROLE_SUPER_ADMIN) menuSuperAdmin();
                    else accessDenied();
                }
            }
        }
    }

    system("cls");
    textNormal();
    gotoxy(1, 1);
    printf("Terima kasih sudah menggunakan program ini. Sampai jumpa!\n\n");
    return 0;
}
