#include "SocialMedia.h"

#include "follow.hpp"
#include "trending.hpp"
#include "like.hpp"
#include "feed.hpp"
#include "Notifikasi.hpp"
#include "chat.hpp"

#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <algorithm>
#include <queue>

static std::string toStd(const QString &s) { return s.toStdString(); }
static QString toQt(const std::string &s) { return QString::fromStdString(s); }

SocialMedia::SocialMedia()
    : posts_(new DoublyList<Post *>()), nextPostId_(1), nextCommentId_(1)
{
    lorem::initgraph();
    lorem::inittrending();
    lorem::initlike();
    lorem::initfeed();
    lorem::initqueue();
    lorem::initchat();
}

SocialMedia::~SocialMedia()
{
    destroyEverything();
}

void SocialMedia::pushUserAction(const QString &user, UserAction::Type type, int postId, const QString &target)
{
    auto it = actionHistory_.find(user);
    Stack<UserAction> *s;
    if (it == actionHistory_.end())
    {
        s = new Stack<UserAction>();
        actionHistory_.insert(user, s);
    }
    else
    {
        s = it.value();
    }
    UserAction a;
    a.type = type;
    a.postId = postId;
    a.targetUser = target;
    a.timestamp = QDateTime::currentDateTime();
    s->push(a);
}

int SocialMedia::actionHistorySize(const QString &user) const
{
    auto it = actionHistory_.constFind(user);
    return (it == actionHistory_.constEnd()) ? 0 : it.value()->size();
}

QVector<UserAction> SocialMedia::getActionHistory(const QString &user, int limit) const
{
    QVector<UserAction> out;
    auto it = actionHistory_.constFind(user);
    if (it == actionHistory_.constEnd())
        return out;
    it.value()->forEach([&](const UserAction &a)
                        {
        if (limit < 0 || out.size() < limit) out.push_back(a); });
    return out;
}

void SocialMedia::clearActionHistory(const QString &user)
{
    auto it = actionHistory_.find(user);
    if (it != actionHistory_.end())
        it.value()->clear();
}

void SocialMedia::destroyEverything()
{
    if (posts_)
    {
        posts_->forEach([](Post *p)
                        {
            if (p) {
                delete p->comments;
                delete p;
            } });
        delete posts_;
        posts_ = nullptr;
    }
    for (auto *q : chats_)
        delete q;
    for (auto *q : notifs_)
        delete q;
    for (auto *s : actionHistory_)
        delete s;
    chats_.clear();
    notifs_.clear();
    actionHistory_.clear();

    lorem::resettrending();
    lorem::resetgraph();
    lorem::resetlike();
}

bool SocialMedia::registerUser(const QString &username, const QString &password)
{
    QString u = username.trimmed().toLower();
    if (u.isEmpty() || password.isEmpty())
        return false;
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it) {
        if (it.key().toLower() == u) return false;
    }
    User user;
    user.username = u;
    user.password = password;
    user.bio = QStringLiteral("Halo, saya %1 ✨").arg(u);
    user.avatarEmoji = "😀";
    user.joinedAt = QDateTime::currentDateTime();
    users_.insert(u, user);

    lorem::tambahuser(toStd(u));
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
    {
        result.push_back(it.key());
    }
    std::sort(result.begin(), result.end());
    return result;
}

QVector<QString> SocialMedia::searchUsers(const QString &query) const
{
    QVector<QString> result;
    QString q = query.trimmed().toLower();
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
    {
        if (q.isEmpty() || it.key().toLower().contains(q))
        {
            result.push_back(it.key());
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

int SocialMedia::addPost(const QString &author, const QString &content)
{
    if (!userExists(author) || content.trimmed().isEmpty())
        return -1;
    Post *p = new Post();
    p->id = nextPostId_++;
    p->author = author;
    p->content = content.trimmed();
    p->timestamp = QDateTime::currentDateTime();
    p->comments = new DoublyList<Comment>();
    posts_->pushBack(p);

    lorem::tambahpostingan(toStd(author), toStd(p->content));
    pushUserAction(author, UserAction::ActPost, p->id);

    const QStringList tags = extractHashtags(content);
    for (const auto &t : tags)
    {
        lorem::updatetrending(toStd(t), 2);
    }
    return p->id;
}

QVector<Post *> SocialMedia::getAllPostsNewestFirst() const
{
    QVector<Post *> out;
    out.reserve(posts_->size());
    posts_->forEachReversed([&](Post *p)
                            { out.push_back(p); });
    return out;
}

QVector<Post *> SocialMedia::getFeedFor(const QString &user) const
{
    QVector<Post *> out;
    QSet<QString> allowed;
    allowed.insert(user);
    auto fol = getFollowing(user);
    for (const auto &u : fol)
        allowed.insert(u);

    posts_->forEachReversed([&](Post *p)
                            {
        if (allowed.contains(p->author)) out.push_back(p); });
    return out;
}

QVector<Post *> SocialMedia::getDiscoverPosts(const QString &user, int limit) const
{
    QSet<QString> allowed;
    allowed.insert(user);
    for (const auto& s : lorem::getAllFollowing(toStd(user)))
        allowed.insert(toQt(s));

    QVector<Post *> out;
    posts_->forEachReversed([&](Post *p) {
        if (!allowed.contains(p->author) && out.size() < limit)
            out.push_back(p);
    });
    return out;
}

QVector<Post *> SocialMedia::getPostsBy(const QString &user) const
{
    QVector<Post *> out;
    posts_->forEachReversed([&](Post *p)
                            {
        if (p->author == user) out.push_back(p); });
    return out;
}

Post *SocialMedia::findPost(int postId) const
{
    Post *found = nullptr;
    posts_->forEach([&](Post *p)
                    {
        if (!found && p->id == postId) found = p; });
    return found;
}

int SocialMedia::totalPosts() const
{
    return posts_->size();
}

bool SocialMedia::deletePost(int postId, const QString &actor)
{
    if (!posts_)
        return false;
    DoublyList<Post *>::Node *target = nullptr;
    for (auto *n = posts_->head(); n != nullptr; n = n->next)
    {
        if (n->data && n->data->id == postId)
        {
            target = n;
            break;
        }
    }
    if (!target)
        return false;
    Post *p = target->data;
    if (p->author != actor)
        return false;

    const QStringList tags = extractHashtags(p->content);
    int totalBump = 2 + p->likedBy.size();
    for (const auto &t : tags)
    {
        lorem::updatetrending(toStd(t), -totalBump);
    }

    for (const auto &liker : p->likedBy)
    {
        if (liker != actor)
        {
            pushNotification(liker,
                             QStringLiteral("Post yang kamu like dihapus oleh %1").arg(actor),
                             actor, "system");
        }
    }

    pushUserAction(actor, UserAction::ActDeletePost, postId);
    delete p->comments;
    delete p;
    posts_->removeNode(target);
    return true;
}

int SocialMedia::toggleLike(const QString &user, int postId)
{
    Post *p = findPost(postId);
    if (!p)
        return -1;
    const QStringList tags = extractHashtags(p->content);

    if (p->likedBy.contains(user))
    {
        p->likedBy.remove(user);
        for (const auto &t : tags)
            lorem::updatetrending(toStd(t), -1);
        pushUserAction(user, UserAction::ActUnlike, postId);
        return 0;
    }
    p->likedBy.insert(user);
    for (const auto &t : tags)
        lorem::updatetrending(toStd(t), 1);
    lorem::pushlike(toStd(user), postId);
    pushUserAction(user, UserAction::ActLike, postId);

    if (p->author != user)
    {
        pushNotification(p->author,
                         QStringLiteral("%1 menyukai postinganmu").arg(user),
                         user, "like");
    }
    return 1;
}

int SocialMedia::undoLastLike(const QString &user)
{
    while (!lorem::isLikeEmpty(toStd(user)))
    {
        lorem::displaylike top = lorem::poplike(toStd(user));
        if (top.isEmpty)
            break;
        Post *p = findPost(top.postId);
        if (p && p->likedBy.contains(user))
        {
            p->likedBy.remove(user);
            const QStringList tags = extractHashtags(p->content);
            for (const auto &t : tags)
                lorem::updatetrending(toStd(t), -1);
            return top.postId;
        }
    }
    return -1;
}

int SocialMedia::likeStackSize(const QString &user) const
{
    return lorem::sizeLike(toStd(user));
}

QVector<LikeStackEntry> SocialMedia::getLikeStack(const QString &user, int limit) const
{
    QVector<LikeStackEntry> out;
    QVector<int> buf;
    while (!lorem::isLikeEmpty(toStd(user)))
    {
        lorem::displaylike t = lorem::poplike(toStd(user));
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
            e.postPreview = QStringLiteral("[post sudah dihapus]");
        }
        out.push_back(e);
    }
    for (int i = buf.size() - 1; i >= 0; --i)
    {
        lorem::pushlike(toStd(user), buf[i]);
    }
    return out;
}

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
    p->comments->pushBack(c);

    if (p->author != author)
    {
        pushNotification(p->author,
                         QStringLiteral("%1 mengomentari postinganmu").arg(author),
                         author, "comment");
    }
    pushUserAction(author, UserAction::ActComment, postId);
    return c.id;
}

QVector<Comment> SocialMedia::getComments(int postId) const
{
    QVector<Comment> out;
    Post *p = findPost(postId);
    if (!p || !p->comments)
        return out;
    p->comments->forEach([&](const Comment &c)
                         { out.push_back(c); });
    return out;
}

bool SocialMedia::deleteComment(int postId, int commentId, const QString &actor)
{
    Post *p = findPost(postId);
    if (!p || !p->comments)
        return false;
    DoublyList<Comment>::Node *target = nullptr;
    for (auto *n = p->comments->head(); n != nullptr; n = n->next)
    {
        if (n->data.id == commentId)
        {
            target = n;
            break;
        }
    }
    if (!target)
        return false;
    if (target->data.author != actor && p->author != actor)
        return false;
    p->comments->removeNode(target);
    pushUserAction(actor, UserAction::ActDeleteComment, postId);
    return true;
}

bool SocialMedia::followUser(const QString &user, const QString &target)
{
    if (user == target || !userExists(user) || !userExists(target))
        return false;
    if (lorem::sudahfollow(toStd(user), toStd(target)))
        return false;
    lorem::follow(toStd(user), toStd(target));
    pushUserAction(user, UserAction::ActFollow, -1, target);
    pushNotification(target,
                     QStringLiteral("%1 mengikuti kamu").arg(user),
                     user, "follow");
    return true;
}

bool SocialMedia::unfollowUser(const QString &user, const QString &target)
{
    if (!lorem::sudahfollow(toStd(user), toStd(target)))
        return false;
    lorem::unfollow(toStd(user), toStd(target));
    pushUserAction(user, UserAction::ActUnfollow, -1, target);
    return true;
}

bool SocialMedia::isFollowing(const QString &user, const QString &target) const
{
    return lorem::sudahfollow(toStd(user), toStd(target));
}

QVector<QString> SocialMedia::getFollowing(const QString &user) const
{
    QVector<QString> out;
    for (const auto& s : lorem::getAllFollowing(toStd(user)))
        out.push_back(toQt(s));
    std::sort(out.begin(), out.end());
    return out;
}

QVector<QString> SocialMedia::getFollowers(const QString &user) const
{
    QVector<QString> out;
    for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
    {
        if (it.key() == user)
            continue;
        if (lorem::sudahfollow(toStd(it.key()), toStd(user)))
        {
            out.push_back(it.key());
        }
    }
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
        auto fof = getFollowing(f);
        for (const auto &ff : fof)
        {
            if (ff == user || myFollowing.contains(ff))
                continue;
            mutual[ff] += 1;
        }
    }

    if (mutual.isEmpty())
    {
        for (auto it = users_.constBegin(); it != users_.constEnd(); ++it)
        {
            if (it.key() != user && !myFollowing.contains(it.key()))
            {
                mutual.insert(it.key(), 0);
            }
        }
    }

    QVector<QPair<QString, int>> sorted;
    sorted.reserve(mutual.size());
    for (auto it = mutual.constBegin(); it != mutual.constEnd(); ++it)
    {
        sorted.push_back({it.key(), it.value()});
    }
    std::sort(sorted.begin(), sorted.end(),
              [](const QPair<QString, int> &a, const QPair<QString, int> &b)
              {
                  if (a.second != b.second)
                      return a.second > b.second;
                  return a.first < b.first;
              });

    QVector<QString> out;
    for (int i = 0; i < sorted.size() && i < maxResults; ++i)
    {
        out.push_back(sorted[i].first);
    }
    return out;
}

QString SocialMedia::chatKey(const QString &a, const QString &b)
{
    return (a < b) ? a + "|" + b : b + "|" + a;
}

void SocialMedia::sendMessage(const QString &from, const QString &to, const QString &text)
{
    if (!userExists(from) || !userExists(to) || text.trimmed().isEmpty())
        return;
    QString key = chatKey(from, to);
    auto it = chats_.find(key);
    Queue<Message> *q;
    if (it == chats_.end())
    {
        q = new Queue<Message>();
        chats_.insert(key, q);
    }
    else
    {
        q = it.value();
    }
    Message m{from, to, text.trimmed(), QDateTime::currentDateTime()};
    q->enqueue(m);
    lorem::kirimpesan(toStd(from), toStd(text));

    if (from != to)
    {
        pushNotification(to,
                         QStringLiteral("Pesan baru dari %1").arg(from),
                         from, "message");
    }
}

QVector<Message> SocialMedia::getConversation(const QString &a, const QString &b) const
{
    QVector<Message> out;
    auto it = chats_.constFind(chatKey(a, b));
    if (it == chats_.constEnd())
        return out;
    it.value()->forEach([&](const Message &m)
                        { out.push_back(m); });
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

Queue<Notification> *SocialMedia::notifFor(const QString &user)
{
    auto it = notifs_.find(user);
    if (it == notifs_.end())
    {
        auto *q = new Queue<Notification>();
        notifs_.insert(user, q);
        return q;
    }
    return it.value();
}

void SocialMedia::pushNotification(const QString &user, const QString &text,
                                   const QString &actor, const QString &type)
{
    if (!userExists(user))
        return;
    Notification n;
    n.id = nextNotifId_++;
    n.text = text;
    n.actor = actor;
    n.type = type;
    n.timestamp = QDateTime::currentDateTime();
    n.read = false;
    notifFor(user)->enqueue(n);
    lorem::notifmasuk(toStd(user), toStd(text));
}

QVector<Notification> SocialMedia::getNotifications(const QString &user, bool newestFirst) const
{
    QVector<Notification> out;
    auto it = notifs_.constFind(user);
    if (it == notifs_.constEnd())
        return out;
    if (newestFirst)
    {
        it.value()->forEachReversed([&](const Notification &n)
                                    { out.push_back(n); });
    }
    else
    {
        it.value()->forEach([&](const Notification &n)
                            { out.push_back(n); });
    }
    return out;
}

void SocialMedia::markAllRead(const QString &user)
{
    auto it = notifs_.find(user);
    if (it == notifs_.end())
        return;
    Queue<Notification> *q = it.value();
    auto *fresh = new Queue<Notification>();
    Notification n;
    while (q->dequeue(n))
    {
        n.read = true;
        fresh->enqueue(n);
    }
    delete q;
    notifs_.insert(user, fresh);
}

int SocialMedia::unreadCount(const QString &user) const
{
    auto it = notifs_.constFind(user);
    if (it == notifs_.constEnd())
        return 0;
    int c = 0;
    it.value()->forEach([&](const Notification &n)
                        { if (!n.read) ++c; });
    return c;
}

QVector<QPair<QString, int>> SocialMedia::getTrendingTopics(int limit) const
{
    QVector<QPair<QString, int>> out;
    lorem::mulailihattrending();
    while (out.size() < limit)
    {
        lorem::displaytree d = lorem::lihatselanjutnyatrending();
        if (d.isEmpty)
            break;
        out.push_back({toQt(d.namatopik), d.jumlahinteraksi});
    }
    return out;
}

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

void SocialMedia::seedDemoData()
{
    if (!users_.isEmpty())
        return;
    registerUser("haikal", "12345");
    registerUser("dzaky", "12345");
    registerUser("rayyan", "12345");

    updateProfile("haikal", "Mahasiswa Informatika - suka struktur data 🌳", "🧠");
    updateProfile("dzaky", "Backend engineer", "🚀");
    updateProfile("rayyan", "UI/UX enthusiast", "🎨");

    followUser("haikal", "dzaky");
    followUser("haikal", "rayyan");
    followUser("dzaky", "rayyan");
    followUser("rayyan", "haikal");

    addPost("haikal", "Hari ini ngoding doubly linked list. Mind-blown 🤯 #strukturdata #cpp");
    addPost("dzaky", "Priority queue itu seru banget buat antrian pasien 🏥 #priorityqueue #strukturdata");
    addPost("rayyan", "UI Qt makin mantap setelah belajar QStackedWidget. #qt #ui");
    addPost("haikal", "Lagi cobain bikin sosmed pakai Qt! 🤩 #qt #project");

    toggleLike("dzaky", 1);
    toggleLike("haikal", 1);
    toggleLike("rayyan", 2);
    toggleLike("dzaky", 4);

    addComment("dzaky", 1, "Siap pak, semangat!");
    addComment("haikal", 1, "Sudah baca materinya, top!");

    sendMessage("haikal", "dzaky", "Bro, udah ngerjain tugas struktur data?");
    sendMessage("dzaky", "haikal", "Belum, lagi baca slide-nya nih");
    sendMessage("haikal", "dzaky", "Nanti malam ngerjain bareng yuk");
}
