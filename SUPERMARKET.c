#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

int headerX = 0;
int consoleWidth = 150;
#define BOX_WIDTH 60

// =====================================================================================
// goFullscreen()
// Memicu kombinasi tombol ALT + ENTER lewat keybd_event untuk memaksa console masuk
// ke fullscreen mode.
// =====================================================================================
void goFullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1C, 0, 0);
    keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(150);
}

// =====================================================================================
// maximizeConsole()
// Memaksimalkan jendela console ke ukuran layar penuh versi windowed (bukan fullscreen).
// =====================================================================================
void maximizeConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(150);
}

// =====================================================================================
// setConsoleTitleBar()
// Mengubah warna kolom 0 dan 15 di tabel warna console (hitam & putih).
// =====================================================================================
void setConsoleTitleBar() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[0] = RGB(0, 0, 0);
    csbi.ColorTable[15] = RGB(255, 255, 255);
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

// =====================================================================================
// loadingAnimation()
// Animasi loading sederhana menggunakan array string yang bergerak dari kiri ke kanan.
// =====================================================================================
void loadingAnimation() {
    const char *frames[] = {
        "[=         ]", "[==        ]", "[===       ]",
        "[====      ]", "[=====     ]", "[======    ]",
        "[=======   ]", "[========  ]", "[========= ]",
        "[==========]"
    };
    int total = sizeof(frames) / sizeof(frames[0]);

    system("cls");
    printf("\n Loading...\n\n");

    for(int i = 0; i < total; i++) {
        printf(" %s", frames[i]);
        fflush(stdout);
        Sleep(80);
        printf("\r");
    }
    Sleep(150);
}

// =====================================================================================
// pulseFlash()
// Efek kilatan layar (blink putih-hitam) ngide aja sih hehe.
// =====================================================================================
void pulseFlash() {
    for (int i = 0; i < 3; i++) {
        system("color F0");
        Sleep(80);
        system("color 0F");
        Sleep(80);
    }
    system("color F0");
}

// =====================================================================================
// startupSequence()
// Urutan startup lengkap: maximize → set warna → loading → flash → fullscreen.
// =====================================================================================
void startupSequence() {
    maximizeConsole();
    setConsoleTitleBar();
    loadingAnimation();
    pulseFlash();
    goFullscreen();
    Sleep(200);
    system("cls");
}

// =====================================================================================
// gotoxy(x,y)
// Memindahkan posisi cursor console ke koordinat tertentu.
// =====================================================================================
void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// =====================================================================================
// drawBox(x,y,width,height)
// Menggambar kotak menggunakan karakter ASCII ┌─┐ │ │ └─┘
// Termasuk area kosong di dalam kotak.
// =====================================================================================
void drawBox(int x, int y, int width, int height) {
    for(int i = 1; i <= height; i++) {
        gotoxy(x + 1, y + i);
        for(int j = 0; j < width; j++) printf(" ");
    }

    gotoxy(x, y); printf("%c", 218);
    for(int i = 0; i < width; i++) printf("%c", 196);
    printf("%c", 191);

    for(int i = 1; i <= height; i++) {
        gotoxy(x, y+i); printf("%c", 179);
        gotoxy(x+width+1, y+i); printf("%c", 179);
    }

    gotoxy(x, y+height+1); printf("%c", 192);
    for(int i = 0; i < width; i++) printf("%c", 196);
    printf("%c", 217);
}

// =====================================================================================
// featureUnderConstruction()
// Template untuk fitur yang belum dibuat. Menampilkan pesan dan jeda.
// =====================================================================================
void featureUnderConstruction(const char *featureName) {
    system("cls");
    printf("\n Fitur '%s' sedang dikembangkan...\n", featureName);
    Sleep(1300);
}

// =====================================================================================
// printHeader()
// Mengambil lebar console, menghitung posisi tengah, dan menampilkan ASCII ART.
// =====================================================================================
void printHeader() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    int asciiWidth = 87;
    headerX = (consoleWidth - asciiWidth) / 2;
    if(headerX < 0) headerX = 0;

    gotoxy(headerX, 1);
    printf("   _____ _    _ _____  ______ _____  __  __           _____  _  ________ _______ ");
    gotoxy(headerX, 2);
    printf("  / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__    __|");
    gotoxy(headerX, 3);
    printf(" | (___ | |  | | |__) | |__  | |__) | \\  / |   /  \\   | |__) | ' /| |__     | |   ");
    gotoxy(headerX, 4);
    printf("  \\___ \\| |  | |  ___/|  __| |  _  /| |\\/| |  / /\\ \\  |  _  /|  < |  __|    | |   ");
    gotoxy(headerX, 5);
    printf("  ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____   | |   ");
    gotoxy(headerX, 6);
    printf(" |_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|  |_|   ");

    gotoxy(0, 8);
    for (int i = 0; i < consoleWidth; i++) printf("-");
}

// =====================================================================================
// menuMember()
// Menu khusus role MEMBER berisi daftar menu placeholder.
// =====================================================================================
void menuMember() {
    int choice;
    int boxX = (consoleWidth - BOX_WIDTH) / 2;

    while(1) {
        system("cls");
        printHeader();

        gotoxy(boxX + 20, 9); printf("=== HALAMAN MEMBER ===");
        drawBox(boxX, 11, BOX_WIDTH, 8);

        gotoxy(boxX+5,13); printf("[1] Daftar Belanjaan");
        gotoxy(boxX+5,14); printf("[2] Daftar Kategori");
        gotoxy(boxX+5,15); printf("[3] Keranjang");
        gotoxy(boxX+5,16); printf("[4] Riwayat Transaksi");
        gotoxy(boxX+5,18); printf("[0] Logout");

        gotoxy(boxX+5,20); printf("Pilihan Anda >> ");
        scanf("%d",&choice);

        switch(choice){
            case 1: featureUnderConstruction("Daftar Belanjaan"); break;
            case 2: featureUnderConstruction("Daftar Kategori"); break;
            case 3: featureUnderConstruction("Keranjang"); break;
            case 4: featureUnderConstruction("Riwayat Transaksi"); break;
            case 0: return;
        }
    }
}

// =====================================================================================
// menuKasir()
// Menu role KASIR berisi fitur-fitur transaksi penjualan.
// =====================================================================================
void menuKasir() {
    int choice;
    int boxX = (consoleWidth - BOX_WIDTH) / 2;

    while(1){
        system("cls");
        printHeader();

        gotoxy(boxX+20,9); printf("=== HALAMAN KASIR ===");
        drawBox(boxX,11,BOX_WIDTH,7);

        gotoxy(boxX+5,13); printf("[1] Transaksi Penjualan");
        gotoxy(boxX+5,14); printf("[2] Retur Barang");
        gotoxy(boxX+5,15); printf("[3] Cetak Struk Ulang");
        gotoxy(boxX+5,17); printf("[0] Logout");

        gotoxy(boxX+5,19); printf("Pilihan Anda >> ");
        scanf("%d",&choice);

        switch(choice){
            case 1: featureUnderConstruction("Transaksi Penjualan"); break;
            case 2: featureUnderConstruction("Retur Barang"); break;
            case 3: featureUnderConstruction("Cetak Struk Ulang"); break;
            case 0: return;
        }
    }
}

// =====================================================================================
// menuAdmin()
// Menu khusus ADMIN dengan lebih banyak opsi manajemen data.
// =====================================================================================
void menuAdmin(){
    int choice;
    int boxX = (consoleWidth - BOX_WIDTH) / 2;

    while(1){
        system("cls");
        printHeader();

        gotoxy(boxX+20,9); printf("=== HALAMAN ADMIN ===");
        drawBox(boxX,11,BOX_WIDTH,12);

        gotoxy(boxX+5,12); printf("[1] Data Produk");
        gotoxy(boxX+5,13); printf("[2] Data Kategori");
        gotoxy(boxX+5,14); printf("[3] Stok Produk");
        gotoxy(boxX+5,15); printf("[4] Data Supplier");
        gotoxy(boxX+5,16); printf("[5] Metode Pembayaran");
        gotoxy(boxX+5,17); printf("[6] Diskon");
        gotoxy(boxX+5,18); printf("[7] Laporan Penjualan");
        gotoxy(boxX+5,19); printf("[8] Laporan Stock Opname");
        gotoxy(boxX+5,21); printf("[0] Logout");

        gotoxy(boxX+5,24); printf("Pilihan Anda >> ");
        scanf("%d",&choice);

        switch(choice){
            case 1: featureUnderConstruction("Data Produk"); break;
            case 2: featureUnderConstruction("Data Kategori"); break;
            case 3: featureUnderConstruction("Stok Produk"); break;
            case 4: featureUnderConstruction("Data Supplier"); break;
            case 5: featureUnderConstruction("Metode Pembayaran"); break;
            case 6: featureUnderConstruction("Diskon"); break;
            case 7: featureUnderConstruction("Laporan Penjualan"); break;
            case 8: featureUnderConstruction("Laporan Stock Opname"); break;
            case 0: return;
        }
    }
}

// =====================================================================================
// menuSuperAdmin()
// =====================================================================================
void menuSuperAdmin() {
    int choice;
    int boxX = (consoleWidth - BOX_WIDTH) / 2;

    while(1){
        system("cls");
        printHeader();

        gotoxy(boxX+15,9); printf("=== MENU SUPER ADMIN ===");
        drawBox(boxX,11,BOX_WIDTH,10);

        gotoxy(boxX+5,13); printf("[1] Manajemen Akun Admin");
        gotoxy(boxX+5,14); printf("[2] Reset Sistem");
        gotoxy(boxX+5,15); printf("[3] Monitoring Aktivitas");
        gotoxy(boxX+5,16); printf("[4] Pengaturan Sistem Global");
        gotoxy(boxX+5,18); printf("[0] Logout");

        gotoxy(boxX+5,21); printf("Pilihan Anda >> ");
        scanf("%d",&choice);

        switch(choice){
            case 1: featureUnderConstruction("Manajemen Akun Admin"); break;
            case 2: featureUnderConstruction("Reset Sistem"); break;
            case 3: featureUnderConstruction("Monitoring Aktivitas"); break;
            case 4: featureUnderConstruction("Pengaturan Sistem Global"); break;
            case 0: return;
        }
    }
}

// =====================================================================================
// loginScreenSuperAdmin()
// Login dengan fitur show/hide password menggunakan tombol F2.
// =====================================================================================
void loginScreenSuperAdmin() {
    char username[50], password[50];
    char ch;
    int i, show = 0;

    int boxX = (consoleWidth - BOX_WIDTH) / 2;

    while(1){
        system("cls");
        printHeader();

        gotoxy(boxX+18,9); printf("LOGIN SUPER ADMIN");

        drawBox(boxX,11,BOX_WIDTH,10);

        gotoxy(boxX+5,13); printf("Username : ");
        gotoxy(boxX+5,15); printf("Password : ");
        gotoxy(boxX+5,17); printf("Tekan F2 untuk Show/Hide");

        gotoxy(boxX+17,13);
        scanf("%s",username);

        gotoxy(boxX+17,15);
        i = 0;

        while(1){
            ch = getch();

            if(ch == 13){
                password[i] = '\0';
                break;
            }

            if(ch == 8){
                if(i > 0){
                    i--;
                    printf("\b \b");
                }
                continue;
            }

            if(ch == 0 || ch == 224){
                ch = getch();
                if(ch == 60){
                    show = !show;
                    gotoxy(boxX+17,15);
                    for(int j=0;j<i;j++) printf("%c", show ? password[j] : '*');
                }
                continue;
            }

            password[i] = ch;
            i++;
            printf(show ? "%c" : "*", ch);
        }

        if(strcmp(username,"superadmin")==0 && strcmp(password,"superadmin")==0){
            gotoxy(boxX+5,20); printf(">> Login Berhasil! Masuk ke menu Super Admin...");
            Sleep(1000);
            menuSuperAdmin();
            return;
        }

        gotoxy(boxX+5,20); printf("Login gagal! Coba lagi...");
        Sleep(1000);
    }
}

// =====================================================================================
// mainMenu()
// Menu utama yang menghubungkan semua role yang tersedia.
// =====================================================================================
void mainMenu(){
    int choice;
    int boxX = (consoleWidth - BOX_WIDTH) / 2;

    while(1){
        system("cls");
        printHeader();

        drawBox(boxX,11,BOX_WIDTH,10);

        gotoxy(boxX+8,13); printf("[1] Kasir");
        gotoxy(boxX+8,14); printf("[2] Member");
        gotoxy(boxX+8,15); printf("[3] Admin");
        gotoxy(boxX+8,16); printf("[4] Super Admin");
        gotoxy(boxX+8,18); printf("[0] Keluar Program");

        gotoxy(boxX+8,21); printf("Pilihan Anda >> ");
        scanf("%d",&choice);

        switch(choice){
            case 1: menuKasir(); break;
            case 2: menuMember(); break;
            case 3: menuAdmin(); break;
            case 4: loginScreenSuperAdmin(); break;
            case 0: return;
            default: featureUnderConstruction("Role lain"); break;
        }
    }
}

// =====================================================================================
// main()
// Entry point: set ukuran console, jalankan startup, lalu tampilkan menu utama.
// =====================================================================================
int main() {
    system("mode con: cols=150 lines=40");
    startupSequence();
    mainMenu();
    return 0;
}
