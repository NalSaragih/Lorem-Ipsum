#pragma once
#include <string>

struct nodechat{
    std :: string namapengirim;
    std :: string isipesan;
    nodechat* next;
};

struct displaychat {
    std :: string namapengirim;
    std :: string isipesan;
    bool isEnd;
};

void initchat();
void kirimpesan(std :: string pengirim, std :: string isi);
void hapusriwayat();
void mulaibaca();
displaychat bacaselanjutnya();