#include "chat.hpp"

nodechat* head = nullptr;
nodechat* tail = nullptr;
nodechat* currentchat = nullptr; 

void initchat(){
    head = nullptr;
    tail = nullptr;
    currentchat = nullptr;
}

void kirimpesan(std :: string nama, std :: string isi){
    nodechat* pnew = new nodechat;
    pnew->namapengirim = nama;
    pnew -> isipesan = isi;
    pnew -> next = nullptr;

    if(tail == nullptr){
        head = pnew;
        tail = pnew;
    }else{
         tail -> next = pnew;
        tail = pnew;
    }
}

void hapusriwayat(){
    nodechat* phapus = head;

    while(phapus != nullptr){
        nodechat* temp = phapus;
        phapus = phapus -> next;
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
    currentchat = nullptr;
}

void mulaibaca(){
    currentchat = head;
}

displaychat bacaselanjutnya(){
    displaychat kirimdata;

    if(currentchat == nullptr){
        kirimdata.isEnd = true;
        return kirimdata;
    }

    kirimdata.namapengirim = currentchat->namapengirim;
    kirimdata.isipesan = currentchat->isipesan;
    kirimdata.isEnd = false;

    currentchat = currentchat->next;

    return kirimdata;
}