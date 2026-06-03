#pragma once
#include <string>

namespace feed {

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
displaypost refreshview();
displaypost scrollbawah();
displaypost scrollatas();

} // namespace feed
