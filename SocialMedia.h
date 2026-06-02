#ifndef SOCIALMEDIA_H
#define SOCIALMEDIA_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QSet>
#include <QVector>
#include <QPair>

#include "DataStructures.h"

struct User
{
    QString username;
    QString password;
    QString bio;
    QString avatarEmoji;
    QDateTime joinedAt;
};

struct Comment
{
    int id = 0;
    QString author;
    QString text;
    QDateTime timestamp;
};

struct Post
{
    int id = 0;
    QString author;
    QString content;
    QDateTime timestamp;
    QSet<QString> likedBy;
    DoublyList<Comment> *comments = nullptr;
};

struct Message
{
    QString sender;
    QString receiver;
    QString text;
    QDateTime timestamp;
};

struct Notification
{
    int id = 0;
    QString text;
    QString actor;
    QString type;
    QDateTime timestamp;
    bool read = false;
};

struct UserAction
{
    enum Type
    {
        ActLike,
        ActUnlike,
        ActFollow,
        ActUnfollow,
        ActPost,
        ActDeletePost,
        ActComment,
        ActDeleteComment
    };
    Type type;
    int postId = -1;
    QString targetUser;
    QDateTime timestamp;
};

struct LikeStackEntry
{
    int postId = -1;
    QString postAuthor;
    QString postPreview;
    bool stillLiked = false;
};

class SocialMedia
{
public:
    SocialMedia();
    ~SocialMedia();

    bool registerUser(const QString &username, const QString &password);
    bool validateLogin(const QString &username, const QString &password) const;
    bool userExists(const QString &username) const;
    User getUser(const QString &username) const;
    void updateProfile(const QString &username, const QString &bio, const QString &avatar);

    QVector<QString> allUsers() const;
    QVector<QString> searchUsers(const QString &query) const;

    int addPost(const QString &author, const QString &content);
    QVector<Post *> getAllPostsNewestFirst() const;
    QVector<Post *> getFeedFor(const QString &user) const;
    QVector<Post *> getDiscoverPosts(const QString &user, int limit = 6) const;
    QVector<Post *> getPostsBy(const QString &user) const;
    Post *findPost(int postId) const;
    int totalPosts() const;
    bool deletePost(int postId, const QString &actor);

    int toggleLike(const QString &user, int postId);

    int undoLastLike(const QString &user);

    int likeStackSize(const QString &user) const;
    QVector<LikeStackEntry> getLikeStack(const QString &user, int limit = -1) const;

    int addComment(const QString &author, int postId, const QString &text);
    QVector<Comment> getComments(int postId) const;
    bool deleteComment(int postId, int commentId, const QString &actor);

    bool followUser(const QString &user, const QString &target);
    bool unfollowUser(const QString &user, const QString &target);
    bool isFollowing(const QString &user, const QString &target) const;
    QVector<QString> getFollowing(const QString &user) const;
    QVector<QString> getFollowers(const QString &user) const;
    QVector<QString> getFollowSuggestions(const QString &user, int maxResults = 5) const;

    void sendMessage(const QString &from, const QString &to, const QString &text);
    QVector<Message> getConversation(const QString &a, const QString &b) const;
    QVector<QString> getChatPartners(const QString &user) const;

    void pushNotification(const QString &user, const QString &text,
                          const QString &actor, const QString &type);
    QVector<Notification> getNotifications(const QString &user, bool newestFirst = true) const;
    void markAllRead(const QString &user);
    int unreadCount(const QString &user) const;

    QVector<QPair<QString, int>> getTrendingTopics(int limit = 10) const;

    void seedDemoData();

    void pushUserAction(const QString &user, UserAction::Type type, int postId = -1, const QString &target = {});
    int actionHistorySize(const QString &user) const;
    QVector<UserAction> getActionHistory(const QString &user, int limit = -1) const;
    void clearActionHistory(const QString &user);

private:
    QHash<QString, User> users_;

    DoublyList<Post *> *posts_;
    int nextPostId_;
    int nextCommentId_;

    QHash<QString, Queue<Message> *> chats_;
    QHash<QString, Queue<Notification> *> notifs_;
    QHash<QString, Stack<UserAction> *> actionHistory_;
    int nextNotifId_ = 1;

    static QString chatKey(const QString &a, const QString &b);
    static QStringList extractHashtags(const QString &content);

    Queue<Notification> *notifFor(const QString &user);

    void destroyEverything();
};

#endif
