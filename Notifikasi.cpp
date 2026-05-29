#include "Notifikasi.hpp"

nodeNotifikasi* head = nullptr;
nodeNotifikasi* tail = nullptr;

void initqueue(){
    head = nullptr;
    tail = nullptr;
}

bool checkEmpty(){
    return head == nullptr;
}

void notifmasuk(std :: string usern, std :: string isin){
    nodeNotifikasi* pnew = new nodeNotifikasi;
    pnew->username = usern;
    pnew->isiNotif = isin;
    pnew->next = nullptr;

    if(checkEmpty()){
        head = tail = pnew;
    }else{
        tail->next = pnew;
        tail = pnew;
    }
}

void hapusNotif(){

    if(!checkEmpty()){
        nodeNotifikasi* phapus = head;
        head = phapus ->next;

        if(head == nullptr) tail = nullptr;
        delete phapus;
    }
}

displayNotif lihatNotif(){
    displayNotif data;
    if(checkEmpty()){
        data.isEmpty = true;
    }else{
        data.username = head -> username;
        data.isiNotif = head->isiNotif;
        data.isEmpty = false;
    }
    return data;
}

displayNotif bacanotif(){
    displayNotif dibaca = lihatNotif();
    hapusNotif();
    return dibaca;
}


