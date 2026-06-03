#pragma once
#include <string>

namespace notif {

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

void initqueue();
void notifmasuk(std :: string usern, std :: string isiN);
void hapusNotif();
displayNotif lihatNotif();
displayNotif bacanotif();
bool checkEmpty();

} // namespace notif
