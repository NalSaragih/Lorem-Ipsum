#pragma once
#include <string>

struct nodepost{
    std :: string username;
    std :: string caption;
    nodepost* next;
    nodepost* prev;
};

struct displaypost{
    std :: string username;
    std :: string caption;
    bool isvalid;
};

void initfeed();
void tambahpostingan(std :: string usern, std :: string capt);
displaypost refresview();
displaypost scrollbawah();
displaypost scrollatas();