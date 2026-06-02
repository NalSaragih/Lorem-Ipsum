#include "follow.hpp"

namespace lorem {

static nodeuser*   Follow         = nullptr;
static nodefollow* currentfollow  = nullptr;

void initgraph() {
    Follow = nullptr;
    currentfollow = nullptr;
}

static nodeuser* findUser(const std::string& username) {
    nodeuser* temp = Follow;
    while (temp != nullptr) {
        if (temp->username == username) return temp;
        temp = temp->next;
    }
    return nullptr;
}

void tambahuser(std::string username) {
    if (findUser(username) != nullptr) return;
    nodeuser* pnew = new nodeuser;
    pnew->username = username;
    pnew->daftarfollow = nullptr;
    pnew->next = Follow;
    Follow = pnew;
}

bool sudahfollow(std::string userasal, std::string usertujuan) {
    nodeuser* asal = findUser(userasal);
    if (asal == nullptr) return false;
    nodefollow* temp = asal->daftarfollow;
    while (temp != nullptr) {
        if (temp->usertujuan == usertujuan) return true;
        temp = temp->next;
    }
    return false;
}

void follow(std::string userasal, std::string usertujuan) {
    if (userasal == usertujuan) return;
    nodeuser* asal   = findUser(userasal);
    nodeuser* tujuan = findUser(usertujuan);
    if (asal == nullptr || tujuan == nullptr) return;
    if (sudahfollow(userasal, usertujuan)) return;

    nodefollow* pnew = new nodefollow;
    pnew->usertujuan = usertujuan;
    pnew->next = asal->daftarfollow;
    asal->daftarfollow = pnew;
}

void unfollow(std::string userasal, std::string usertujuan) {
    nodeuser* asal = findUser(userasal);
    if (asal == nullptr) return;

    nodefollow* phapus = asal->daftarfollow;
    nodefollow* prev   = nullptr;
    while (phapus != nullptr && phapus->usertujuan != usertujuan) {
        prev = phapus;
        phapus = phapus->next;
    }
    if (phapus == nullptr) return;

    if (phapus == asal->daftarfollow) {
        asal->daftarfollow = phapus->next;
    } else {
        prev->next = phapus->next;
    }
    phapus->next = nullptr;
    delete phapus;
}

void mulailihatfoll(std::string username) {
    nodeuser* tujuan = findUser(username);
    currentfollow = (tujuan != nullptr) ? tujuan->daftarfollow : nullptr;
}

displaylist lihatfollow(std::string /*username*/) {
    displaylist data;
    if (currentfollow == nullptr) {
        data.isEnd = true;
        return data;
    }
    data.username = currentfollow->usertujuan;
    data.isEnd = false;
    currentfollow = currentfollow->next;
    return data;
}

int hitungfollower(std::string username) {
    int n = 0;
    nodeuser* user = Follow;
    while (user != nullptr) {
        nodefollow* edge = user->daftarfollow;
        while (edge != nullptr) {
            if (edge->usertujuan == username) { n++; break; }
            edge = edge->next;
        }
        user = user->next;
    }
    return n;
}

int hitungfollowing(std::string username) {
    nodeuser* asal = findUser(username);
    if (asal == nullptr) return 0;
    int n = 0;
    nodefollow* edge = asal->daftarfollow;
    while (edge != nullptr) { n++; edge = edge->next; }
    return n;
}

std::vector<std::string> getAllFollowing(const std::string& username) {
    std::vector<std::string> result;
    nodeuser* asal = findUser(username);
    if (asal == nullptr) return result;
    nodefollow* edge = asal->daftarfollow;
    while (edge != nullptr) {
        result.push_back(edge->usertujuan);
        edge = edge->next;
    }
    return result;
}

void resetgraph() {
    nodeuser* user = Follow;
    while (user != nullptr) {
        nodefollow* edge = user->daftarfollow;
        while (edge != nullptr) {
            nodefollow* del = edge;
            edge = edge->next;
            delete del;
        }
        nodeuser* delU = user;
        user = user->next;
        delete delU;
    }
    Follow = nullptr;
    currentfollow = nullptr;
}

}
