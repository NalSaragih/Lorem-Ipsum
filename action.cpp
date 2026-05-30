#include "action.hpp"

nodeaction* top = nullptr;


void initaction(){
    top = nullptr;
}

bool isStackempty(){
    return top == nullptr;
}

void pushaction(std :: string namaAct, std :: string usern){
    nodeaction* pnew = new nodeaction;

    pnew->namaaksi = namaAct;
    pnew -> username = usern;

    pnew -> next = top;
    top = pnew;

}

void popaction(){
    if(!isStackempty()){
        nodeaction* phapus = top;
        top = top -> next;
        delete phapus;
    }
}

displayaction peekaction(){

    displayaction data;
    if(isStackempty()){
        data.isEmpty = true;
    }else{
        data.namaaksi = top->namaaksi;
        data.username = top->username;
        data.isEmpty = false;
    }

    return data;
}