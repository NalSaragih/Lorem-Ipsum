#ifndef SOCIALMEDIA_H
#define SOCIALMEDIA_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QSet>
#include <QVector>
#include <QPair>

struct User {
    QString   username;
    QString   password;
    QString   bio;
    QString   avatarEmoji;
    QDateTime joinedAt;
};

struct Comment {
    int       id        = 0;
    QString   author;
    QString   text;
    QDateTime timestamp;
};

struct CommentNode {
    Comment      data;
    CommentNode* prev;
    CommentNode* next;
};

struct Post {
    int           id      = 0;
    QString       author;
    QString       content;
    QDateTime     timestamp;
    QSet<QString> likedBy;
    CommentNode*  commentsHead = nullptr;
    CommentNode*  commentsTail = nullptr;
};

struct PostNode {
    Post*     data;
    PostNode* prev;
    PostNode* next;
};

struct Message {
    QString   sender;
    QString   receiver;
    QString   text;
    QDateTime timestamp;
};

struct MsgNode {
    Message  data;
    MsgNode* next;
};

struct MsgQueue {
    MsgNode* head;
    MsgNode* tail;
    MsgQueue() : head(nullptr), tail(nullptr) {}
};

struct Notification {
    int       id   = 0;
    QString   text;
    QString   actor;
    QString   type;
    QDateTime timestamp;
    bool      read = false;
};

struct NotifNode {
    Notification data;
    NotifNode*   next;
};

struct NotifQueue {
    NotifNode* head;
    NotifNode* tail;
    NotifQueue() : head(nullptr), tail(nullptr) {}
};

struct UserAction {
    enum Type {
        ActLike, ActUnlike, ActFollow, ActUnfollow,
        ActPost, ActDeletePost, ActComment, ActDeleteComment
    };
    Type      type;
    int       postId = -1;
    QString   targetUser;
    QDateTime timestamp;
};

struct ActionNode {
    UserAction  data;
    ActionNode* next;
};

struct LikeStackEntry {
    int     postId    = -1;
    QString postAuthor;
    QString postPreview;
    bool    stillLiked = false;
};

class SocialMedia {
public:
    SocialMedia();
    ~SocialMedia();

    bool registerUser(const QString& username, const QString& password);
    bool validateLogin(const QString& username, const QString& password) const;
    bool userExists(const QString& username) const;
    User getUser(const QString& username) const;
    void updateProfile(const QString& username, const QString& bio, const QString& avatar);

    QVector<QString> allUsers() const;
    QVector<QString> searchUsers(const QString& query) const;

    int              addPost(const QString& author, const QString& content);
    QVector<Post*>   getAllPostsNewestFirst() const;
    QVector<Post*>   getFeedFor(const QString& user) const;
    QVector<Post*>   getDiscoverPosts(const QString& user, int limit = 6) const;
    QVector<Post*>   getPostsBy(const QString& user) const;
    Post*            findPost(int postId) const;
    int              totalPosts() const;
    bool             deletePost(int postId, const QString& actor);

    int  toggleLike(const QString& user, int postId);
    int  undoLastLike(const QString& user);
    int  likeStackSize(const QString& user) const;
    QVector<LikeStackEntry> getLikeStack(const QString& user, int limit = -1) const;

    int              addComment(const QString& author, int postId, const QString& text);
    QVector<Comment> getComments(int postId) const;
    bool             deleteComment(int postId, int commentId, const QString& actor);

    bool             followUser(const QString& user, const QString& target);
    bool             unfollowUser(const QString& user, const QString& target);
    bool             isFollowing(const QString& user, const QString& target) const;
    QVector<QString> getFollowing(const QString& user) const;
    QVector<QString> getFollowers(const QString& user) const;
    QVector<QString> getFollowSuggestions(const QString& user, int maxResults = 5) const;

    void             sendMessage(const QString& from, const QString& to, const QString& text);
    QVector<Message> getConversation(const QString& a, const QString& b) const;
    QVector<QString> getChatPartners(const QString& user) const;

    void                   pushNotification(const QString& user, const QString& text,
                                            const QString& actor, const QString& type);
    QVector<Notification>  getNotifications(const QString& user, bool newestFirst = true) const;
    void                   markAllRead(const QString& user);
    int                    unreadCount(const QString& user) const;

    QVector<QPair<QString,int>> getTrendingTopics(int limit = 10) const;

    void seedDemoData();

    void                pushUserAction(const QString& user, UserAction::Type type,
                                       int postId = -1, const QString& target = {});
    int                 actionHistorySize(const QString& user) const;
    QVector<UserAction> getActionHistory(const QString& user, int limit = -1) const;
    void                clearActionHistory(const QString& user);

private:
    QHash<QString, User> users_;

    PostNode* postsHead_;
    PostNode* postsTail_;
    int       postsSize_;
    int       nextPostId_;
    int       nextCommentId_;

    QHash<QString, MsgQueue>   chats_;
    QHash<QString, NotifQueue> notifs_;
    QHash<QString, ActionNode*> actionHistory_;
    int nextNotifId_ = 1;

    static QString     chatKey(const QString& a, const QString& b);
    static QStringList extractHashtags(const QString& content);

    NotifQueue& notifFor(const QString& user);

    static void        pushBackPost(PostNode*& head, PostNode*& tail, int& size, Post* p);
    static void        removePostNode(PostNode*& head, PostNode*& tail, int& size, PostNode* node);

    static void        pushBackComment(CommentNode*& head, CommentNode*& tail, const Comment& c);
    static void        removeCommentNode(CommentNode*& head, CommentNode*& tail, CommentNode* node);
    static void        clearCommentList(CommentNode*& head, CommentNode*& tail);

    static void        enqueueMsg(MsgQueue& q, const Message& m);
    static void        clearMsgQueue(MsgQueue& q);

    static void        enqueueNotif(NotifQueue& q, const Notification& n);
    static bool        dequeueNotif(NotifQueue& q, Notification& out);
    static void        clearNotifQueue(NotifQueue& q);

    static void        pushAction(ActionNode*& top, const UserAction& a);
    static void        clearActionStack(ActionNode*& top);

    void destroyEverything();
};

#endif
