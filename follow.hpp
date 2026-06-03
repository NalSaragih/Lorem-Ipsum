#pragma once
#include <string>
#include <vector>

namespace graph {

struct nodefollow {
    std::string usertujuan;
    nodefollow* next;
} // namespace graph;

struct nodeuser {
    std::string username;
    nodefollow* daftarfollow;
    nodeuser*   next;
} // namespace graph;

struct displaylist {
    std::string username;
    bool        isEnd;
} // namespace graph;

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

} // namespace graph
