#include "like.hpp"

namespace lorem {

static nodeuserlike* listUserLike = nullptr;

void initlike() {
    listUserLike = nullptr;
}

static nodeuserlike* getOrCreateUserStack(std::string username) {
    nodeuserlike* temp = listUserLike;
    while (temp != nullptr) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    nodeuserlike* pnew = new nodeuserlike;
    pnew->username = username;
    pnew->topStack = nullptr;
    pnew->next = listUserLike;
    listUserLike = pnew;
    return pnew;
}

static nodeuserlike* findUserStack(std::string username) {
    nodeuserlike* temp = listUserLike;
    while (temp != nullptr) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    return nullptr;
}

void pushlike(std::string username, int postId) {
    nodeuserlike* userBox = getOrCreateUserStack(username);
    nodelike* pnew = new nodelike;
    pnew->postId = postId;
    pnew->next = userBox->topStack;
    userBox->topStack = pnew;
}

displaylike poplike(std::string username) {
    displaylike data;
    nodeuserlike* userBox = findUserStack(username);
    if (userBox == nullptr || userBox->topStack == nullptr) {
        data.isEmpty = true;
        data.postId = -1;
        return data;
    }
    nodelike* phapus = userBox->topStack;
    data.postId = phapus->postId;
    data.isEmpty = false;
    userBox->topStack = phapus->next;
    delete phapus;
    return data;
}

displaylike peeklike(std::string username) {
    displaylike data;
    nodeuserlike* userBox = findUserStack(username);
    if (userBox == nullptr || userBox->topStack == nullptr) {
        data.isEmpty = true;
        data.postId = -1;
        return data;
    }
    data.postId = userBox->topStack->postId;
    data.isEmpty = false;
    return data;
}

bool isLikeEmpty(std::string username) {
    nodeuserlike* userBox = findUserStack(username);
    return userBox == nullptr || userBox->topStack == nullptr;
}

int sizeLike(std::string username) {
    nodeuserlike* userBox = findUserStack(username);
    if (userBox == nullptr) return 0;
    int n = 0;
    nodelike* temp = userBox->topStack;
    while (temp != nullptr) {
        n++;
        temp = temp->next;
    }
    return n;
}

void resetlike() {
    nodeuserlike* userBox = listUserLike;
    while (userBox != nullptr) {
        nodelike* curr = userBox->topStack;
        while (curr != nullptr) {
            nodelike* del = curr;
            curr = curr->next;
            delete del;
        }
        nodeuserlike* delUser = userBox;
        userBox = userBox->next;
        delete delUser;
    }
    listUserLike = nullptr;
}

}
