#pragma once
#include <string>

namespace likestack {

struct nodelike {
    int postId;
    nodelike* next;
};

struct nodeuserlike {
    std::string username;
    nodelike*   topStack;
    nodeuserlike* next;
};

struct displaylike {
    int  postId;
    bool isEmpty;
};

void initlike();
void resetlike();

void pushlike(std::string username, int postId);
displaylike poplike(std::string username);
displaylike peeklike(std::string username);
bool isLikeEmpty(std::string username);
int  sizeLike(std::string username);

} // namespace likestack
