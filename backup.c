#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>

#define MAX_DATA 100
#define SIDEBAR_WIDTH 35
#define HEADER_HEIGHT 8
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_TAB 9
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define COLOR_HOT_PINK 13
#define COLOR_WHITE 15
#define ROWS_PER_PAGE 10

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
int pageOffset = 0;
const int MAX_SCROLL_OFFSET = 50;

COORD getCurrentCursorPosition() {
    COORD coord = {0, 0};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        coord = csbi.dwCursorPosition;
    }
    return coord;
}

void setPinkTheme();
void textNormal();
void goFullscreen();
void maximizeConsole();
void removeScrollbar();
void gotoxy(int x, int y);
void updateScreenSize();
void generateDummyData();
void setCursorVisible(BOOL visible);
void hideCursor();
void getString(char *buffer, int maxLen);
long getLongInput(int inputX, int inputY);
void drawFullFrame();
void drawLoginFrame();
void drawHeader();
void clearRightContent();
void printCenterRight(int y, const char* text);
int getCenterXForTable(int tableWidth);
void drawBaseLayout(const char* sidebarTitle);
void printMenuItem(int x, int y, char* text, int isSelected);
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight);
void drawTableBox(int startX, int startY, int width, int height);
void loadingAnimation();
void getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible);
int loginScreen();
void displaySupplierTable(int tableX);
void displayProdukTable(int tableX);
void crudSupplier();
void crudProduk();
void menuUnderConstruction(char *title);
void drawMainMenuStatic();
void updateMainMenuOptions(int selected);
void updateSubMenu(int selected);

void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[COLOR_HOT_PINK] = RGB(255, 105, 180);
    csbi.ColorTable[COLOR_WHITE] = RGB(255, 255, 255);
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void textNormal() {
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

void removeScrollbar() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    COORD newSize;
    newSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    newSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    SetConsoleScreenBufferSize(hOut, newSize);
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

void generateDummyData() {
    totalSupplier = 0;
    totalProduk = 0;

    const struct {
        const char *nama;
        int stok;
        long harga;
    } new_products[] = {
        {"Beras Premium 5kg", 15, 65000L},
        {"Minyak Goreng 2L", 20, 32500L},
        {"Gula Pasir 1kg", 10, 17000L},
        {"Telur Ayam (Tray)", 50, 54000L},
        {"Mie Instan Kari", 50, 3500L},
        {"Kopi Bubuk 200g", 10, 25000L},
        {"Teh Celup Kotak", 16, 12000L},
        {"Susu UHT Cokelat 1L", 28, 18500L},
        {"Roti Tawar Gandum", 75, 16000L},
        {"Sabun Mandi Cair", 31, 28000L},
        {"Shampoo Anti Ketombe", 14, 45000L},
        {"Deterjen Bubuk 800g", 19, 22000L},
        {"Pasta Gigi Herbal", 25, 14500L},
        {"Air Mineral 1.5L", 40, 6000L},
        {"Saus Tomat Botol", 13, 19000L},
        {"Kecap Manis Besar", 10, 27500L},
        {"Biskuit Kelapa Kaleng", 8, 38000L},
        {"Sereal Sarapan", 12, 49000L},
        {"Keju Cheddar Blok", 95, 35500L},
        {"Margarin 250g", 21, 11000L},
        {"Tepung Terigu 1kg", 17, 13000L},
        {"Garam Meja Beryodium", 35, 4000L},
        {"Daging Ayam Beku", 6, 48000L},
        {"Ikan Tuna Kaleng", 15, 18000L},
        {"Sosis Sapi Pack", 11, 30000L},
        {"Yogurt Plain Cup", 40, 8500L},
        {"Minuman Soda Kaleng", 30, 7000L},
        {"Buah Apel Merah (kg)", 55, 34000L},
        {"Sayur Bayam (ikat)", 70, 5000L},
        {"Sabun Cuci Piring", 20, 16500L},
        {"Pembalut Wanita", 13, 29000L},
        {"Popok Bayi Pack M", 80, 95000L},
        {"Tisu Wajah Kotak", 19, 15000L},
        {"Lilin Aromaterapi", 5, 42000L},
        {"Pembersih Lantai", 15, 23500L},
        {"Obat Nyamuk Spray", 15, 36000L},
        {"Snack Kentang Besar", 20, 19500L},
        {"Permen Mint Bag", 2, 9000L},
        {"Cokelat Batangan", 60, 11500L},
        {"Susu Bubuk Anak", 70, 88000L},
        {"Kopi Instan Sachet", 50, 28000L},
        {"Madu Murni Botol", 50, 75000L},
        {"Cuka Dapur", 165, 7500L},
        {"Merica Bubuk", 15, 15500L},
        {"Minyak Zaitun", 45, 110000L},
        {"Mayones Botol", 90, 25000L},
        {"Es Krim Keluarga", 10, 52000L},
        {"Jagung Kaleng", 15, 10500L},
        {"Deterjen Cair 1L", 15, 33000L},
        {"Pewangi Pakaian", 25, 20000L}
    };

    int num_user_products = 50;

    for (int i = 0; i < num_user_products; i++) {
        dbProduk[i].id = i + 1;
        strncpy(dbProduk[i].nama, new_products[i].nama, 50);
        dbProduk[i].stok = new_products[i].stok;
        dbProduk[i].harga = new_products[i].harga;
        totalProduk++;
    }

    dbProduk[50].id = 51;
    strcpy(dbProduk[50].nama, "Tambahan Produk A (Stok Extra)");
    dbProduk[50].stok = 999;
    dbProduk[50].harga = 50000L;
    totalProduk++;

    dbProduk[51].id = 52;
    strcpy(dbProduk[51].nama, "Tambahan Produk B (Promo)");
    dbProduk[51].stok = 120;
    dbProduk[51].harga = 12000L;
    totalProduk++;

    dbProduk[52].id = 53;
    strcpy(dbProduk[52].nama, "Tambahan Produk C (Import)");
    dbProduk[52].stok = 80;
    dbProduk[52].harga = 180000L;
    totalProduk++;

    dbProduk[53].id = 54;
    strcpy(dbProduk[53].nama, "Tambahan Produk D");
    dbProduk[53].stok = 10;
    dbProduk[53].harga = 5000L;
    totalProduk++;

    dbProduk[54].id = 55;
    strcpy(dbProduk[54].nama, "Tambahan Produk E (Grosir)");
    dbProduk[54].stok = 50;
    dbProduk[54].harga = 75000L;
    totalProduk++;

    const char *new_suppliers[][2] = {
        {"PT Cipta Jaya", "Jl. Raya No. 12"},
        {"CV Makmur Abadi", "Komplek Bunga Blok C"},
        {"Sinar Logistik", "Pergudangan Utara"},
        {"Global Teknik", "Ruko Sentosa 3A"},
        {"Prima Kimia", "Kawasan Industri A"},
        {"Indo Perkasa", "Jl. Veteran 15"},
        {"Karya Bersama", "Pasar Baru Lantai 2"},
        {"Jaya Sentosa", "Desa Sukamaju RT 5"},
        {"Multi Boga", "Area Kuliner Blok K"},
        {"Mitra Utama", "Gudang Hijau"},
        {"PT Dirgantara", "Jl. Garuda No. 8"},
        {"CV Tirta Sari", "Komplek Air Biru"},
        {"Surya Elektronik", "Pusat Elektronik"},
        {"Agung Furnitur", "Jl. Kayu Jati 22"},
        {"Nusa Media", "Ruko Digital Blok Z"},
        {"PT Bahari", "Pelabuhan Baru"},
        {"CV Lestari", "Kebun Raya"},
        {"Mutiara Tekstil", "Sentra Garmen"},
        {"Cahaya Semesta", "Jl. Angkasa No. 9"},
        {"Tani Makmur", "Desa Padi"},
        {"PT Andalan", "Jl. Utama No. 45"},
        {"CV Sejahtera", "Komplek Damai"},
        {"Bintang Baja", "Industri Berat"},
        {"Prima Food", "Sentra Makanan"},
        {"Omega Solusi", "Ruko Bisnis 1B"},
        {"PT Samudra", "Dermaga Lama"},
        {"CV Elang", "Bukit Tinggi"},
        {"Diamond Kaca", "Jl. Kristal 5"},
        {"Mega Beton", "Proyek Konstruksi"},
        {"Tirta Murni", "Sumber Air"},
        {"PT Delta", "Jl. Kaki No. 11"},
        {"CV Emas", "Tambang Kecil"},
        {"Trans Logistik", "Terminal Kargo"},
        {"Sentra Olahraga", "Mall Sport Lantai 3"},
        {"PT Permata", "Blok Berlian"},
        {"CV Satria", "Barisan Prajurit"},
        {"Bumi Hijau", "Kebun Sayur"},
        {"Harapan Baru", "Jl. Pencerahan"},
        {"Inti Komponen", "Pusat Onderdil"},
        {"PT Globalindo", "Zona Bebas Pajak"},
        {"CV Pelangi", "Warna-Warni Blok F"},
        {"Aroma Kopi", "Perkebunan Gayo"},
        {"Multi Guna", "Kantor Pusat"},
        {"PT Timur Jaya", "Gerbang Timur"},
        {"CV Barat Sentosa", "Gerbang Barat"},
        {"Lintas Cepat", "Jl. Tol Km 10"},
        {"Sigma Energi", "Stasiun Listrik"},
        {"PT Indah", "Taman Asri"},
        {"CV Harmoni", "Nada Indah"},
        {"Dunia Mainan", "Toko Hobi"}
    };

    for (int i = 0; i < 50; i++) {
        dbSupplier[i].id = 1000 + i;
        strncpy(dbSupplier[i].nama, new_suppliers[i][0], 50);
        strncpy(dbSupplier[i].alamat, new_suppliers[i][1], 100);
        totalSupplier++;
    }

    dbSupplier[50].id = 1050;
    strcpy(dbSupplier[50].nama, "PT. Tambahan Logistik A");
    strcpy(dbSupplier[50].alamat, "Gudang Cadangan 01");
    totalSupplier++;

    dbSupplier[51].id = 1051;
    strcpy(dbSupplier[51].nama, "CV. Extra Supplier B");
    strcpy(dbSupplier[51].alamat, "Komplek Extra Block B");
    totalSupplier++;
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

    COORD currentPos = getCurrentCursorPosition();
    int currentX = currentPos.X;
    int currentY = currentPos.Y;

    gotoxy(currentX, currentY);
    for (int i = 0; i < maxLen; i++) printf(" ");
    gotoxy(currentX, currentY);

    fflush(stdin);
    fgets(buffer, maxLen, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    setCursorVisible(FALSE);
}

long getLongInput(int inputX, int inputY) {
    char buffer[20];
    long result = -1;

    setCursorVisible(TRUE);
    gotoxy(inputX, inputY);

    for (int i = 0; i < 19; i++) printf(" ");
    gotoxy(inputX, inputY);

    fflush(stdin);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        result = atol(buffer);
    }
    setCursorVisible(FALSE);
    return result;
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

    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);

    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
    gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", 203);
    gotoxy(SIDEBAR_WIDTH, bottom); printf("%c", 202);
}

void drawLoginFrame() {
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
    }

    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);

    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
}

void drawHeader() {
    int startX = (screenWidth - 85) / 2; if (startX < 1) startX = 1;
    textNormal();
    gotoxy(startX, 1); printf("  _____ _    _ _____  ______ _____  __  __           _____  _  ________ _________ ");
    gotoxy(startX, 2); printf(" / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__   __|");
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
    textNormal();
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(1, y);
        for(int x = 1; x < SIDEBAR_WIDTH; x++) printf(" ");
    }

    gotoxy(2, HEADER_HEIGHT + 2);
    printf("     MENU");
    gotoxy(2, HEADER_HEIGHT + 3);
    printf("   %-20s", sidebarTitle);
}

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

void drawTableBox(int startX, int startY, int width, int height) {
    textNormal();
    for(int x = startX + 1; x < startX + width; x++) {
        gotoxy(x, startY); printf("%c", 205);
        gotoxy(x, startY + height); printf("%c", 205);
    }
    for(int y = startY + 1; y < startY + height; y++) {
        gotoxy(startX, y); printf("%c", 186);
        gotoxy(startX + width, y); printf("%c", 186);
    }
    gotoxy(startX, startY); printf("%c", 201);
    gotoxy(startX + width, startY); printf("%c", 187);
    gotoxy(startX, startY + height); printf("%c", 200);
    gotoxy(startX + width, startY + height); printf("%c", 188);
}


void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight) {
    clearRightContent();
    *boxWidth = 60;
    *boxHeight = 15;
    *startX = SIDEBAR_WIDTH + 5;
    *startY = HEADER_HEIGHT + 3;

    textNormal();
    gotoxy(*startX + 2, *startY); printf(" %s ", title);

    for(int x=*startX; x<*startX+*boxWidth; x++) {
        gotoxy(x, *startY); printf("%c", 205);
        gotoxy(x, *startY+*boxHeight); printf("%c", 205);
    }
    for(int y=*startY+1; y<*startY+*boxHeight; y++) {
        gotoxy(*startX, y); printf("%c", 186);
        gotoxy(*startX+*boxWidth, y); printf("%c", 186);
    }
    gotoxy(*startX, *startY); printf("%c", 201);
    gotoxy(*startX+*boxWidth, *startY); printf("%c", 187);
    gotoxy(*startX, *startY+*boxHeight); printf("%c", 200);
    gotoxy(*startX+*boxWidth, *startY+*boxHeight); printf("%c", 188);
}

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

        if (key == KEY_ENTER || key == 0) {
            buffer[i] = '\0';
            break;
        } else if (key == 8) {
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                gotoxy(inputX + i, inputY); printf(" ");
                gotoxy(inputX + i, inputY);
            }
        } else if (key == KEY_TAB) {
            *isVisible = !(*isVisible);
            gotoxy(inputX, inputY);
            for(int j=0; j<maxLen-1; j++) printf(" ");
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

int loginScreen() {
    char username[20] = "";
    char password[20] = "";
    int loginX, loginY, boxW, boxH;

    while(1) {
        system("cls");
        drawLoginFrame();
        drawHeader();

        updateScreenSize();
        boxW = 50;
        boxH = 10;
        loginX = (screenWidth - boxW) / 2;
        loginY = (screenHeight - boxH) / 2;
        if(loginY <= HEADER_HEIGHT) loginY = HEADER_HEIGHT + 4;

        textNormal();

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

        gotoxy(loginX + (boxW/2) - (int)strlen("LOGIN")/2, loginY - 2);
        printf("LOGIN");

        int labelX = loginX + 5;
        int inputX = loginX + 17;
        int labelY = loginY + 3;

        gotoxy(labelX, labelY);    printf("Username :");
        gotoxy(labelX, labelY+1); printf("Password :");
        gotoxy(labelX, labelY+2); printf("Tekan Tab untuk lihat password");

        gotoxy(inputX, labelY);   printf("                   ");
        gotoxy(inputX, labelY+1); printf("                   ");

        setCursorVisible(TRUE);
        gotoxy(inputX, labelY); getString(username, 19);
        int isVisible = 0;
        gotoxy(inputX, labelY+1); getPassword(password, 19, inputX, labelY+1, &isVisible);

        if (strcmp(username, "123") == 0 && strcmp(password, "123") == 0) {
            gotoxy(loginX + 5, loginY + 8);
            printf("Login Berhasil! Memuat Menu Utama...");
            Sleep(1000);
            setCursorVisible(FALSE);
            return 1;
        } else {
            gotoxy(loginX + 5, loginY + 8);
            printf("Nama Pengguna/Sandi Salah! (ESC Keluar)");
            setCursorVisible(FALSE);
            while(1) {
                int key = getch();
                if (key == KEY_ESC) return 0;
                if (key == KEY_ENTER) break;
            }
        }
    }
}

void displaySupplierTable(int tableX) {
    const int TABLE_WIDTH = 95;
    const int TABLE_HEIGHT = 12;
    const int NAME_WIDTH = 30;
    const int ADDRESS_WIDTH = 50;
    int startY = HEADER_HEIGHT + 4;

    int totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
    if (totalPages == 1 && totalSupplier == 0) totalPages = 0;
    if (totalPages == 0) totalPages = 1;
    int currentPage = pageOffset + 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA SUPPLIER ===");

    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    for(int i=0; i<TABLE_HEIGHT; i++) {
        gotoxy(tableX, startY+i);
        for(int x=0; x<TABLE_WIDTH; x++) printf(" ");
    }

    textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-*s %c %-*s ", "ID", 186, NAME_WIDTH, "NAMA SUPPLIER", 186, ADDRESS_WIDTH, "ALAMAT");
    gotoxy(tableX, startY+1);

    gotoxy(tableX, startY + 1);
    printf("%c%c%c%c%c%c", 204, 205, 205, 205, 205, 205);

    gotoxy(tableX + 7, startY + 1); printf("%c", 206);

    gotoxy(tableX + 7 + NAME_WIDTH + 1, startY + 1); printf("%c", 206);

    for(int x = tableX + 8; x < tableX + 7 + NAME_WIDTH + 1; x++) gotoxy(x, startY + 1); printf("%c", 205);

    for(int x = tableX + 7 + NAME_WIDTH + 2; x < tableX + TABLE_WIDTH + 1; x++) gotoxy(x, startY + 1); printf("%c", 205);

    gotoxy(tableX + TABLE_WIDTH + 1, startY + 1); printf("%c", 185);

    if (totalSupplier == 0) {
        gotoxy(tableX + (TABLE_WIDTH / 2) - 10, startY + 3);
        printf(">> DATA MASIH KOSONG <<");
    } else {
        int endIndex = startIndex + ROWS_PER_PAGE;
        if (endIndex > totalSupplier) endIndex = totalSupplier;

        for(int i = startIndex; i < endIndex; i++) {

            int displayRow = startY + 2 + (i - startIndex);

            char nama_disp[NAME_WIDTH + 1];
            char alamat_disp[ADDRESS_WIDTH + 1];

            strncpy(nama_disp, dbSupplier[i].nama, NAME_WIDTH);
            nama_disp[NAME_WIDTH] = '\0';

            strncpy(alamat_disp, dbSupplier[i].alamat, ADDRESS_WIDTH);
            alamat_disp[ADDRESS_WIDTH] = '\0';

            gotoxy(tableX, displayRow);
            printf(" %-5d %c %-*s %c %-*s ",
                   dbSupplier[i].id, 186,
                   NAME_WIDTH, nama_disp, 186,
                   ADDRESS_WIDTH, alamat_disp);
        }
    }

    if (totalSupplier > 0) {
        gotoxy(tableX + (TABLE_WIDTH / 2) - 18, startY + TABLE_HEIGHT + 2);
        printf(" PAGING: [ ^ ATAS | BAWAH v ] | Halaman %d / %d", currentPage, totalPages);
    }
}

void displayProdukTable(int tableX) {
    const int TABLE_WIDTH = 95;
    const int TABLE_HEIGHT = 12;
    const int NAME_WIDTH = 40;
    int startY = HEADER_HEIGHT + 4;

    int totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
    if (totalPages == 1 && totalProduk == 0) totalPages = 0;
    if (totalPages == 0) totalPages = 1;
    int currentPage = pageOffset + 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA STOK PRODUK ===");

    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    for(int i=0; i<TABLE_HEIGHT; i++) {
        gotoxy(tableX, startY+i);
        for(int x=0; x<TABLE_WIDTH; x++) printf(" ");
    }

    textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-*s %c %-10s %c %-20s ", "ID", 186, NAME_WIDTH, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
    gotoxy(tableX, startY+1);

    gotoxy(tableX, startY + 1);
    printf("%c%c%c%c%c%c", 204, 205, 205, 205, 205, 205);

    gotoxy(tableX + 7, startY + 1); printf("%c", 206);

    gotoxy(tableX + 7 + NAME_WIDTH + 1, startY + 1); printf("%c", 206);

    gotoxy(tableX + 7 + NAME_WIDTH + 1 + 12, startY + 1); printf("%c", 206);

    for(int x = tableX + 8; x < tableX + TABLE_WIDTH + 1; x++) gotoxy(x, startY + 1); printf("%c", 205);

    gotoxy(tableX + TABLE_WIDTH + 1, startY + 1); printf("%c", 185);

    if (totalProduk == 0) {
        gotoxy(tableX + (TABLE_WIDTH / 2) - 10, startY + 3);
        printf(">> DATA MASIH KOSONG <<");
    } else {
        int endIndex = startIndex + ROWS_PER_PAGE;
        if (endIndex > totalProduk) endIndex = totalProduk;

        for(int i = startIndex; i < endIndex; i++) {

            int displayRow = startY + 2 + (i - startIndex);
            char nama_disp[NAME_WIDTH + 1];

            strncpy(nama_disp, dbProduk[i].nama, NAME_WIDTH);
            nama_disp[NAME_WIDTH] = '\0';

            gotoxy(tableX, displayRow);
            printf(" %-5d %c %-*s %c %-10d %c Rp. %-15ld ",
                   dbProduk[i].id, 186,
                   NAME_WIDTH, nama_disp, 186,
                   dbProduk[i].stok, 186,
                   dbProduk[i].harga);
        }
    }

    if (totalProduk > 0) {
        gotoxy(tableX + (TABLE_WIDTH / 2) - 18, startY + TABLE_HEIGHT + 2);
        printf(" PAGING: [ ^ ATAS | BAWAH v ] | Halaman %d / %d", currentPage, totalPages);
    }
}

void updateSubMenu(int selected) {
    printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Data", selected == 1);
    printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Data", selected == 2);
    printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Data", selected == 3);
    printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Data", selected == 4);
    printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);
}

void crudSupplier() {
    int selected = 1;
    int totalMenu = 5;
    int key;
    drawBaseLayout("MANAJEMEN SUPPLIER");
    const int TABLE_WIDTH = 95;
    int tableX = getCenterXForTable(TABLE_WIDTH);
    int formBoxX, formBoxY, boxW, boxH;
    int formX, formInputX = SIDEBAR_WIDTH + 25;
    int formY;
    int isPagingMode = 0;

    int totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    pageOffset = 0;
    clearRightContent();
    updateSubMenu(selected);

    while(1) {
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            if(isPagingMode) {
                if(key == KEY_UP) {
                    if (pageOffset > 0) {
                        pageOffset--;
                        clearRightContent();
                        displaySupplierTable(tableX);
                        updateSubMenu(selected);
                    }
                }
                else if(key == KEY_DOWN) {
                    if (pageOffset < totalPages - 1) {
                        pageOffset++;
                        clearRightContent();
                        displaySupplierTable(tableX);
                        updateSubMenu(selected);
                    }
                }
            }
            else {
                 if(key == KEY_UP) {
                    selected--;
                    if(selected < 1) selected = totalMenu;
                    updateSubMenu(selected);
                }
                else if(key == KEY_DOWN) {
                    selected++;
                    if(selected > totalMenu) selected = 1;
                    updateSubMenu(selected);
                }
            }
        }
        else if(key == KEY_ENTER) {
            formBoxX = 0; formBoxY = 0; boxW = 0; boxH = 0;
            formX = SIDEBAR_WIDTH + 7;
            formY = HEADER_HEIGHT + 5;

            if(selected == 5) break;

            pageOffset = 0;
            isPagingMode = 0;

            if(selected == 1) {
                 totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
                 if (totalPages == 0) totalPages = 1;
                 clearRightContent();
                 displaySupplierTable(tableX);
                 isPagingMode = 1;
            }

            else if(selected == 2) {
                if(totalSupplier >= MAX_DATA) {
                    printCenterRight(HEADER_HEIGHT + 20, "DATABASE SUPPLIER PENUH!"); Sleep(1000);
                    clearRightContent();
                    updateSubMenu(selected);
                    continue;
                }
                drawFormBox("TAMBAH SUPPLIER BARU", &formBoxX, &formBoxY, &boxW, &boxH);
                setCursorVisible(TRUE);

                int newId = 1;
                if(totalSupplier > 0) newId = dbSupplier[totalSupplier-1].id + 1;

                dbSupplier[totalSupplier].id = newId;
                gotoxy(formX, formY+2); printf("ID Otomatis  : %d", dbSupplier[totalSupplier].id);

                gotoxy(formX, formY+4); printf("Nama Supplier : ");
                gotoxy(formInputX, formY+4); getString(dbSupplier[totalSupplier].nama, 49);

                gotoxy(formX, formY+6); printf("Alamat        : ");
                gotoxy(formInputX, formY+6); getString(dbSupplier[totalSupplier].alamat, 99);

                if(strlen(dbSupplier[totalSupplier].nama) == 0 || strlen(dbSupplier[totalSupplier].alamat) == 0) {
                    printCenterRight(formY+12, "Input dibatalkan (Nama/Alamat kosong).");
                    Sleep(1000);
                    clearRightContent();
                    updateSubMenu(selected);
                    continue;
                }

                totalSupplier++;
                totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
                setCursorVisible(FALSE);
                gotoxy(formX, formY+10); printf(">> SUKSES! Supplier berhasil disimpan.");
                Sleep(1000);
                clearRightContent();
                updateSubMenu(selected);
            }
            else if (selected == 3) {
                if(totalSupplier == 0) { clearRightContent(); updateSubMenu(selected); continue; }
                drawFormBox("EDIT DATA SUPPLIER", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;

                gotoxy(formX, formY+2); printf("Masukkan ID yang akan diedit: ");
                idTarget = (int)getLongInput(formInputX, formY+2);

                for(int i=0; i<totalSupplier; i++) {
                    if(dbSupplier[i].id == idTarget) { idx = i; break; }
                }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("ID Supplier: %d", dbSupplier[idx].id);
                    gotoxy(formX, formY+6); printf("Nama Lama  : %s", dbSupplier[idx].nama);
                    gotoxy(formX, formY+7); printf("Nama Baru  : ");
                    gotoxy(formX, formY+9); printf("Alamat Lama: %s", dbSupplier[idx].alamat);
                    gotoxy(formX, formY+10); printf("Alamat Baru: ");

                    gotoxy(formInputX, formY+7); getString(dbSupplier[idx].nama, 49);
                    gotoxy(formInputX, formY+10); getString(dbSupplier[idx].alamat, 99);

                    setCursorVisible(FALSE);
                    gotoxy(formX, formY+13); printf(">> Data berhasil diperbarui!");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID %d Tidak Ditemukan!", idTarget);
                }
                Sleep(1500);
                clearRightContent();
                updateSubMenu(selected);
            }
            else if (selected == 4) {
                if(totalSupplier == 0) { clearRightContent(); updateSubMenu(selected); continue; }
                drawFormBox("HAPUS DATA SUPPLIER", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;

                gotoxy(formX, formY+2); printf("Masukkan ID untuk dihapus: ");
                idTarget = (int)getLongInput(formInputX, formY+2);

                for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == idTarget) { idx = i; break; }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("Menghapus Supplier ID %d: %s...", dbSupplier[idx].id, dbSupplier[idx].nama);
                    for(int j=idx; j<totalSupplier-1; j++) {
                        dbSupplier[j] = dbSupplier[j+1];
                    }
                    totalSupplier--;
                    totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
                    gotoxy(formX, formY+6); printf(">> Data Terhapus Permanen.");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID Tidak Ditemukan!");
                }
                Sleep(1500);
                clearRightContent();
                updateSubMenu(selected);
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) {
                clearRightContent();
                isPagingMode = 0;
            }

            if (selected == 5) {
                break;
            }
        }
    }
}

void crudProduk() {
    int selected = 1;
    int totalMenu = 5;
    int key;
    drawBaseLayout("MANAJEMEN PRODUK");
    const int TABLE_WIDTH = 95;
    int tableX = getCenterXForTable(TABLE_WIDTH);
    int formBoxX, formBoxY, boxW, boxH;
    int formX, formInputX = SIDEBAR_WIDTH + 25;
    int formY = HEADER_HEIGHT + 5;
    int isPagingMode = 0;

    int totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    pageOffset = 0;
    clearRightContent();
    updateSubMenu(selected);

    while(1) {
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            if(isPagingMode) {
                if(key == KEY_UP) {
                    if (pageOffset > 0) {
                        pageOffset--;
                        clearRightContent();
                        displayProdukTable(tableX);
                        updateSubMenu(selected);
                    }
                }
                else if(key == KEY_DOWN) {
                    if (pageOffset < totalPages - 1) {
                        pageOffset++;
                        clearRightContent();
                        displayProdukTable(tableX);
                        updateSubMenu(selected);
                    }
                }
            }
            else {
                 if(key == KEY_UP) {
                    selected--;
                    if(selected < 1) selected = totalMenu;
                    updateSubMenu(selected);
                }
                else if(key == KEY_DOWN) {
                    selected++;
                    if(selected > totalMenu) selected = 1;
                    updateSubMenu(selected);
                }
            }
        }
        else if(key == KEY_ENTER) {
            formBoxX = 0; formBoxY = 0; boxW = 0; boxH = 0;
            formX = SIDEBAR_WIDTH + 7;
            formY = HEADER_HEIGHT + 5;

            if(selected == 5) break;

            pageOffset = 0;
            isPagingMode = 0;

            if(selected == 1) {
                 totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
                 if (totalPages == 0) totalPages = 1;
                 clearRightContent();
                 displayProdukTable(tableX);
                 isPagingMode = 1;
            }

            else if (selected == 2) {
                 if(totalProduk >= MAX_DATA) {
                    printCenterRight(HEADER_HEIGHT + 20, "DATABASE PRODUK PENUH!"); Sleep(1000);
                    clearRightContent();
                    updateSubMenu(selected);
                    continue;
                 }
                 drawFormBox("TAMBAH PRODUK BARU", &formBoxX, &formBoxY, &boxW, &boxH);
                 setCursorVisible(TRUE);
                 int newId = 1;
                 if(totalProduk > 0) newId = dbProduk[totalProduk-1].id + 1;
                 dbProduk[totalProduk].id = newId;
                 gotoxy(formX, formY+2); printf("ID Otomatis  : %d", dbProduk[totalProduk].id);

                 gotoxy(formX, formY+4); printf("Nama Produk : ");
                 gotoxy(formInputX, formY+4); getString(dbProduk[totalProduk].nama, 49);

                 gotoxy(formX, formY+6); printf("Stok Awal    : ");
                 dbProduk[totalProduk].stok = (int)getLongInput(formInputX, formY+6);

                 gotoxy(formX, formY+8); printf("Harga Satuan: ");
                 dbProduk[totalProduk].harga = getLongInput(formInputX, formY+8);

                 if(strlen(dbProduk[totalProduk].nama) == 0 || dbProduk[totalProduk].stok < 0 || dbProduk[totalProduk].harga < 0) {
                     printCenterRight(formY+12, "Input dibatalkan (Data tidak valid).");
                     Sleep(1000);
                     clearRightContent();
                     updateSubMenu(selected);
                     continue;
                 }

                 totalProduk++;
                 totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
                 setCursorVisible(FALSE);
                 gotoxy(formX, formY+10); printf(">> SUKSES! Produk berhasil disimpan.");
                 Sleep(1000);
                 clearRightContent();
                 updateSubMenu(selected);
            }
            else if (selected == 3) {
                if(totalProduk == 0) { clearRightContent(); updateSubMenu(selected); continue; }
                drawFormBox("EDIT DATA PRODUK", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;

                gotoxy(formX, formY+2); printf("Masukkan ID yang akan diedit: ");
                idTarget = (int)getLongInput(formInputX, formY+2);

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

                    gotoxy(formInputX, formY+5); getString(dbProduk[idx].nama, 49);

                    gotoxy(formInputX, formY+8);
                    dbProduk[idx].stok = (int)getLongInput(formInputX, formY+8);

                    gotoxy(formInputX, formY+11);
                    dbProduk[idx].harga = getLongInput(formInputX, formY+11);

                    setCursorVisible(FALSE);
                    gotoxy(formX, formY+13); printf(">> Data berhasil diperbarui!");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID %d Tidak Ditemukan!", idTarget);
                }
                Sleep(1500);
                clearRightContent();
                updateSubMenu(selected);
            }
            else if (selected == 4) {
                if(totalProduk == 0) { clearRightContent(); updateSubMenu(selected); continue; }
                drawFormBox("HAPUS DATA PRODUK", &formBoxX, &formBoxY, &boxW, &boxH);
                int idTarget, idx = -1;

                gotoxy(formX, formY+2); printf("Masukkan ID untuk dihapus: ");
                idTarget = (int)getLongInput(formInputX, formY+2);

                for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == idTarget) { idx = i; break; }
                if(idx != -1) {
                    gotoxy(formX, formY+4); printf("Menghapus Produk ID %d: %s...", dbProduk[idx].id, dbProduk[idx].nama);
                    for(int j=idx; j<totalProduk-1; j++) {
                        dbProduk[j] = dbProduk[j+1];
                    }
                    totalProduk--;
                    totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
                    gotoxy(formX, formY+6); printf(">> Data Terhapus Permanen.");
                } else {
                    gotoxy(formX, formY+4); printf(">> ID Tidak Ditemukan!");
                }
                Sleep(1500);
                clearRightContent();
                updateSubMenu(selected);
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) {
                clearRightContent();
                isPagingMode = 0;
            }

            if (selected == 5) {
                break;
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

void drawMainMenuStatic() {
    system("cls");
    drawLoginFrame();
    drawHeader();

    updateScreenSize();

    int boxW = 50;
    int boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    textNormal();

    for(int x=menuX; x<menuX+boxW; x++) {
        gotoxy(x, menuY); printf("%c", 205);
        gotoxy(x, menuY+boxH); printf("%c", 205);
    }
    for(int y=menuY+1; y<menuY+boxH; y++) {
        gotoxy(menuX, y); printf("%c", 186);
        gotoxy(menuX+boxW, y); printf("%c", 186);
    }
    gotoxy(menuX, menuY); printf("%c", 201);
    gotoxy(menuX+boxW, menuY); printf("%c", 187);
    gotoxy(menuX, menuY+boxH); printf("%c", 200);
    gotoxy(menuX+boxW, menuY+boxH); printf("%c", 188);

    char* title = "=== MENU UTAMA SYSTEM ===";
    gotoxy(menuX + (boxW - strlen(title))/2, menuY + 2);
    printf("%s", title);

    char* footer = "Gunakan Panah Atas/Bawah";
    gotoxy(menuX + (boxW - strlen(footer))/2, menuY + boxH - 2);
    printf("%s", footer);
}

void updateMainMenuOptions(int selected) {
    updateScreenSize();
    int boxW = 50;
    int boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    int startY = menuY + 5;
    int itemX = menuX + (boxW/2) - 12;

    printMenuItem(itemX, startY,     "1. KARYAWAN", selected == 1);
    printMenuItem(itemX, startY + 1, "2. SUPPLIER", selected == 2);
    printMenuItem(itemX, startY + 2, "3. GUDANG",   selected == 3);
    printMenuItem(itemX, startY + 3, "4. PRODUK",   selected == 4);
    printMenuItem(itemX, startY + 4, "0. KELUAR",   selected == 5);
}

int main() {
    maximizeConsole();
    setPinkTheme();
    system("color DF");

    generateDummyData();

    loadingAnimation();

    goFullscreen();
    removeScrollbar();

    updateScreenSize();
    hideCursor();

    if (!loginScreen()) {
        return 0;
    }

    int selected = 1;
    int totalMenu = 5;
    int key;

    drawMainMenuStatic();
    updateMainMenuOptions(selected);

    while(1) {
        hideCursor();
        key = getch();

        if(key == 224) {
            key = getch();
            if(key == KEY_UP) {
                selected--;
                if(selected < 1) selected = totalMenu;
                updateMainMenuOptions(selected);
            }
            else if(key == KEY_DOWN) {
                selected++;
                if(selected > totalMenu) selected = 1;
                updateMainMenuOptions(selected);
            }
        }
        else if(key == KEY_ENTER) {
            switch(selected) {
                case 1: menuUnderConstruction("KARYAWAN"); break;
                case 2: crudSupplier(); break;
                case 3: menuUnderConstruction("GUDANG"); break;
                case 4: crudProduk(); break;
                case 5: return 0;
            }
            drawMainMenuStatic();
            updateMainMenuOptions(selected);
        }
        else if (key == KEY_ESC) {
            if (selected != 5) {
                selected = 5;
                updateMainMenuOptions(selected);
            } else {
                return 0;
            }
        }
    }
    return 0;
}