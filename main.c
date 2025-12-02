#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

int headerX = 0;  // Posisi X header global
int consoleWidth = 0; // Lebar konsol akan diisi di printHeader

// --- KONSTANTA UNTUK PEMUSATAN ---
#define BOX_WIDTH 60

// ======================================================================
//                  ANIMASI STARTUP + FULLSCREEN (Tidak Berubah)
// ======================================================================

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

void setConsoleTitleBar() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };

    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[0] = RGB(0, 0, 0);
    csbi.ColorTable[15] = RGB(255, 255, 255);
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void loadingAnimation() {
    const char *frames[] = {
        "[=         ]", "[==        ]", "[===       ]",
        "[====      ]", "[=====     ]", "[======    ]",
        "[=======   ]", "[========  ]", "[========= ]",
        "[==========]"
    };
    int total = sizeof(frames) / sizeof(frames[0]);

    system("cls");
    printf("\n Memanggil dimensi fullscreen...\n\n");

    for(int i = 0; i < total; i++) {
        printf(" %s", frames[i]);
        fflush(stdout);
        Sleep(80);
        printf("\r");
    }
    Sleep(150);
}

void pulseFlash() {
    for (int i = 0; i < 3; i++) {
        system("color F0");
        Sleep(80);
        system("color 0F");
        Sleep(80);
    }
    system("color F0");
}

void startupSequence() {
    maximizeConsole();
    setConsoleTitleBar();
    loadingAnimation();
    pulseFlash();
    goFullscreen();
    Sleep(200);
    system("cls");
}

// ======================================================================
//                        HELPER FUNCTIONS UI
// ======================================================================

void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void drawBox(int x, int y, int width, int height) {
    // Pastikan area di dalam kotak di-clear
    for(int i = 1; i <= height; i++) {
        gotoxy(x + 1, y + i);
        for(int j = 0; j < width; j++) printf(" ");
    }

    gotoxy(x, y); printf("%c", 218);
    for(int i=0;i<width;i++) printf("%c", 196);
    printf("%c", 191);

    for(int i=1;i<=height;i++) {
        gotoxy(x, y+i); printf("%c", 179);
        gotoxy(x+width+1, y+i); printf("%c", 179);
    }

    gotoxy(x, y+height+1); printf("%c", 192);
    for(int i=0;i<width;i++) printf("%c", 196);
    printf("%c", 217);
}

void printHeader() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    // Dapatkan lebar konsol yang sebenarnya
    consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    // Lebar ASCII Art (tetap 87)
    int asciiWidth = 87;

    // Hitung posisi X untuk menengahkan ASCII Art
    headerX = (consoleWidth - asciiWidth) / 2;
    if(headerX < 0) headerX = 0;

    // Cetak ASCII Art
    gotoxy(headerX, 1);  printf("   _____ _    _ _____  ______ _____  __  __           _____  _  ________ _______ ");
    gotoxy(headerX, 2);  printf("  / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__    __|");
    gotoxy(headerX, 3);  printf(" | (___ | |  | | |__) | |__  | |__) | \\  / |   /  \\   | |__) | ' /| |__     | |   ");
    gotoxy(headerX, 4);  printf("  \\___ \\| |  | |  ___/|  __| |  _  /| |\\/| |  / /\\ \\  |  _  /|  < |  __|    | |   ");
    gotoxy(headerX, 5);  printf("  ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____   | |   ");
    gotoxy(headerX, 6);  printf(" |_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|  |_|   ");

    // Garis Pemisah Penuh
    // Kita pindahkan garis pemisah sedikit ke bawah untuk memberi jarak
    gotoxy(0, 8);
    for(int i=0; i < consoleWidth; i++) {
        printf("-");
    }
}

// ======================================================================
//                   FEATURE MOCKUP (Sudah Diperbaiki Pemusatan)
// ======================================================================

void featureUnderConstruction(char *featureName) {
    system("cls");
    printHeader();

    int boxW = BOX_WIDTH;
    int boxX = (consoleWidth - boxW) / 2;
    drawBox(boxX, 10, boxW, 4);

    gotoxy(boxX + 5, 12); printf("Fitur: %s", featureName);
    gotoxy(boxX + 5, 13); printf("Status: Sedang dalam pengembangan...");
    gotoxy(boxX + 5, 15); printf("Tekan sembarang tombol untuk kembali.");
    getch();
}

// ======================================================================
//                          MENU MEMBER (Sudah Diperbaiki Pemusatan)
// ======================================================================

void menuMember() {
    int choice;
    int boxW = BOX_WIDTH;
    int boxX = (consoleWidth - boxW) / 2;

    while(1) {
        system("cls");
        printHeader();

        int titleX = boxX + (boxW / 2) - strlen("=== HALAMAN MEMBER ===") / 2;
        gotoxy(titleX, 9); printf("=== HALAMAN MEMBER ===");

        drawBox(boxX, 11, boxW, 8);

        gotoxy(boxX + 5, 13); printf("[1] Daftar Belanjaan");
        gotoxy(boxX + 5, 14); printf("[2] Daftar Kategori");
        gotoxy(boxX + 5, 15); printf("[3] Keranjang");
        gotoxy(boxX + 5, 16); printf("[4] Riwayat Transaksi");
        gotoxy(boxX + 5, 18); printf("[0] Logout");

        gotoxy(boxX + 5, 20); printf("Pilihan Anda >> ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: featureUnderConstruction("Daftar Belanjaan"); break;
            case 2: featureUnderConstruction("Daftar Kategori"); break;
            case 3: featureUnderConstruction("Keranjang"); break;
            case 4: featureUnderConstruction("Riwayat Transaksi"); break;
            case 0: return;
            default: break;
        }
    }
}

// ======================================================================
//                          MENU KASIR (Sudah Diperbaiki Pemusatan)
// ======================================================================

void menuKasir() {
    int choice;
    int boxW = BOX_WIDTH;
    int boxX = (consoleWidth - boxW) / 2;

    while(1) {
        system("cls");
        printHeader();

        int titleX = boxX + (boxW / 2) - strlen("=== HALAMAN KASIR ===") / 2;
        gotoxy(titleX, 9); printf("=== HALAMAN KASIR ===");

        drawBox(boxX, 11, boxW, 7);

        gotoxy(boxX + 5, 13); printf("[1] Transaksi Penjualan");
        gotoxy(boxX + 5, 14); printf("[2] Retur Barang");
        gotoxy(boxX + 5, 15); printf("[3] Cetak Struk Ulang");
        gotoxy(boxX + 5, 17); printf("[0] Logout");

        gotoxy(boxX + 5, 19); printf("Pilihan Anda >> ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: featureUnderConstruction("Transaksi Penjualan"); break;
            case 2: featureUnderConstruction("Retur Barang"); break;
            case 3: featureUnderConstruction("Cetak Struk Ulang"); break;
            case 0: return;
            default: break;
        }
    }
}

// ======================================================================
//                          MENU ADMIN (Sudah Diperbaiki Pemusatan)
// ======================================================================

void menuAdmin() {
    int choice;
    int boxW = BOX_WIDTH;
    int boxX = (consoleWidth - boxW) / 2;

    while(1) {
        system("cls");
        printHeader();

        int titleX = boxX + (boxW / 2) - strlen("=== HALAMAN ADMIN ===") / 2;
        gotoxy(titleX, 9); printf("=== HALAMAN ADMIN ===");

        drawBox(boxX, 11, boxW, 12);

        gotoxy(boxX + 5, 12); printf("[1] Data Produk");
        gotoxy(boxX + 5, 13); printf("[2] Data Kategori");
        gotoxy(boxX + 5, 14); printf("[3] Stok Produk");
        gotoxy(boxX + 5, 15); printf("[4] Data Supplier");
        gotoxy(boxX + 5, 16); printf("[5] Metode Pembayaran");
        gotoxy(boxX + 5, 17); printf("[6] Diskon");
        gotoxy(boxX + 5, 18); printf("[7] Laporan Penjualan");
        gotoxy(boxX + 5, 19); printf("[8] Laporan Stock Opname");
        gotoxy(boxX + 5, 21); printf("[0] Logout");

        gotoxy(boxX + 5, 24); printf("Pilihan Anda >> ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: featureUnderConstruction("Data Produk"); break;
            case 2: featureUnderConstruction("Data Kategori"); break;
            case 3: featureUnderConstruction("Stok Produk"); break;
            case 4: featureUnderConstruction("Data Supplier"); break;
            case 5: featureUnderConstruction("Metode Pembayaran"); break;
            case 6: featureUnderConstruction("Diskon"); break;
            case 7: featureUnderConstruction("Laporan Penjualan"); break;
            case 8: featureUnderConstruction("Laporan Stock Opname"); break;
            case 0: return;
            default: break;
        }
    }
}

// ======================================================================
//                             LOGIN (Sudah Diperbaiki Pemusatan)
// ======================================================================

void loginScreen(int role) {
    char username[50], password[50], ch;
    char roleName[20];
    int i;
    int boxW = BOX_WIDTH;
    int boxX = (consoleWidth - boxW) / 2;

    if(role == 1) strcpy(roleName, "KASIR");
    else if(role == 2) strcpy(roleName, "MEMBER");
    else strcpy(roleName, "ADMIN");

    while(1) {
        system("cls");
        printHeader();

        char title[50];
        sprintf(title, "LOGIN SEBAGAI %s", roleName);

        int titleX = boxX + (boxW / 2) - strlen(title) / 2;
        gotoxy(titleX, 9); printf("%s", title);

        drawBox(boxX, 11, boxW, 8);

        gotoxy(boxX + 5, 13); printf("Username : ");
        gotoxy(boxX + 5, 16); printf("Password : ");

        gotoxy(boxX + 17, 13);
        if(scanf("%s", username) != 1) { /* handle error */ }

        gotoxy(boxX + 17, 16);
        i = 0;

        while(1) {
            ch = getch();
            if(ch == 13) {
                password[i] = '\0';
                break;
            } else if(ch == 8) {
                if(i > 0) {
                    i--;
                    printf("\b \b");
                }
            } else {
                password[i] = ch;
                i++;
                printf("*");
            }
        }

        gotoxy(boxX + 5, 21);
        printf(">> Login Berhasil! Masuk ke menu...");
        Sleep(1000);

        if(role == 1) menuKasir();
        else if(role == 2) menuMember();
        else if(role == 3) menuAdmin();

        return;
    }
}

// ======================================================================
//                               MAIN (Sudah Diperbaiki Penumpukan)
// ======================================================================

int main() {
    int choice;

    startupSequence();
    system("color F0");

    int boxW = BOX_WIDTH;

    while(1) {
        system("cls");
        printHeader(); // consoleWidth akan dihitung di sini

        // boxX harus dihitung ulang setelah printHeader
        int boxX = (consoleWidth - boxW) / 2;

        char title[] = "PILIH LOGIN SEBAGAI";
        int titleX = boxX + (boxW / 2) - strlen(title) / 2;
        gotoxy(titleX, 9); printf("%s", title);

        // Kotak Menu Utama
        drawBox(boxX, 11, boxW, 7);

        // Opsi di dalam kotak - Kunci: Pastikan setiap opsi berada di baris Y yang berbeda
        gotoxy(boxX + 8, 13); printf("[1] Kasir");
        gotoxy(boxX + 8, 14); printf("[2] Member");
        gotoxy(boxX + 8, 15); printf("[3] Admin");
        gotoxy(boxX + 8, 17); printf("[0] Keluar Program");

        // Input Pilihan - Pastikan di baris Y yang berbeda
        gotoxy(boxX + 8, 19); printf("Pilihan >> ");

        if(scanf("%d", &choice) != 1) {
            // Bersihkan buffer input jika terjadi error
            while(getchar() != '\n');
            continue;
        }

        if(choice == 1) loginScreen(1);
        else if(choice == 2) loginScreen(2);
        else if(choice == 3) loginScreen(3);
        else if(choice == 0) {
            gotoxy(boxX, 22);
            printf("Terima kasih telah menggunakan aplikasi ini.");
            break;
        }
        else {
            // Pilihan invalid
        }
    }

    return 0;
}