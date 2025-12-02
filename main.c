#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

// ====================== UTIL ===========================
void setFullConsole() {
    system("mode con: cols=160 lines=45");
}

void clearScreen() {
    system("cls");
}

void gotoXY(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void greenBG() {
    printf("\033[42m\033[37m");
}

void resetColor() {
    printf("\033[0m");
}

void fillGreenBG() {
    greenBG();
    for (int y = 1; y <= 45; y++) {
        gotoXY(1, y);
        for (int x = 1; x <= 160; x++) printf(" ");
    }
}

// Garis Horizontal Double-Line
void drawH2(int x, int y, int length) {
    gotoXY(x, y);
    for (int i = 0; i < length; i++) printf("═");
}

// Garis Vertikal Double-Line
void drawV2(int x, int y, int height) {
    for (int i = 0; i < height; i++) {
        gotoXY(x, y + i);
        printf("║");
    }
}

void drawBox2(int x, int y, int w, int h) {
    gotoXY(x, y);         printf("╔");
    for (int i=0; i<w-2; i++) printf("═");
    printf("╗");

    for (int i=1; i<h-1; i++) {
        gotoXY(x, y+i);       printf("║");
        gotoXY(x + w - 1, y+i); printf("║");
    }

    gotoXY(x, y+h-1);     printf("╚");
    for (int i=0; i<w-2; i++) printf("═");
    printf("╝");
}

// ====================== ASCII HEADER ===========================
void headerASCII() {
    gotoXY(42, 6);
    printf("███████╗██╗   ██╗██████╗ ███████╗██████╗ ███╗   ███╗ █████╗ ███╗  ██╗");
    gotoXY(42, 7);
    printf("██╔════╝██║   ██║██╔══██╗██╔════╝██╔══██╗████╗ ████║██╔══██╗████╗ ██║");
    gotoXY(42, 8);
    printf("█████╗  ██║   ██║██████╔╝█████╗  ██████╔╝██╔████╔██║███████║██╔██╗██║");
    gotoXY(42, 9);
    printf("██╔══╝  ██║   ██║██╔══██╗██╔══╝  ██╔══██╗██║╚██╔╝██║██╔══██║██║╚████║");
    gotoXY(42, 10);
    printf("██║     ╚██████╔╝██████╔╝███████╗██║  ██║██║ ╚═╝ ██║██║  ██║██║ ╚███║");
}

// ====================== LOADING ===========================
void loadingAnimation() {
    const char *phase[3] = { "Loading.", "Loading..", "Loading..." };

    for (int i = 0; i < 3; i++) {
        gotoXY(70, 30);
        printf("%s", phase[i]);
        Sleep(600);
    }
}

// ====================== LOGIN SCREEN ===========================
void loginScreen(char user[], char pass[]) {
    fillGreenBG();

    headerASCII();

    // Garis layout tebal
    drawH2(1, 13, 160);
    drawH2(1, 4, 160);
    drawV2(20, 1, 45);
    drawV2(120, 1, 45);

    gotoXY(75, 16);
    printf("LOGIN");

    drawBox2(40, 18, 80, 12);

    gotoXY(50, 22); printf("Username : ");
    gotoXY(50, 25); printf("Password : ");

    // Input
    gotoXY(62, 22);
    scanf("%s", user);

    gotoXY(62, 25);
    scanf("%s", pass);

    // Loading
    loadingAnimation();
}

// ====================== ROLE ===========================
int screenRole() {
    int r;
    fillGreenBG();
    headerASCII();

    gotoXY(70, 15);
    printf("Login Sebagai :");

    drawBox2(45, 18, 70, 10);
    gotoXY(55, 22); printf("[1] Kasir");
    gotoXY(55, 24); printf("[2] Member");
    gotoXY(55, 26); printf("[3] Admin");

    gotoXY(55, 29); printf("Pilih : ");
    scanf("%d", &r);

    loadingAnimation();
    return r;
}

// ====================== MENU KASIR ===========================
void menuKasir() {
    fillGreenBG();
    headerASCII();

    gotoXY(70, 16); printf("MENU KASIR");
    drawBox2(40, 18, 80, 12);

    gotoXY(50, 22); printf("[1] Transaksi Penjualan");
    gotoXY(50, 24); printf("[2] Retur Barang");
    gotoXY(50, 26); printf("[3] Cetak Struk Ulang");

    gotoXY(50, 29); printf("[0] Logout");
    getch();
}

// ====================== MENU MEMBER ===========================
void menuMember() {
    fillGreenBG();
    headerASCII();

    gotoXY(70, 16); printf("MENU MEMBER");
    drawBox2(40, 18, 80, 12);

    gotoXY(50, 22); printf("[1] Daftar Belanja");
    gotoXY(50, 24); printf("[2] Daftar Kategori");
    gotoXY(50, 26); printf("[3] Keranjang");
    gotoXY(50, 28); printf("[4] Riwayat Transaksi");

    gotoXY(50, 30); printf("[0] Logout");
    getch();
}

// ====================== MENU ADMIN ===========================
void menuAdmin() {
    fillGreenBG();
    headerASCII();

    gotoXY(70, 16); printf("MENU ADMIN");
    drawBox2(40, 18, 80, 16);

    gotoXY(50, 21); printf("[1] Data Produk");
    gotoXY(50, 22); printf("[2] Data Kategori");
    gotoXY(50, 23); printf("[3] Stok Produk");
    gotoXY(50, 24); printf("[4] Supplier");
    gotoXY(50, 25); printf("[5] Metode Pembayaran");
    gotoXY(50, 26); printf("[6] Diskon");
    gotoXY(50, 27); printf("[7] Laporan Penjualan");
    gotoXY(50, 28); printf("[8] Stok Opname");

    gotoXY(50, 31); printf("[0] Logout");
    getch();
}

// ====================== MAIN ===========================
int main() {
    char user[50], pass[50];
    int role;

    setFullConsole();
    loginScreen(user, pass);
    role = screenRole();

    if (role == 1) menuKasir();
    else if (role == 2) menuMember();
    else if (role == 3) menuAdmin();
    else {
        gotoXY(60, 30);
        printf("Role tidak valid.");
        getch();
    }

    resetColor();
    return 0;
}
