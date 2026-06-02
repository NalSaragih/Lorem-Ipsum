#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <functional>

#include "SocialMedia.h"

class QStackedWidget;
class QListWidget;
class QPushButton;
class QLineEdit;
class QLabel;
class QTextEdit;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;

class PostWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleLogin();
    void handleRegister();
    void handleLogout();

    void goHome();
    void goExplore();
    void goNotifications();
    void goMessages();
    void goProfile();
    void doSearch();

    void handleCreatePost();
    void handleUndoLike();

    void handleEditProfile();
    void handleProfileFollowToggle();
    void handleMessageProfile();

    void handleSelectChatPartner();
    void handleSendMessage();
    void handleStartNewChat();

    void handleMarkAllRead();

private:
    enum Tab { TabHome = 0, TabExplore, TabNotifications, TabMessages, TabProfile, TabSearch, TabCount };

    void buildAll();
    QWidget* buildAuthScreen();
    QWidget* buildAppShell();
    QWidget* buildSidebar();
    QWidget* buildRightPanel();
    QWidget* buildHomePage();
    QWidget* buildExplorePage();
    QWidget* buildNotificationsPage();
    QWidget* buildMessagesPage();
    QWidget* buildProfilePage();
    QWidget* buildSearchPage();

    void switchTab(Tab tab);
    void viewProfileOf(const QString& user);
    void startChatWith(const QString& partner);
    void refreshAll();
    void refreshHome();
    void refreshExplore();
    void refreshNotifications();
    void refreshMessages();
    void refreshProfile();
    void refreshSearch();
    void refreshRightPanel();
    void refreshTopBar();
    void renderPostsInto(QVBoxLayout* layout, const QVector<Post*>& posts);
    void clearLayout(QVBoxLayout* layout);
    QWidget* makeUserCard(const QString& username, const QString& subtitle,
                          bool showFollow, bool showRemove = false,
                          std::function<void()> onActionDone = nullptr);
    QWidget* makeChatBubble(const Message& msg);
    void showFollowListDialog(const QString& title, const QVector<QString>& users,
                              bool isFollowingList);

    SocialMedia sm_;
    QString currentUser_;
    QString viewingProfile_;
    QString currentChatPartner_;

    QStackedWidget* rootStack_;

    QWidget* authScreen_;
    QLineEdit* loginUser_;
    QLineEdit* loginPass_;
    QLabel*    loginStatus_;
    QLineEdit* regUser_;
    QLineEdit* regPass_;
    QLabel*    regStatus_;

    QWidget* appShell_;
    QStackedWidget* pageStack_;
    QPushButton* navButtons_[TabCount];

    QLineEdit* topSearchBar_;
    QLabel* meBadgeAvatar_;
    QLabel* meBadgeName_;
    QPushButton* logoutBtn_;
    QScrollArea* rightScroll_;

    QTextEdit* composer_;
    QPushButton* composerPostBtn_;
    QVBoxLayout* homeFeedLayout_;

    QPushButton* undoLikeBtn_;
    QLabel*      likeStackHeaderLabel_;
    QVBoxLayout* likeStackLayout_;

    QVBoxLayout* exploreFeedLayout_;

    QVBoxLayout* notifListLayout_;
    QPushButton* markAllReadBtn_;

    QListWidget* chatList_;
    QVBoxLayout* chatMessagesLayout_;
    QScrollArea* chatScroll_;
    QLineEdit* chatInput_;
    QPushButton* chatSendBtn_;
    QLabel* chatHeaderLabel_;
    QLineEdit* newChatInput_;
    QPushButton* newChatBtn_;

    QLabel*      profileAvatarLabel_;
    QLabel*      profileNameLabel_;
    QLabel*      profileBioLabel_;
    QLabel*      profilePostsLabel_;
    QPushButton* profileFollowingBtn_;
    QPushButton* profileFollowersBtn_;
    QPushButton* profileEditBtn_;
    QPushButton* profileFollowBtn_;
    QPushButton* profileMessageBtn_;
    QVBoxLayout* profilePostsLayout_;
    QScrollArea* profileRelScroll_;
    QVBoxLayout* profileFollowingLayout_;
    QVBoxLayout* profileFollowersLayout_;

    QVBoxLayout* searchResultsLayout_;
    QLabel* searchHeader_;
};

#endif // MAINWINDOW_H
