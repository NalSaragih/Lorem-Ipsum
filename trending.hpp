#pragma once
#include <string>

namespace trending {

struct nodeTree {
    std::string namaTopik;
    int         jumlahinteraksi;
    nodeTree*   kiri;
    nodeTree*   kanan;
};

struct displaytree {
    std::string namatopik;
    int         jumlahinteraksi;
    bool        isEmpty;
};

struct trendingIter {
    nodeTree* stackBuffer[256];
    int       sp;
    nodeTree* current;
};

void inittrending();
void resettrending();

void tambahtrending(std::string namatopik, int jumlahinteraksi);
void updatetrending(std::string namatopik, int delta);
void hapustrending(std::string namatopik);

displaytree caritoptrending();

void        mulailihattrending();
displaytree lihatselanjutnyatrending();

void cetakrekursif(nodeTree* current);
void tampilkansemuatrending();

} // namespace trending
