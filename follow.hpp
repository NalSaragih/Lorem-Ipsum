#pragma once
#include <string>
#include <vector>

namespace lorem {

struct nodefollow {
    std::string usertujuan;
    nodefollow* next;
};

struct nodeuser {
    std::string username;
    nodefollow* daftarfollow;
    nodeuser*   next;
};

struct displaylist {
    std::string username;
    bool        isEnd;
};

void initgraph();
void resetgraph();

void tambahuser(std::string username);
bool sudahfollow(std::string userasal, std::string usertujuan);
void follow(std::string userasal, std::string usertujuan);
void unfollow(std::string userasal, std::string usertujuan);

void        mulailihatfoll(std::string username);
displaylist lihatfollow(std::string username);

int  hitungfollower(std::string username);
int  hitungfollowing(std::string username);

std::vector<std::string> getAllFollowing(const std::string& username);

}
