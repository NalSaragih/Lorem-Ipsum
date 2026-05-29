#include "feed.hpp"

nodepost* head = nullptr;
nodepost* tail = nullptr;
nodepost* currentfeed = nullptr;


void initfeed(){
    head = nullptr;
    tail = nullptr;
    currentfeed = nullptr;
}

displaypost getDisplaydata(){
    displaypost data;
    if(currentfeed == nullptr){
        data.isvalid = false;
    }else{
        data.username = currentfeed->username;
        data.caption = currentfeed->caption;
        data.isvalid = true;
    }

    return data;
}

void tambahpostingan(std :: string usern, std :: string capt){
    nodepost* pnew = new nodepost;
    pnew->username = usern;
    pnew->caption = capt;
    pnew -> next = nullptr;
    pnew -> prev = nullptr;

    if(head == nullptr){
        head = tail = pnew;
    }else {
        pnew -> next = head;
        head -> prev = pnew;
        head = pnew;
    }
}

displaypost refreshview(){
    currentfeed = head;
    return getDisplaydata();
}

displaypost scrollbawah(){
    if(currentfeed != nullptr && currentfeed->next != nullptr){
        currentfeed = currentfeed->next;
    }
    return getDisplaydata();
}

displaypost scrollatas(){
    if(currentfeed != nullptr && currentfeed -> prev != nullptr){
        currentfeed = currentfeed->prev;
    }
    return getDisplaydata();
}