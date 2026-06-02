#include "trending.hpp"
#include <iostream>

namespace lorem {

static nodeTree*    root = nullptr;
static trendingIter iter;

void inittrending() {
    root = nullptr;
    iter.sp = 0;
    iter.current = nullptr;
}

static nodeTree* findNode(nodeTree* node, const std::string& topik) {
    if (node == nullptr) return nullptr;
    if (node->namaTopik == topik) return node;
    nodeTree* l = findNode(node->kiri, topik);
    if (l != nullptr) return l;
    return findNode(node->kanan, topik);
}

static nodeTree* removeNode(nodeTree* node, const std::string& topik, int score) {
    if (node == nullptr) return nullptr;
    if (node->namaTopik == topik && node->jumlahinteraksi == score) {
        if (node->kiri == nullptr) {
            nodeTree* right = node->kanan;
            delete node;
            return right;
        }
        if (node->kanan == nullptr) {
            nodeTree* left = node->kiri;
            delete node;
            return left;
        }
        nodeTree* succParent = node;
        nodeTree* succ = node->kanan;
        while (succ->kiri != nullptr) {
            succParent = succ;
            succ = succ->kiri;
        }
        node->namaTopik = succ->namaTopik;
        node->jumlahinteraksi = succ->jumlahinteraksi;
        if (succParent->kiri == succ) succParent->kiri = succ->kanan;
        else                          succParent->kanan = succ->kanan;
        delete succ;
        return node;
    }
    if (score < node->jumlahinteraksi) {
        node->kiri = removeNode(node->kiri, topik, score);
    } else {
        node->kanan = removeNode(node->kanan, topik, score);
    }
    return node;
}

static void destroyAll(nodeTree* node) {
    if (node == nullptr) return;
    destroyAll(node->kiri);
    destroyAll(node->kanan);
    delete node;
}

void resettrending() {
    destroyAll(root);
    root = nullptr;
}

void tambahtrending(std::string namatopik, int jumlahinteraksi) {
    nodeTree* pnew = new nodeTree;
    pnew->namaTopik = namatopik;
    pnew->jumlahinteraksi = jumlahinteraksi;
    pnew->kanan = nullptr;
    pnew->kiri = nullptr;

    if (root == nullptr) {
        root = pnew;
        return;
    }
    nodeTree* temp = root;
    nodeTree* parent = nullptr;
    while (temp != nullptr) {
        parent = temp;
        if (jumlahinteraksi < temp->jumlahinteraksi) temp = temp->kiri;
        else                                          temp = temp->kanan;
    }
    if (pnew->jumlahinteraksi < parent->jumlahinteraksi) parent->kiri = pnew;
    else                                                  parent->kanan = pnew;
}

void updatetrending(std::string namatopik, int delta) {
    nodeTree* found = findNode(root, namatopik);
    int newScore = (found != nullptr ? found->jumlahinteraksi + delta : delta);
    if (found != nullptr) {
        root = removeNode(root, namatopik, found->jumlahinteraksi);
    }
    if (newScore > 0) {
        tambahtrending(namatopik, newScore);
    }
}

void hapustrending(std::string namatopik) {
    nodeTree* found = findNode(root, namatopik);
    if (found != nullptr) {
        root = removeNode(root, namatopik, found->jumlahinteraksi);
    }
}

displaytree caritoptrending() {
    displaytree data;
    if (root == nullptr) {
        data.isEmpty = true;
        data.jumlahinteraksi = 0;
        return data;
    }
    nodeTree* temp = root;
    while (temp->kanan != nullptr) temp = temp->kanan;
    data.namatopik = temp->namaTopik;
    data.jumlahinteraksi = temp->jumlahinteraksi;
    data.isEmpty = false;
    return data;
}

void mulailihattrending() {
    iter.sp = 0;
    iter.current = root;
}

displaytree lihatselanjutnyatrending() {
    displaytree data;
    while (iter.current != nullptr || iter.sp > 0) {
        while (iter.current != nullptr) {
            if (iter.sp < 256) iter.stackBuffer[iter.sp++] = iter.current;
            iter.current = iter.current->kanan;
        }
        nodeTree* node = iter.stackBuffer[--iter.sp];
        iter.current = node->kiri;
        data.namatopik = node->namaTopik;
        data.jumlahinteraksi = node->jumlahinteraksi;
        data.isEmpty = false;
        return data;
    }
    data.isEmpty = true;
    data.jumlahinteraksi = 0;
    return data;
}

void cetakrekursif(nodeTree* current) {
    if (current == nullptr) return;
    cetakrekursif(current->kanan);
    std::cout << "#" << current->namaTopik << " (" << current->jumlahinteraksi << " interaksi)" << std::endl;
    cetakrekursif(current->kiri);
}

void tampilkansemuatrending() {
    if (root == nullptr) {
        std::cout << "Belum ada topik trending saat ini" << std::endl;
        return;
    }
    cetakrekursif(root);
}

}
