#include "SocialMedia.h"

#include "follow.hpp"
#include "trending.hpp"
#include "like.hpp"
#include "feed.hpp"
#include "Notifikasi.hpp"
#include "chat.hpp"

#include <QRegularExpression>
#include <algorithm>

static std::string toStd(const QString &s) { return s.toStdString(); }
static QString toQt(const std::string &s) { return QString::fromStdString(s); }

// ─────────────────────────────────────────────────────────
//  PostNode — doubly linked list (gaya feed.cpp)
// ─────────────────────────────────────────────────────────

void SocialMedia::pushBackPost(PostNode *&head, PostNode *&tail, int &size, Post *p)
{
    PostNode *pnew = new PostNode;
    pnew->data = p;
    pnew->prev = tail;
    pnew->next = nullptr;
    if (!head)
    {
        head = tail = pnew;
    }
    else
    {
        tail->next = pnew;
        tail = pnew;
    }
    ++size;
}

void SocialMedia::removePostNode(PostNode *&head, PostNode *&tail, int &size, PostNode *node)
{
    if (!node)
        return;
    if (node->prev)
        node->prev->next = node->next;
    else
        head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        tail = node->prev;
    delete node;
    --size;
}

// ─────────────────────────────────────────────────────────
//  CommentNode — doubly linked list (gaya feed.cpp)
// ─────────────────────────────────────────────────────────

void SocialMedia::pushBackComment(CommentNode *&head, CommentNode *&tail, const Comment &c)
{
    CommentNode *pnew = new CommentNode;
    pnew->data = c;
    pnew->prev = tail;
    pnew->next = nullptr;
    if (!head)
    {
        head = tail = pnew;
    }
    else
    {
        tail->next = pnew;
        tail = pnew;
    }
}

void SocialMedia::removeCommentNode(CommentNode *&head, CommentNode *&tail, CommentNode *node)
{
    if (!node)
        return;
    if (node->prev)
        node->prev->next = node->next;
    else
        head = node->next;
    if (node->next)
        node->next->prev = node->prev;
    else
        tail = node->prev;
    delete node;
}

void SocialMedia::clearCommentList(CommentNode *&head, CommentNode *&tail)
{
    CommentNode *cur = head;
    while (cur)
    {
        CommentNode *del = cur;
        cur = cur->next;
        delete del;
    }
    head = tail = nullptr;
}

// ─────────────────────────────────────────────────────────
//  MsgQueue — queue (gaya chat.cpp)
// ─────────────────────────────────────────────────────────

void SocialMedia::enqueueMsg(MsgQueue &q, const Message &m)
{
    MsgNode *pnew = new MsgNode;
    pnew->data = m;
    pnew->next = nullptr;
    if (!q.head)
    {
        q.head = q.tail = pnew;
    }
    else
    {
        q.tail->next = pnew;
        q.tail = pnew;
    }
}

void SocialMedia::clearMsgQueue(MsgQueue &q)
{
    MsgNode *cur = q.head;
    while (cur)
    {
        MsgNode *del = cur;
        cur = cur->next;
        delete del;
    }
    q.head = q.tail = nullptr;
}

// ─────────────────────────────────────────────────────────
//  NotifQueue — queue (gaya Notifikasi.cpp)
// ─────────────────────────────────────────────────────────

void SocialMedia::enqueueNotif(NotifQueue &q, const Notification &n)
{
    NotifNode *pnew = new NotifNode;
    pnew->data = n;
    pnew->next = nullptr;
    if (!q.head)
    {
        q.head = q.tail = pnew;
    }
    else
    {
        q.tail->next = pnew;
        q.tail = pnew;
    }
}

bool SocialMedia::dequeueNotif(NotifQueue &q, Notification &out)
{
    if (!q.head)
        return false;
    NotifNode *del = q.head;
    out = del->data;
    q.head = del->next;
    if (!q.head)
        q.tail = nullptr;
    delete del;
    return true;
}

void SocialMedia::clearNotifQueue(NotifQueue &q)
{
    NotifNode *cur = q.head;
    while (cur)
    {
        NotifNode *del = cur;
        cur = cur->next;
        delete del;
    }
    q.head = q.tail = nullptr;
}

// ─────────────────────────────────────────────────────────
//  ActionNode — stack (gaya like.cpp)
// ─────────────────────────────────────────────────────────

void SocialMedia::pushAction(ActionNode *&top, const UserAction &a)
{
    ActionNode *pnew = new ActionNode;
    pnew->data = a;
    pnew->next = top;
    top = pnew;
}

void SocialMedia::clearActionStack(ActionNode *&top)
{
    ActionNode *cur = top;
    while (cur)
    {
        ActionNode *del = cur;
        cur = cur->next;
        delete del;
    }
    top = nullptr;
}

// ─────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────

SocialMedia::SocialMedia()
    : postsHead_(nullptr), postsTail_(nullptr), postsSize_(0), nextPostId_(1), nextCommentId_(1)
{
    graph::initgraph();
    trending::inittrending();
    likestack::initlike();
    feed::initfeed();
    notif::initqueue();
    chat::initchat();
}

SocialMedia::~SocialMedia()
{
    destroyEverything();
}

void SocialMedia::destroyEverything()
{
    PostNode *cur = postsHead_;
    while (cur)
    {
        Post *p = cur->data;
        if (p)
        {
            clearCommentList(p->commentsHead, p->commentsTail);
            delete p;
        }
        PostNode *del = cur;
        cur = cur->next;
        delete del;
    }
    postsHead_ = postsTail_ = nullptr;
    postsSize_ = 0;

    for (auto &q : chats_)
        clearMsgQueue(q);
    for (auto &q : notifs_)
        clearNotifQueue(q);
    for (auto &top : actionHistory_)
        clearActionStack(top);
    chats_.clear();
    notifs_.clear();
    actionHistory_.clear();

    trending::resettrending();
    graph::resetgraph();
    likestack::resetlike();
}

// ─────────────────────────────────────────────────────────
//  pushUserAction
// ─────────────────────────────────────────────────────────

void SocialMedia::pushUserAction(const QString &user, UserAction::Type type,
                                 int postId, const QString &target)
{
    if (!actionHistory_.contains(user))
        actionHistory_.insert(user, nullptr);

    UserAction a;
    a.type = type;
    a.postId = postId;
    a.targetUser = target;
    a.timestamp = QDateTime::currentDateTime();
    pushAction(actionHistory_[user], a);
}

int SocialMedia::actionHistorySize(const QString &user) const
{
    auto it = actionHistory_.constFind(user);
    if (it == actionHistory_.constEnd())
        return 0;
    int n = 0;
    ActionNode *cur = it.value();
    while (cur)
    {
        ++n;
        cur = cur->next;
    }
    return n;
}

QVector<UserAction> SocialMedia::getActionHistory(const QString &user, int limit) const
{
    QVector<UserAction> out;
    auto it = actionHistory_.constFind(user);
    if (it == actionHistory_.constEnd())
        return out;
    ActionNode *cur = it.value();
    while (cur)
    {
        if (limit >= 0 && out.size() >= limit)
            break;
        out.push_back(cur->data);
        cur = cur->next;
    }
    return out;
}

void SocialMedia::clearActionHistory(const QString &user)
{
    auto it = actionHistory_.find(user);
    if (it != actionHistory_.end())
        clearActionStack(it.value());
}

// ─────────────────────────────────────────────────────────
//  User
// ─────────────────────────────────────────────────────────

bool SocialMedia::registerUser(const QString &username, const QString &password)
{
    QString u = username.trimmed().toLower();
    if (u.isEmpty() || password.isEmpty())
        return false;
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
        if (it.key().toLower() == u)
            return false;
    User user;
    user.username = u;
    user.password = password;
    user.bio = QStringLiteral("Halo, saya %1 ✨").arg(u);
    user.avatarEmoji = "😀";
    user.joinedAt = QDateTime::currentDateTime();
    users_.insert(u, user);
    graph::tambahuser(toStd(u));
    return true;
}

bool SocialMedia::validateLogin(const QString &username, const QString &password) const
{
    auto it = users_.constFind(username.trimmed().toLower());
    return it != users_.constEnd() && it.value().password == password;
}

bool SocialMedia::userExists(const QString &username) const
{
    return users_.contains(username.trimmed().toLower());
}

User SocialMedia::getUser(const QString &username) const
{
    return users_.value(username.trimmed().toLower());
}

void SocialMedia::updateProfile(const QString &username, const QString &bio, const QString &avatar)
{
    auto it = users_.find(username);
    if (it == users_.end())
        return;
    if (!bio.isNull())
        it.value().bio = bio;
    if (!avatar.isEmpty())
        it.value().avatarEmoji = avatar;
}

QVector<QString> SocialMedia::allUsers() const
{
    QVector<QString> result;
    result.reserve(users_.size());
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
        result.push_back(it.key());
    std::sort(result.begin(), result.end());
    return result;
}

QVector<QString> SocialMedia::searchUsers(const QString &query) const
{
    QVector<QString> result;
    QString q = query.trimmed().toLower();
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
        if (q.isEmpty() || it.key().toLower().contains(q))
            result.push_back(it.key());
    std::sort(result.begin(), result.end());
    return result;
}

// ─────────────────────────────────────────────────────────
//  Post
// ─────────────────────────────────────────────────────────

int SocialMedia::addPost(const QString &author, const QString &content)
{
    if (!userExists(author) || content.trimmed().isEmpty())
        return -1;
    Post *p = new Post;
    p->id = nextPostId_++;
    p->author = author;
    p->content = content.trimmed();
    p->timestamp = QDateTime::currentDateTime();

    pushBackPost(postsHead_, postsTail_, postsSize_, p);

    feed::tambahpostingan(toStd(author), toStd(p->content));
    pushUserAction(author, UserAction::ActPost, p->id);

    for (const auto &t : extractHashtags(content))
        trending::updatetrending(toStd(t), 0);

    return p->id;
}

QVector<Post *> SocialMedia::getAllPostsNewestFirst() const
{
    QVector<Post *> out;
    PostNode *cur = postsTail_;
    while (cur)
    {
        out.push_back(cur->data);
        cur = cur->prev;
    }
    return out;
}

QVector<Post *> SocialMedia::getFeedFor(const QString &user) const
{
    QSet<QString> allowed;
    allowed.insert(user);
    for (const auto &s : graph::getAllFollowing(toStd(user)))
        allowed.insert(toQt(s));

    QVector<Post *> out;
    PostNode *cur = postsTail_;
    while (cur)
    {
        if (allowed.contains(cur->data->author))
            out.push_back(cur->data);
        cur = cur->prev;
    }
    return out;
}

QVector<Post *> SocialMedia::getDiscoverPosts(const QString &user, int limit) const
{
    QSet<QString> allowed;
    allowed.insert(user);
    for (const auto &s : graph::getAllFollowing(toStd(user)))
        allowed.insert(toQt(s));

    QVector<Post *> out;
    PostNode *cur = postsTail_;
    while (cur && out.size() < limit)
    {
        if (!allowed.contains(cur->data->author))
            out.push_back(cur->data);
        cur = cur->prev;
    }
    return out;
}

QVector<Post *> SocialMedia::getPostsBy(const QString &user) const
{
    QVector<Post *> out;
    PostNode *cur = postsTail_;
    while (cur)
    {
        if (cur->data->author == user)
            out.push_back(cur->data);
        cur = cur->prev;
    }
    return out;
}

QVector<Post *> SocialMedia::getPostsByHashtag(const QString &hashtag) const
{
    QVector<Post *> out;
    QString tag = hashtag.trimmed().toLower();
    if (tag.isEmpty())
        return out;
    if (!tag.startsWith('#'))
        tag.prepend('#');

    PostNode *cur = postsTail_;
    while (cur)
    {
        if (extractHashtags(cur->data->content).contains(tag))
            out.push_back(cur->data);
        cur = cur->prev;
    }
    return out;
}

Post *SocialMedia::findPost(int postId) const
{
    PostNode *cur = postsHead_;
    while (cur)
    {
        if (cur->data->id == postId)
            return cur->data;
        cur = cur->next;
    }
    return nullptr;
}

int SocialMedia::totalPosts() const
{
    return postsSize_;
}

bool SocialMedia::deletePost(int postId, const QString &actor)
{
    PostNode *target = nullptr;
    for (PostNode *cur = postsHead_; cur; cur = cur->next)
    {
        if (cur->data && cur->data->id == postId)
        {
            target = cur;
            break;
        }
    }
    if (!target)
        return false;
    Post *p = target->data;
    if (p->author != actor)
        return false;

    const QStringList tags = extractHashtags(p->content);
    const int likeCount = p->likedBy.size();
    auto hasOtherPostWithTag = [this, postId](const QString &tag)
    {
        for (PostNode *cur = postsHead_; cur; cur = cur->next)
        {
            if (!cur->data || cur->data->id == postId)
                continue;
            if (extractHashtags(cur->data->content).contains(tag))
                return true;
        }
        return false;
    };

    for (const auto &t : tags)
    {
        if (hasOtherPostWithTag(t))
            trending::updatetrending(toStd(t), -likeCount);
        else
            trending::hapustrending(toStd(t));
    }

    for (const auto &liker : p->likedBy)
        if (liker != actor)
            pushNotification(liker,
                             QStringLiteral("Post yang kamu like dihapus oleh %1").arg(actor),
                             actor, "system");

    pushUserAction(actor, UserAction::ActDeletePost, postId);
    clearCommentList(p->commentsHead, p->commentsTail);
    delete p;
    removePostNode(postsHead_, postsTail_, postsSize_, target);
    return true;
}

// ─────────────────────────────────────────────────────────
//  Like
// ─────────────────────────────────────────────────────────

int SocialMedia::toggleLike(const QString &user, int postId)
{
    Post *p = findPost(postId);
    if (!p)
        return -1;

    if (p->likedBy.contains(user))
    {
        p->likedBy.remove(user);
        for (const auto &t : extractHashtags(p->content))
            trending::updatetrending(toStd(t), -1);
        pushUserAction(user, UserAction::ActUnlike, postId);
        return 0;
    }
    p->likedBy.insert(user);
    for (const auto &t : extractHashtags(p->content))
        trending::updatetrending(toStd(t), 1);
    likestack::pushlike(toStd(user), postId);
    pushUserAction(user, UserAction::ActLike, postId);

    if (p->author != user)
        pushNotification(p->author,
                         QStringLiteral("%1 menyukai postinganmu").arg(user), user, "like", postId);
    return 1;
}

int SocialMedia::undoLastLike(const QString &user)
{
    while (!likestack::isLikeEmpty(toStd(user)))
    {
        likestack::displaylike top = likestack::poplike(toStd(user));
        if (top.isEmpty)
            break;
        Post *p = findPost(top.postId);
        if (p && p->likedBy.contains(user))
        {
            p->likedBy.remove(user);
            for (const auto &t : extractHashtags(p->content))
                trending::updatetrending(toStd(t), -1);
            return top.postId;
        }
    }
    return -1;
}

int SocialMedia::likeStackSize(const QString &user) const
{
    return likestack::sizeLike(toStd(user));
}

QVector<LikeStackEntry> SocialMedia::getLikeStack(const QString &user, int limit) const
{
    QVector<LikeStackEntry> out;
    QVector<int> buf;

    while (!likestack::isLikeEmpty(toStd(user)))
    {
        likestack::displaylike t = likestack::poplike(toStd(user));
        if (t.isEmpty)
            break;
        buf.push_back(t.postId);
        if (limit > 0 && buf.size() >= limit)
            break;
    }
    for (int i = 0; i < buf.size(); ++i)
    {
        LikeStackEntry e;
        e.postId = buf[i];
        Post *p = findPost(buf[i]);
        if (p)
        {
            e.postAuthor = p->author;
            e.postPreview = p->content.left(48);
            e.stillLiked = p->likedBy.contains(user);
        }
        else
        {
            e.postPreview = "[post sudah dihapus]";
        }
        out.push_back(e);
    }
    for (int i = buf.size() - 1; i >= 0; --i)
        likestack::pushlike(toStd(user), buf[i]);

    return out;
}

// ─────────────────────────────────────────────────────────
//  Comment
// ─────────────────────────────────────────────────────────

int SocialMedia::addComment(const QString &author, int postId, const QString &text)
{
    Post *p = findPost(postId);
    if (!p || text.trimmed().isEmpty() || !userExists(author))
        return -1;

    Comment c;
    c.id = nextCommentId_++;
    c.author = author;
    c.text = text.trimmed();
    c.timestamp = QDateTime::currentDateTime();
    pushBackComment(p->commentsHead, p->commentsTail, c);

    if (p->author != author)
        pushNotification(p->author,
                         QStringLiteral("%1 mengomentari postinganmu").arg(author), author, "comment", postId);
    pushUserAction(author, UserAction::ActComment, postId);
    return c.id;
}

QVector<Comment> SocialMedia::getComments(int postId) const
{
    QVector<Comment> out;
    Post *p = findPost(postId);
    if (!p)
        return out;
    CommentNode *cur = p->commentsHead;
    while (cur)
    {
        out.push_back(cur->data);
        cur = cur->next;
    }
    return out;
}

bool SocialMedia::deleteComment(int postId, int commentId, const QString &actor)
{
    Post *p = findPost(postId);
    if (!p)
        return false;

    CommentNode *target = nullptr;
    for (CommentNode *cur = p->commentsHead; cur; cur = cur->next)
    {
        if (cur->data.id == commentId)
        {
            target = cur;
            break;
        }
    }
    if (!target)
        return false;
    if (target->data.author != actor && p->author != actor)
        return false;

    removeCommentNode(p->commentsHead, p->commentsTail, target);
    pushUserAction(actor, UserAction::ActDeleteComment, postId);
    return true;
}

// ─────────────────────────────────────────────────────────
//  Follow
// ─────────────────────────────────────────────────────────

bool SocialMedia::followUser(const QString &user, const QString &target)
{
    if (user == target || !userExists(user) || !userExists(target))
        return false;
    if (graph::sudahfollow(toStd(user), toStd(target)))
        return false;
    graph::follow(toStd(user), toStd(target));
    pushUserAction(user, UserAction::ActFollow, -1, target);
    pushNotification(target,
                     QStringLiteral("%1 mengikuti kamu").arg(user), user, "follow");
    return true;
}

bool SocialMedia::unfollowUser(const QString &user, const QString &target)
{
    if (!graph::sudahfollow(toStd(user), toStd(target)))
        return false;
    graph::unfollow(toStd(user), toStd(target));
    pushUserAction(user, UserAction::ActUnfollow, -1, target);
    return true;
}

bool SocialMedia::isFollowing(const QString &user, const QString &target) const
{
    return graph::sudahfollow(toStd(user), toStd(target));
}

QVector<QString> SocialMedia::getFollowing(const QString &user) const
{
    QVector<QString> out;
    for (const auto &s : graph::getAllFollowing(toStd(user)))
        out.push_back(toQt(s));
    std::sort(out.begin(), out.end());
    return out;
}

QVector<QString> SocialMedia::getFollowers(const QString &user) const
{
    QVector<QString> out;
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
        if (it.key() != user && graph::sudahfollow(toStd(it.key()), toStd(user)))
            out.push_back(it.key());
    std::sort(out.begin(), out.end());
    return out;
}

QVector<QString> SocialMedia::getFollowSuggestions(const QString &user, int maxResults) const
{
    QSet<QString> myFollowing;
    auto direct = getFollowing(user);
    for (const auto &u : direct)
        myFollowing.insert(u);

    QHash<QString, int> mutual;
    for (const auto &f : direct)
    {
        for (const auto &ff : getFollowing(f))
        {
            if (ff == user || myFollowing.contains(ff))
                continue;
            mutual[ff] += 1;
        }
    }
    if (mutual.isEmpty())
    {
        for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
            if (it.key() != user && !myFollowing.contains(it.key()))
                mutual.insert(it.key(), 0);
    }

    QVector<QPair<QString, int>> sorted;
    sorted.reserve(mutual.size());
    for (auto it = mutual.constBegin(); it != mutual.constEnd(); ++it)
        sorted.push_back({it.key(), it.value()});
    std::sort(sorted.begin(), sorted.end(),
              [](const QPair<QString, int> &a, const QPair<QString, int> &b)
              {
                  return a.second != b.second ? a.second > b.second : a.first < b.first;
              });

    QVector<QString> out;
    for (int i = 0; i < sorted.size() && i < maxResults; ++i)
        out.push_back(sorted[i].first);
    return out;
}

// ─────────────────────────────────────────────────────────
//  Chat
// ─────────────────────────────────────────────────────────

QString SocialMedia::chatKey(const QString &a, const QString &b)
{
    return (a < b) ? a + "|" + b : b + "|" + a;
}

void SocialMedia::sendMessage(const QString &from, const QString &to, const QString &text)
{
    if (!userExists(from) || !userExists(to) || text.trimmed().isEmpty())
        return;
    QString key = chatKey(from, to);
    if (!chats_.contains(key))
        chats_.insert(key, MsgQueue{});

    Message m;
    m.sender = from;
    m.receiver = to;
    m.text = text.trimmed();
    m.timestamp = QDateTime::currentDateTime();
    enqueueMsg(chats_[key], m);
    chat::kirimpesan(toStd(from), toStd(text));

    if (from != to)
        pushNotification(to,
                         QStringLiteral("Pesan baru dari %1").arg(from), from, "message");
}

QVector<Message> SocialMedia::getConversation(const QString &a, const QString &b) const
{
    QVector<Message> out;
    auto it = chats_.constFind(chatKey(a, b));
    if (it == chats_.constEnd())
        return out;
    for (MsgNode *cur = it.value().head; cur; cur = cur->next)
        out.push_back(cur->data);
    return out;
}

QVector<QString> SocialMedia::getChatPartners(const QString &user) const
{
    QSet<QString> seen;
    for (auto it = chats_.constBegin(); it != chats_.constEnd(); ++it)
    {
        const QString &key = it.key();
        int sep = key.indexOf('|');
        if (sep < 0)
            continue;
        QString left = key.left(sep);
        QString right = key.mid(sep + 1);
        if (left == user)
            seen.insert(right);
        else if (right == user)
            seen.insert(left);
    }
    QVector<QString> out;
    for (const auto &p : seen)
        out.push_back(p);
    std::sort(out.begin(), out.end());
    return out;
}

// ─────────────────────────────────────────────────────────
//  Notification
// ─────────────────────────────────────────────────────────

NotifQueue &SocialMedia::notifFor(const QString &user)
{
    if (!notifs_.contains(user))
        notifs_.insert(user, NotifQueue{});
    return notifs_[user];
}

void SocialMedia::pushNotification(const QString &user, const QString &text,
                                   const QString &actor, const QString &type,
                                   int postId)
{
    if (!userExists(user))
        return;
    Notification n;
    n.id = nextNotifId_++;
    n.text = text;
    n.actor = actor;
    n.type = type;
    n.postId = postId;
    n.timestamp = QDateTime::currentDateTime();
    n.read = false;
    enqueueNotif(notifFor(user), n);
    notif::notifmasuk(toStd(user), toStd(text));
}

QVector<Notification> SocialMedia::getNotifications(const QString &user, bool newestFirst) const
{
    QVector<Notification> out;
    auto it = notifs_.constFind(user);
    if (it == notifs_.constEnd())
        return out;
    for (NotifNode *cur = it.value().head; cur; cur = cur->next)
        out.push_back(cur->data);
    if (newestFirst)
        std::reverse(out.begin(), out.end());
    return out;
}

void SocialMedia::markAllRead(const QString &user)
{
    auto it = notifs_.find(user);
    if (it == notifs_.end())
        return;

    QVector<Notification> buf;
    Notification n;
    while (dequeueNotif(it.value(), n))
    {
        n.read = true;
        buf.push_back(n);
    }
    for (const auto &item : buf)
        enqueueNotif(it.value(), item);
}

bool SocialMedia::markNotificationRead(const QString &user, int notificationId)
{
    auto it = notifs_.find(user);
    if (it == notifs_.end())
        return false;

    for (NotifNode *cur = it.value().head; cur; cur = cur->next)
    {
        if (cur->data.id == notificationId)
        {
            cur->data.read = true;
            return true;
        }
    }
    return false;
}

int SocialMedia::unreadCount(const QString &user) const
{
    auto it = notifs_.constFind(user);
    if (it == notifs_.constEnd())
        return 0;
    int c = 0;
    for (NotifNode *cur = it.value().head; cur; cur = cur->next)
        if (!cur->data.read)
            ++c;
    return c;
}

// ─────────────────────────────────────────────────────────
//  Trending
// ─────────────────────────────────────────────────────────

QVector<QPair<QString, int>> SocialMedia::getTrendingTopics(int limit) const
{
    QVector<QPair<QString, int>> out;
    trending::mulailihattrending();
    while (out.size() < limit)
    {
        trending::displaytree d = trending::lihatselanjutnyatrending();
        if (d.isEmpty)
            break;
        out.push_back({toQt(d.namatopik), d.jumlahinteraksi});
    }
    return out;
}

// ─────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────

QStringList SocialMedia::extractHashtags(const QString &content)
{
    QStringList tags;
    QRegularExpression re("#([A-Za-z0-9_]+)");
    auto it = re.globalMatch(content);
    while (it.hasNext())
    {
        auto match = it.next();
        QString tag = "#" + match.captured(1).toLower();
        if (!tags.contains(tag))
            tags.append(tag);
    }
    return tags;
}

// ─────────────────────────────────────────────────────────
//  Seed Demo Data
// ─────────────────────────────────────────────────────────

void SocialMedia::seedDemoData()
{
    if (!users_.isEmpty())
        return;
    registerUser("haikal", "12345");
    registerUser("dzaky", "12345");
    registerUser("nala", "12345");
    registerUser("fassaha", "12345");
    registerUser("hasya", "12345");
    registerUser("rafael", "12345");
    registerUser("justin", "12345");
    registerUser("athar", "12345");
    registerUser("aisha", "12345");
    registerUser("kaila", "12345");
    registerUser("shafira", "12345");
    registerUser("rasya", "12345");
    registerUser("renata", "12345");
    registerUser("fayha", "12345");
    registerUser("syifa", "12345");
    registerUser("nafarrel", "12345");
    registerUser("yunus", "12345");
    registerUser("aliyyah", "12345");
    registerUser("silka", "12345");
    registerUser("jasmine", "12345");
    registerUser("hafidz", "12345");
    registerUser("sherin", "12345");
    registerUser("mufid", "12345");
    registerUser("fikri", "12345");
    registerUser("salsa", "12345");
    registerUser("kayla", "12345");
    registerUser("adrian", "12345");
    registerUser("mikko", "12345");
    registerUser("farrel", "12345");
    registerUser("steven", "12345");

    updateProfile("haikal", "Mahasiswa Informatika - suka struktur data", "🧠");
    updateProfile("dzaky", "Backend engineer dan pecinta clean code", "🚀");
    updateProfile("nala", "Data science enthusiast", "📊");
    updateProfile("fassaha", "Mahasiswa informatika dan pecinta matematika", "📚");
    updateProfile("hasya", "Suka desain dan ilustrasi digital", "🎨");
    updateProfile("rafael", "Penggemar teknologi dan startup", "💡");
    updateProfile("justin", "Belajar web development", "💻");
    updateProfile("athar", "Competitive programming enthusiast", "🏆");
    updateProfile("aisha", "Suka membaca dan menulis", "✍️");
    updateProfile("kaila", "UI/UX learner", "🖌️");
    updateProfile("shafira", "Data enthusiast", "📈");
    updateProfile("rasya", "Pecinta game dan teknologi", "🎮");
    updateProfile("renata", "Frontend developer", "🌸");
    updateProfile("fayha", "Machine learning beginner", "🤖");
    updateProfile("syifa", "Aktif di organisasi kampus", "🌟");
    updateProfile("nafarrel", "Cloud computing enthusiast", "☁️");
    updateProfile("yunus", "Problem solver", "🧩");
    updateProfile("aliyyah", "Suka fotografi", "📷");
    updateProfile("silka", "Belajar mobile development", "📱");
    updateProfile("jasmine", "Content creator", "🎥");
    updateProfile("hafidz", "Cybersecurity enthusiast", "🔐");
    updateProfile("sherin", "Pecinta buku dan kopi", "☕");
    updateProfile("mufid", "Backend developer", "⚙️");
    updateProfile("fikri", "Open source contributor", "❤️");
    updateProfile("salsa", "Frontend enthusiast", "💜");
    updateProfile("kayla", "Visual designer", "✨");
    updateProfile("adrian", "Software engineering learner", "🛠️");
    updateProfile("mikko", "Database enthusiast", "🗄️");
    updateProfile("farrel", "AI enthusiast", "🧠");
    updateProfile("steven", "Linux enthusiast", "🐧");

    followUser("haikal", "dzaky");
    followUser("haikal", "nala");
    followUser("haikal", "athar");
    followUser("haikal", "hafidz");
    followUser("haikal", "rafael");
    followUser("haikal", "renata");
    followUser("haikal", "mufid");
    followUser("haikal", "fikri");

    followUser("dzaky", "haikal");
    followUser("dzaky", "nala");
    followUser("dzaky", "hafidz");
    followUser("dzaky", "athar");
    followUser("dzaky", "mufid");
    followUser("dzaky", "rafael");
    followUser("dzaky", "fassaha");
    followUser("dzaky", "renata");

    followUser("nala", "haikal");
    followUser("nala", "dzaky");
    followUser("nala", "fayha");
    followUser("nala", "shafira");
    followUser("nala", "farrel");
    followUser("nala", "renata");
    followUser("nala", "kayla");

    followUser("fassaha", "haikal");
    followUser("fassaha", "dzaky");
    followUser("fassaha", "nala");
    followUser("fassaha", "athar");
    followUser("fassaha", "fikri");

    followUser("hasya", "renata");
    followUser("hasya", "kayla");
    followUser("hasya", "salsa");
    followUser("hasya", "haikal");

    followUser("rafael", "haikal");
    followUser("rafael", "dzaky");
    followUser("rafael", "athar");
    followUser("rafael", "nala");
    followUser("rafael", "hafidz");
    followUser("rafael", "renata");

    followUser("justin", "haikal");
    followUser("justin", "rafael");
    followUser("justin", "adrian");
    followUser("justin", "dzaky");

    followUser("athar", "haikal");
    followUser("athar", "dzaky");
    followUser("athar", "fikri");
    followUser("athar", "hafidz");
    followUser("athar", "yunus");
    followUser("athar", "rafael");

    followUser("aisha", "hasya");
    followUser("aisha", "jasmine");
    followUser("aisha", "renata");

    followUser("kaila", "hasya");
    followUser("kaila", "renata");
    followUser("kaila", "salsa");
    followUser("kaila", "kayla");

    followUser("shafira", "nala");
    followUser("shafira", "fayha");
    followUser("shafira", "farrel");
    followUser("shafira", "haikal");

    followUser("rasya", "athar");
    followUser("rasya", "rafael");
    followUser("rasya", "haikal");

    followUser("renata", "hasya");
    followUser("renata", "kaila");
    followUser("renata", "salsa");
    followUser("renata", "haikal");
    followUser("renata", "nala");

    followUser("fayha", "nala");
    followUser("fayha", "shafira");
    followUser("fayha", "farrel");
    followUser("fayha", "haikal");

    followUser("syifa", "haikal");
    followUser("syifa", "renata");
    followUser("syifa", "jasmine");

    followUser("nafarrel", "dzaky");
    followUser("nafarrel", "haikal");
    followUser("nafarrel", "mufid");

    followUser("yunus", "athar");
    followUser("yunus", "fikri");
    followUser("yunus", "haikal");

    followUser("aliyyah", "hasya");
    followUser("aliyyah", "renata");
    followUser("aliyyah", "kayla");

    followUser("silka", "justin");
    followUser("silka", "renata");
    followUser("silka", "adrian");

    followUser("jasmine", "hasya");
    followUser("jasmine", "aisha");
    followUser("jasmine", "kayla");

    followUser("hafidz", "haikal");
    followUser("hafidz", "dzaky");
    followUser("hafidz", "fikri");
    followUser("hafidz", "steven");
    followUser("hafidz", "athar");
    followUser("hafidz", "mufid");

    followUser("sherin", "aisha");
    followUser("sherin", "hasya");
    followUser("sherin", "jasmine");

    followUser("mufid", "haikal");
    followUser("mufid", "dzaky");
    followUser("mufid", "mikko");
    followUser("mufid", "nafarrel");
    followUser("mufid", "hafidz");

    followUser("fikri", "haikal");
    followUser("fikri", "hafidz");
    followUser("fikri", "athar");
    followUser("fikri", "yunus");
    followUser("fikri", "dzaky");

    followUser("salsa", "hasya");
    followUser("salsa", "kayla");
    followUser("salsa", "renata");

    followUser("kayla", "hasya");
    followUser("kayla", "renata");
    followUser("kayla", "salsa");
    followUser("kayla", "nala");

    followUser("adrian", "haikal");
    followUser("adrian", "justin");
    followUser("adrian", "dzaky");

    followUser("mikko", "mufid");
    followUser("mikko", "dzaky");
    followUser("mikko", "haikal");

    followUser("farrel", "nala");
    followUser("farrel", "fayha");
    followUser("farrel", "shafira");
    followUser("farrel", "haikal");

    followUser("steven", "hafidz");
    followUser("steven", "fikri");
    followUser("steven", "haikal");
    followUser("steven", "dzaky");

    addPost("haikal", "Hari ini belajar AVL Tree 🌳 #strukturdata");
    addPost("haikal", "Qt ternyata menyenangkan untuk membuat GUI 🤩");
    addPost("haikal", "Lagi debugging linked list selama 2 jam 😭 #cpp");
    addPost("dzaky", "Sedang merapikan REST API project 🚀");
    addPost("dzaky", "Clean code membuat hidup lebih tenang ☕");
    addPost("dzaky", "Caching berhasil mengurangi response time 🔥");
    addPost("nala", "Eksperimen model klasifikasi data 📊");
    addPost("nala", "Belajar visualisasi data menggunakan Python 📈");
    addPost("nala", "Dataset baru untuk proyek machine learning ✨");
    addPost("fassaha", "Matematika diskrit ternyata seru 📚");
    addPost("fassaha", "Graph theory bikin penasaran 🌐");
    addPost("fassaha", "Belajar kombinatorika malam ini 🔢");
    addPost("hasya", "Lagi membuat ilustrasi baru 🎨");
    addPost("rafael", "Ide startup muncul tengah malam 💡");
    addPost("rafael", "Mencari co-founder untuk project baru 🚀");
    addPost("rafael", "Pitch deck akhirnya selesai 🎯");
    addPost("justin", "Belajar React hari ini 💻");
    addPost("athar", "Berhasil AC soal DP yang sulit 🏆");
    addPost("athar", "Ngulik shortest path hari ini 🛣️");
    addPost("athar", "Codeforces malam ini let's go 🔥");
    addPost("aisha", "Menyelesaikan satu buku minggu ini ✍️");
    addPost("kaila", "Redesign halaman login aplikasi 🖌️");
    addPost("shafira", "Data cleaning lebih lama dari modeling 😅");
    addPost("rasya", "Main game sambil belajar logika 🎮");
    addPost("renata", "Membuat landing page responsif 🌸");
    addPost("renata", "Eksplorasi desain dashboard baru 🎨");
    addPost("renata", "Belajar animasi CSS untuk UI ✨");
    addPost("fayha", "Pertama kali training model ML 🤖");
    addPost("syifa", "Rapat organisasi berjalan lancar 🌟");
    addPost("nafarrel", "Deploy aplikasi ke cloud ☁️");
    addPost("yunus", "Algoritma hari ini cukup menantang 🧩");
    addPost("aliyyah", "Hasil foto sunset hari ini 📷");
    addPost("silka", "Belajar Flutter layout 📱");
    addPost("jasmine", "Upload video baru 🎥");
    addPost("hafidz", "Belajar hashing dan enkripsi 🔐");
    addPost("hafidz", "Mencoba CTF sederhana hari ini 💻");
    addPost("hafidz", "Cybersecurity semakin menarik 🚨");
    addPost("sherin", "Ngopi sambil baca buku favorit ☕");
    addPost("mufid", "Optimasi query database ⚙️");
    addPost("mufid", "Belajar indexing untuk performa lebih baik 🚀");
    addPost("fikri", "PR open source diterima ❤️");
    addPost("fikri", "Berhasil memperbaiki bug pertama di project komunitas 🎉");
    addPost("salsa", "Eksplorasi animasi CSS 💜");
    addPost("kayla", "Membuat palet warna baru ✨");
    addPost("adrian", "Belajar design pattern 🛠️");
    addPost("mikko", "Normalisasi database selesai 🗄️");
    addPost("farrel", "Mencoba AI image generation 🧠");
    addPost("farrel", "Prompt engineering ternyata seru 🤖");
    addPost("steven", "Install distro Linux baru 🐧");

    sendMessage("haikal", "dzaky", "Bro, backend project gimana?");
    sendMessage("dzaky", "haikal", "Lagi beresin endpoint terakhir.");
    sendMessage("nala", "haikal", "Dataset yang kemarin masih ada?");
    sendMessage("haikal", "nala", "Masih, nanti aku kirim.");
    sendMessage("hafidz", "dzaky", "Ada referensi belajar API?");
    sendMessage("dzaky", "hafidz", "Coba mulai dari dokumentasi Express.");
    sendMessage("shafira", "nala", "Kamu pakai library apa buat visualisasi?");
    sendMessage("nala", "shafira", "Biasanya matplotlib dan seaborn.");
    sendMessage("renata", "salsa", "Boleh review desain landing page?");
    sendMessage("salsa", "renata", "Boleh, kirim aja.");
    sendMessage("athar", "yunus", "Sudah coba soal graph terbaru?");
    sendMessage("yunus", "athar", "Sudah, lumayan tricky.");
    sendMessage("mufid", "haikal", "Besok jadi presentasi?");
    sendMessage("haikal", "mufid", "Jadi, jam 9 pagi.");
    sendMessage("steven", "hafidz", "Rekomendasi distro Linux?");
    sendMessage("hafidz", "steven", "Fedora atau Ubuntu dulu aja.");
}
