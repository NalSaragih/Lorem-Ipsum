#pragma once
#include <string>

/* ini file hpp untuk queue notifikasi*/
struct nodeNotifikasi{
    std :: string username;
    std :: string isiNotif;
    nodeNotifikasi* next;
};

struct displayNotif{
    std :: string username;
    std :: string isiNotif;
    bool isEmpty;
};

void initqueue();  //inisialisasi queue
void notifmasuk(std :: string usern, std :: string isiN); // tambah notif
void hapusNotif(); // hapus notif
displayNotif lihatNotif(); // lihat notif paling atas
displayNotif bacanotif(); // lihat dan hapus
bool checkEmpty(); //check apakah kosong





