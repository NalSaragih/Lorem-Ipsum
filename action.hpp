#pragma once
#include <string>

struct nodeaction{
    std :: string namaaksi;
    std :: string username;
    nodeaction* next;
};

struct displayaction{
    std :: string namaaksi;
    std :: string username;
    bool isEmpty;
};


void initaction();
void pushaction(std :: string namaAct, std :: string usern);
void popaction();
displayaction peekaction();
bool isStackempty();