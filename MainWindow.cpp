#include "MainWindow.h"
#include "PostWidget.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDialog>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>

static const char* kBlue    = "#1877F2";
static const char* kBlueDk  = "#1464D2";
static const char* kRed     = "#E0245E";
static const char* kGreen   = "#42B72A";
static const char* kGreenDk = "#36A420";
static const char* kBg      = "#F0F4F8";
static const char* kCard    = "#FFFFFF";
static const char* kBorder  = "#E2E8F0";
static const char* kMuted   = "#718096";
static const char* kText    = "#1A202C";

static QString cardStyle(int radius = 16) {
    return QStringLiteral(
        "QFrame { background:%1; border:1px solid %2; border-radius:%3px; }")
        .arg(kCard, kBorder).arg(radius);
}

static QString primaryBtnStyle() {
    return QStringLiteral(
        "QPushButton { background:%1; color:white; border:none; border-radius:10px; "
        "padding:8px 18px; font-weight:700; font-size:13px; }"
        "QPushButton:hover { background:%2; }"
        "QPushButton:disabled { background:#B0C4E8; }").arg(kBlue, kBlueDk);
}

static QString secondaryBtnStyle() {
    return QStringLiteral(
        "QPushButton { background:#E4E6EB; color:%1; border:none; border-radius:10px; "
        "padding:8px 18px; font-weight:600; font-size:13px; }"
        "QPushButton:hover { background:#D8DADF; }").arg(kText);
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , rootStack_(new QStackedWidget(this))
    , authScreen_(nullptr)
    , loginUser_(new QLineEdit)
    , loginPass_(new QLineEdit)
    , loginStatus_(new QLabel)
    , regUser_(new QLineEdit)
    , regPass_(new QLineEdit)
    , regStatus_(new QLabel)
    , appShell_(nullptr)
    , pageStack_(new QStackedWidget(this))
    , topSearchBar_(new QLineEdit)
    , meBadgeAvatar_(new QLabel)
    , meBadgeName_(new QLabel)
    , logoutBtn_(new QPushButton("Keluar"))
    , trendingLayout_(nullptr)
    , suggestionsLayout_(nullptr)
    , composer_(new QTextEdit)
    , composerPostBtn_(new QPushButton("Posting"))
    , homeFeedLayout_(nullptr)
    , undoLikeBtn_(new QPushButton("⟲  Undo Like"))
    , likeStackHeaderLabel_(new QLabel)
    , likeStackLayout_(nullptr)
    , exploreFeedLayout_(nullptr)
    , notifListLayout_(nullptr)
    , markAllReadBtn_(new QPushButton("Tandai semua dibaca"))
    , chatList_(new QListWidget)
    , chatMessagesLayout_(nullptr)
    , chatScroll_(nullptr)
    , chatInput_(new QLineEdit)
    , chatSendBtn_(new QPushButton("Kirim"))
    , chatHeaderLabel_(new QLabel("Pilih percakapan"))
    , newChatInput_(new QLineEdit)
    , newChatBtn_(new QPushButton("Mulai Chat"))
    , profileAvatarLabel_(new QLabel)
    , profileNameLabel_(new QLabel)
    , profileBioLabel_(new QLabel)
    , profileStatsLabel_(new QLabel)
    , profileEditBtn_(new QPushButton("Edit Profil"))
    , profileFollowBtn_(new QPushButton("Follow"))
    , profileMessageBtn_(new QPushButton("Pesan"))
    , profilePostsLayout_(nullptr)
    , profileFollowingLayout_(nullptr)
    , profileFollowersLayout_(nullptr)
    , searchResultsLayout_(nullptr)
    , searchHeader_(new QLabel) {
    for (int i = 0; i < TabCount; ++i) navButtons_[i] = nullptr;

    setWindowTitle("SocialDS");
    resize(1340, 840);
    setMinimumSize(1100, 700);

    qApp->setStyleSheet(QStringLiteral(R"(
        * {
            font-family: 'Inter', 'SF Pro Text', 'Segoe UI', 'Helvetica Neue', Arial;
            font-size: 13px;
            color: %2;
        }
        QMainWindow { background: %1; }
        QWidget { background: %1; color: %2; }
        QFrame { background: %1; }

        QScrollArea { border: none; background: %1; }
        QScrollArea > QWidget > QWidget { background: %1; }
        QScrollBar:vertical {
            background: %1;
            width: 6px;
            margin: 0;
            border: none;
        }
        QScrollBar::handle:vertical {
            background: #CBD5E0;
            border-radius: 3px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #A0AEC0; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
        QScrollBar:horizontal {
            background: %1;
            height: 6px;
            margin: 0;
            border: none;
        }
        QScrollBar::handle:horizontal { background: #CBD5E0; border-radius: 3px; }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

        QLineEdit {
            background: %5;
            border: 1.5px solid %3;
            border-radius: 10px;
            padding: 9px 14px;
            color: %2;
            font-size: 13px;
            selection-background-color: #BEE3F8;
        }
        QLineEdit:focus {
            border: 2px solid %4;
            background: %5;
        }
        QLineEdit:hover:!focus { border: 1.5px solid #A0AEC0; }

        QTextEdit {
            background: %5;
            border: 1.5px solid %3;
            border-radius: 12px;
            padding: 10px 14px;
            color: %2;
            font-size: 13px;
        }
        QTextEdit:focus { border: 2px solid %4; }

        QListWidget {
            background: %5;
            border: 1px solid %3;
            border-radius: 12px;
            outline: none;
            padding: 4px;
        }
        QListWidget::item {
            padding: 10px 12px;
            border-radius: 8px;
            background: transparent;
        }
        QListWidget::item:selected { background: #EBF5FF; color: %4; }
        QListWidget::item:hover:!selected { background: %1; }

        QDialog { background: %5; }

        QToolTip {
            background: #2D3748;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 5px 10px;
            font-size: 11px;
        }

        QMessageBox { background: %5; }
        QMessageBox QLabel { background: transparent; }
        QMessageBox QPushButton {
            background: %4;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 6px 20px;
            font-weight: 700;
            min-width: 80px;
        }
        QMessageBox QPushButton:hover { background: %6; }
    )").arg(kBg, kText, kBorder, kBlue, kCard, kBlueDk));

    sm_.seedDemoData();
    buildAll();
    setCentralWidget(rootStack_);
    rootStack_->setCurrentWidget(authScreen_);

    auto* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!currentUser_.isEmpty()) refreshRightPanel();
    });
    timer->start(30000);
}

MainWindow::~MainWindow() = default;

void MainWindow::buildAll() {
    authScreen_ = buildAuthScreen();
    appShell_   = buildAppShell();
    rootStack_->addWidget(authScreen_);
    rootStack_->addWidget(appShell_);
}

QWidget* MainWindow::buildAuthScreen() {
    auto* page = new QWidget;
    page->setObjectName("authPage");
    page->setStyleSheet(QStringLiteral(
        "QWidget#authPage { background: qlineargradient("
        "x1:0,y1:0,x2:1,y2:1, stop:0 #1565C0, stop:0.5 #1877F2, stop:1 #42A5F5); }"));

    auto* card = new QFrame(page);
    card->setStyleSheet(QStringLiteral(
        "QFrame { background:%1; border-radius:20px; }").arg(kCard));
    card->setFixedWidth(420);

    auto* logo = new QLabel("SocialDS");
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet(QStringLiteral(
        "font-size:38px; font-weight:900; color:%1; letter-spacing:-1px;").arg(kBlue));

    auto* tagline = new QLabel("Mini sosial media berbasis struktur data");
    tagline->setAlignment(Qt::AlignCenter);
    tagline->setStyleSheet(QStringLiteral("color:%1; font-size:13px;").arg(kMuted));

    auto inputStyle = QStringLiteral(
        "QLineEdit { background:#F0F2F5; border:1.5px solid %1; border-radius:10px; "
        "padding:10px 14px; font-size:14px; }"
        "QLineEdit:focus { border:1.5px solid %2; background:#FFFFFF; }").arg(kBorder, kBlue);

    loginUser_->setPlaceholderText("Username");
    loginUser_->setStyleSheet(inputStyle);
    loginPass_->setPlaceholderText("Password");
    loginPass_->setEchoMode(QLineEdit::Password);
    loginPass_->setStyleSheet(inputStyle);

    auto* loginBtn = new QPushButton("Masuk");
    loginBtn->setMinimumHeight(44);
    loginBtn->setStyleSheet(QStringLiteral(
        "QPushButton { background:%1; color:white; border:none; border-radius:10px; "
        "font-weight:800; font-size:15px; }"
        "QPushButton:hover { background:%2; }"
        "QPushButton:pressed { background:#0F52A8; }").arg(kBlue, kBlueDk));

    loginStatus_->setAlignment(Qt::AlignCenter);
    loginStatus_->setStyleSheet("color:#D32F2F; font-size:12px;");
    loginStatus_->setWordWrap(true);

    auto* loginBox = new QVBoxLayout;
    loginBox->setSpacing(10);
    auto* loginTitle = new QLabel("Masuk ke akunmu");
    loginTitle->setStyleSheet("font-weight:700; font-size:15px;");
    loginBox->addWidget(loginTitle);
    loginBox->addWidget(loginUser_);
    loginBox->addWidget(loginPass_);
    loginBox->addWidget(loginBtn);
    loginBox->addWidget(loginStatus_);

    auto* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet(QStringLiteral("color:%1; margin:4px 0;").arg(kBorder));

    regUser_->setPlaceholderText("Username baru");
    regUser_->setStyleSheet(inputStyle);
    regPass_->setPlaceholderText("Password baru");
    regPass_->setEchoMode(QLineEdit::Password);
    regPass_->setStyleSheet(inputStyle);

    auto* regBtn = new QPushButton("Buat Akun Baru");
    regBtn->setMinimumHeight(44);
    regBtn->setStyleSheet(QStringLiteral(
        "QPushButton { background:%1; color:white; border:none; border-radius:10px; "
        "font-weight:800; font-size:15px; }"
        "QPushButton:hover { background:%2; }"
        "QPushButton:pressed { background:#2D9020; }").arg(kGreen, kGreenDk));

    regStatus_->setAlignment(Qt::AlignCenter);
    regStatus_->setStyleSheet(QStringLiteral("color:%1; font-size:12px;").arg(kBlue));
    regStatus_->setWordWrap(true);

    auto* regBox = new QVBoxLayout;
    regBox->setSpacing(10);
    auto* regTitle = new QLabel("Belum punya akun?");
    regTitle->setStyleSheet("font-weight:700; font-size:15px;");
    regBox->addWidget(regTitle);
    regBox->addWidget(regUser_);
    regBox->addWidget(regPass_);
    regBox->addWidget(regBtn);
    regBox->addWidget(regStatus_);

    auto* demoHint = new QLabel(
        "<span style='color:#65676B; font-size:11px;'>"
        "Demo — <b>haikal / 12345</b> &nbsp;·&nbsp; <b>dzaky / 12345</b> &nbsp;·&nbsp; <b>rayyan / 12345</b>"
        "</span>");
    demoHint->setAlignment(Qt::AlignCenter);
    demoHint->setTextFormat(Qt::RichText);

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(36, 32, 36, 28);
    cardLayout->setSpacing(14);
    cardLayout->addWidget(logo);
    cardLayout->addWidget(tagline);
    cardLayout->addSpacing(4);
    cardLayout->addLayout(loginBox);
    cardLayout->addWidget(sep);
    cardLayout->addLayout(regBox);
    cardLayout->addSpacing(2);
    cardLayout->addWidget(demoHint);

    auto* outer = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignCenter);
    outer->addWidget(card);

    connect(loginBtn,   &QPushButton::clicked,     this, &MainWindow::handleLogin);
    connect(regBtn,     &QPushButton::clicked,     this, &MainWindow::handleRegister);
    connect(loginPass_, &QLineEdit::returnPressed, this, &MainWindow::handleLogin);

    return page;
}

QWidget* MainWindow::buildAppShell() {
    auto* shell = new QWidget;
    auto* shellLayout = new QHBoxLayout(shell);
    shellLayout->setContentsMargins(0, 0, 0, 0);
    shellLayout->setSpacing(0);

    auto* sidebar = buildSidebar();
    auto* center  = new QWidget;
    auto* right   = buildRightPanel();

    auto* centerLayout = new QVBoxLayout(center);
    centerLayout->setContentsMargins(16, 14, 16, 14);
    centerLayout->setSpacing(12);

    auto* topBar = new QFrame;
    topBar->setStyleSheet(QStringLiteral(
        "QFrame { background:%1; border:1px solid %2; border-radius:16px; }").arg(kCard, kBorder));
    auto* topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(14, 10, 14, 10);

    topSearchBar_->setPlaceholderText("🔍  Cari pengguna...");
    topSearchBar_->setMinimumHeight(40);
    topSearchBar_->setStyleSheet(QStringLiteral(
        "QLineEdit { background:%1; border:1.5px solid %2; border-radius:20px; "
        "padding:9px 18px; font-size:13px; }"
        "QLineEdit:focus { border:2px solid %3; background:%4; }").arg(kBg, kBorder, kBlue, kCard));
    topBarLayout->addWidget(topSearchBar_, 1);
    connect(topSearchBar_, &QLineEdit::returnPressed, this, &MainWindow::doSearch);

    pageStack_->addWidget(buildHomePage());
    pageStack_->addWidget(buildExplorePage());
    pageStack_->addWidget(buildNotificationsPage());
    pageStack_->addWidget(buildMessagesPage());
    pageStack_->addWidget(buildProfilePage());
    pageStack_->addWidget(buildSearchPage());

    centerLayout->addWidget(topBar);
    centerLayout->addWidget(pageStack_, 1);

    shellLayout->addWidget(sidebar);
    shellLayout->addWidget(center, 1);
    shellLayout->addWidget(right);

    return shell;
}

QWidget* MainWindow::buildSidebar() {
    auto* sidebar = new QWidget;
    sidebar->setFixedWidth(252);
    sidebar->setObjectName("sidebar");
    sidebar->setStyleSheet(QStringLiteral(
        "QWidget#sidebar { background:%1; border-right:1.5px solid %2; }").arg(kCard, kBorder));

    auto* layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(16, 24, 16, 16);
    layout->setSpacing(4);

    auto* brand = new QLabel("SocialDS");
    brand->setStyleSheet(QStringLiteral(
        "font-size:26px; font-weight:900; color:%1; letter-spacing:-0.5px;").arg(kBlue));
    layout->addWidget(brand);

    auto* brandSub = new QLabel("Struktur Data · Qt6");
    brandSub->setStyleSheet(QStringLiteral(
        "color:%1; font-size:11px; margin-bottom:16px;").arg(kMuted));
    layout->addWidget(brandSub);

    struct NavItem { Tab tab; QString icon; QString label; };
    static const NavItem items[5] = {
        { TabHome,          "🏠", "Beranda"    },
        { TabExplore,       "🧭", "Jelajahi"   },
        { TabNotifications, "🔔", "Notifikasi" },
        { TabMessages,      "💬", "Pesan"       },
        { TabProfile,       "👤", "Profil"      },
    };

    for (const auto& it : items) {
        auto* btn = new QPushButton(QStringLiteral("  %1   %2").arg(it.icon, it.label));
        btn->setCheckable(true);
        btn->setMinimumHeight(46);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(QStringLiteral(R"(
            QPushButton {
                text-align: left;
                padding: 11px 18px;
                border: none;
                background: transparent;
                border-radius: 14px;
                color: %1;
                font-size: 14px;
                font-weight: 600;
            }
            QPushButton:hover { background: %2; }
            QPushButton:checked {
                background: #EBF5FF;
                color: %3;
                font-weight: 800;
                border-left: 3px solid %3;
            }
        )").arg(kText, kBg, kBlue));
        navButtons_[it.tab] = btn;
        layout->addWidget(btn);
    }

    connect(navButtons_[TabHome],          &QPushButton::clicked, this, &MainWindow::goHome);
    connect(navButtons_[TabExplore],       &QPushButton::clicked, this, &MainWindow::goExplore);
    connect(navButtons_[TabNotifications], &QPushButton::clicked, this, &MainWindow::goNotifications);
    connect(navButtons_[TabMessages],      &QPushButton::clicked, this, &MainWindow::goMessages);
    connect(navButtons_[TabProfile],       &QPushButton::clicked, this, &MainWindow::goProfile);

    layout->addStretch();

    auto* meBox = new QFrame;
    meBox->setObjectName("meBox");
    meBox->setStyleSheet(QStringLiteral(
        "QFrame#meBox { background:%1; border:1px solid %2; border-radius:14px; }").arg(kBg, kBorder));
    auto* meLayout = new QHBoxLayout(meBox);
    meLayout->setContentsMargins(10, 8, 10, 8);
    meLayout->setSpacing(10);

    meBadgeAvatar_->setFixedSize(36, 36);
    meBadgeAvatar_->setAlignment(Qt::AlignCenter);
    meBadgeAvatar_->setStyleSheet(QStringLiteral(
        "background:%1; border-radius:18px; font-size:20px; border:2px solid %2;").arg(kCard, kBorder));
    meBadgeName_->setStyleSheet("font-weight:700; font-size:12px;");
    meLayout->addWidget(meBadgeAvatar_);
    meLayout->addWidget(meBadgeName_, 1);
    layout->addWidget(meBox);

    logoutBtn_->setMinimumHeight(38);
    logoutBtn_->setCursor(Qt::PointingHandCursor);
    logoutBtn_->setStyleSheet(R"(
        QPushButton {
            background: #FFF0F0;
            color: #C62828;
            border: 1px solid #FFCDD2;
            border-radius: 10px;
            padding: 8px;
            font-weight: 700;
            font-size: 13px;
        }
        QPushButton:hover { background: #FFCDD2; }
    )");
    layout->addWidget(logoutBtn_);
    connect(logoutBtn_, &QPushButton::clicked, this, &MainWindow::handleLogout);

    return sidebar;
}

QWidget* MainWindow::buildRightPanel() {
    auto* right = new QWidget;
    right->setFixedWidth(292);
    right->setObjectName("rightPanel");
    right->setStyleSheet(QStringLiteral(
        "QWidget#rightPanel { background:%1; border-left:1.5px solid %2; }").arg(kCard, kBorder));

    auto* scroll = new QScrollArea(right);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* container = new QWidget;
    auto* layout = new QVBoxLayout(container);
    layout->setContentsMargins(4, 14, 12, 14);
    layout->setSpacing(12);

    auto mkCard = [&](const QString& title, const QString& desc, QVBoxLayout*& contentLayout) {
        auto* card = new QFrame;
        card->setStyleSheet(cardStyle());
        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(14, 12, 14, 14);
        cl->setSpacing(6);
        auto* t = new QLabel(title);
        t->setStyleSheet("font-weight:800; font-size:14px;");
        cl->addWidget(t);
        if (!desc.isEmpty()) {
            auto* d = new QLabel(desc);
            d->setStyleSheet(QStringLiteral("color:%1; font-size:11px;").arg(kMuted));
            cl->addWidget(d);
        }
        contentLayout = new QVBoxLayout;
        contentLayout->setSpacing(4);
        cl->addLayout(contentLayout);
        return card;
    };

    layout->addWidget(mkCard("🔥  Trending", "Diranking via Binary Search Tree", trendingLayout_));
    layout->addWidget(mkCard("🤝  Mungkin kamu kenal", "Saran via BFS pada graph following", suggestionsLayout_));
    layout->addStretch();

    scroll->setWidget(container);
    auto* outer = new QVBoxLayout(right);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(scroll);
    return right;
}

QWidget* MainWindow::buildHomePage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    auto* composerCard = new QFrame;
    composerCard->setStyleSheet(cardStyle());
    auto* cl = new QVBoxLayout(composerCard);
    cl->setContentsMargins(16, 14, 16, 14);
    cl->setSpacing(10);

    auto* composerTop = new QHBoxLayout;
    auto* composerAvatar = new QLabel("✍️");
    composerAvatar->setFixedSize(38, 38);
    composerAvatar->setAlignment(Qt::AlignCenter);
    composerAvatar->setStyleSheet(QStringLiteral(
        "background:#EEF2FF; border-radius:19px; font-size:20px;"));
    composer_->setPlaceholderText("Apa yang sedang kamu pikirkan?  Gunakan #hashtag...");
    composer_->setMaximumHeight(90);
    composer_->setStyleSheet(QStringLiteral(
        "QTextEdit { background:%1; border:1.5px solid %2; border-radius:12px; "
        "padding:8px 12px; font-size:13px; }"
        "QTextEdit:focus { border:1.5px solid %3; background:%4; }").arg(kBg, kBorder, kBlue, kCard));
    composerTop->addWidget(composerAvatar);
    composerTop->addWidget(composer_, 1);
    cl->addLayout(composerTop);

    composerPostBtn_->setMinimumHeight(36);
    composerPostBtn_->setStyleSheet(primaryBtnStyle());
    auto* composerBottom = new QHBoxLayout;
    composerBottom->addStretch();
    composerBottom->addWidget(composerPostBtn_);
    cl->addLayout(composerBottom);

    auto* likeCard = new QFrame;
    likeCard->setStyleSheet(cardStyle());
    auto* ll = new QVBoxLayout(likeCard);
    ll->setContentsMargins(14, 10, 14, 12);
    ll->setSpacing(6);

    auto* likeHeader = new QHBoxLayout;
    auto* likeTitle = new QLabel("❤️  Like Stack");
    likeTitle->setStyleSheet("font-weight:800; font-size:14px;");
    likeStackHeaderLabel_->setStyleSheet(QStringLiteral("color:%1; font-size:11px;").arg(kMuted));
    likeStackHeaderLabel_->setText("LIFO – like terbaru ada di TOP.");

    undoLikeBtn_->setCursor(Qt::PointingHandCursor);
    undoLikeBtn_->setMinimumHeight(30);
    undoLikeBtn_->setStyleSheet(R"(
        QPushButton {
            background: #FFF0F3;
            color: #C62828;
            border: 1px solid #FFCDD2;
            border-radius: 8px;
            padding: 4px 14px;
            font-size: 12px;
            font-weight: 700;
        }
        QPushButton:hover { background: #FFCDD2; }
        QPushButton:disabled {
            background: #F5F5F5;
            color: #BDBDBD;
            border-color: #E0E0E0;
        }
    )");
    undoLikeBtn_->setEnabled(false);

    likeHeader->addWidget(likeTitle);
    likeHeader->addStretch();
    likeHeader->addWidget(undoLikeBtn_);
    ll->addLayout(likeHeader);
    ll->addWidget(likeStackHeaderLabel_);

    likeStackLayout_ = new QVBoxLayout;
    likeStackLayout_->setSpacing(3);
    ll->addLayout(likeStackLayout_);

    auto* feedScroll = new QScrollArea;
    feedScroll->setWidgetResizable(true);
    feedScroll->setFrameShape(QFrame::NoFrame);
    auto* feedContainer = new QWidget;
    homeFeedLayout_ = new QVBoxLayout(feedContainer);
    homeFeedLayout_->setSpacing(12);
    homeFeedLayout_->setContentsMargins(0, 0, 0, 0);
    homeFeedLayout_->addStretch();
    feedScroll->setWidget(feedContainer);

    layout->addWidget(composerCard);
    layout->addWidget(likeCard);
    layout->addWidget(feedScroll, 1);

    connect(composerPostBtn_, &QPushButton::clicked, this, &MainWindow::handleCreatePost);
    connect(undoLikeBtn_,     &QPushButton::clicked, this, &MainWindow::handleUndoLike);

    return page;
}

QWidget* MainWindow::buildExplorePage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 4, 0);
    layout->setSpacing(12);

    auto* header = new QLabel("🧭  Jelajahi");
    header->setStyleSheet("font-size:20px; font-weight:800; padding:4px 0;");
    auto* sub = new QLabel("Semua postingan terbaru dari seluruh pengguna");
    sub->setStyleSheet(QStringLiteral("color:%1; font-size:12px;").arg(kMuted));
    layout->addWidget(header);
    layout->addWidget(sub);

    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* container = new QWidget;
    exploreFeedLayout_ = new QVBoxLayout(container);
    exploreFeedLayout_->setSpacing(12);
    exploreFeedLayout_->addStretch();
    scroll->setWidget(container);
    layout->addWidget(scroll, 1);

    return page;
}

QWidget* MainWindow::buildNotificationsPage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 4, 0);
    layout->setSpacing(12);

    auto* headerRow = new QHBoxLayout;
    auto* header = new QLabel("🔔  Notifikasi");
    header->setStyleSheet("font-size:20px; font-weight:800;");
    markAllReadBtn_->setMinimumHeight(36);
    markAllReadBtn_->setStyleSheet(primaryBtnStyle());
    headerRow->addWidget(header);
    headerRow->addStretch();
    headerRow->addWidget(markAllReadBtn_);

    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* container = new QWidget;
    notifListLayout_ = new QVBoxLayout(container);
    notifListLayout_->setSpacing(8);
    notifListLayout_->addStretch();
    scroll->setWidget(container);

    layout->addLayout(headerRow);
    layout->addWidget(scroll, 1);

    connect(markAllReadBtn_, &QPushButton::clicked, this, &MainWindow::handleMarkAllRead);

    return page;
}

QWidget* MainWindow::buildMessagesPage() {
    auto* page = new QWidget;
    auto* outer = new QHBoxLayout(page);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(12);

    auto* leftCard = new QFrame;
    leftCard->setStyleSheet(cardStyle());
    leftCard->setFixedWidth(240);
    auto* leftLayout = new QVBoxLayout(leftCard);
    leftLayout->setContentsMargins(12, 14, 12, 12);
    leftLayout->setSpacing(10);

    auto* leftTitle = new QLabel("💬  Percakapan");
    leftTitle->setStyleSheet("font-weight:800; font-size:15px;");
    leftLayout->addWidget(leftTitle);

    newChatInput_->setPlaceholderText("Username...");
    newChatBtn_->setMinimumHeight(34);
    newChatBtn_->setStyleSheet(primaryBtnStyle());
    auto* newChatRow = new QHBoxLayout;
    newChatRow->setSpacing(6);
    newChatRow->addWidget(newChatInput_, 1);
    newChatRow->addWidget(newChatBtn_);
    leftLayout->addLayout(newChatRow);

    chatList_->setStyleSheet("QListWidget { border:none; }");
    leftLayout->addWidget(chatList_, 1);

    auto* rightCard = new QFrame;
    rightCard->setStyleSheet(cardStyle());
    auto* rightLayout = new QVBoxLayout(rightCard);
    rightLayout->setContentsMargins(16, 14, 16, 14);
    rightLayout->setSpacing(10);

    chatHeaderLabel_->setStyleSheet("font-weight:800; font-size:16px;");
    rightLayout->addWidget(chatHeaderLabel_);

    chatScroll_ = new QScrollArea;
    chatScroll_->setWidgetResizable(true);
    chatScroll_->setFrameShape(QFrame::NoFrame);
    chatScroll_->setStyleSheet(QStringLiteral(
        "QScrollArea > QWidget > QWidget { background:%1; }"
        "QScrollArea { background:%1; border:1px solid %2; border-radius:12px; }").arg(kBg, kBorder));
    auto* chatContainer = new QWidget;
    chatMessagesLayout_ = new QVBoxLayout(chatContainer);
    chatMessagesLayout_->setSpacing(6);
    chatMessagesLayout_->addStretch();
    chatScroll_->setWidget(chatContainer);
    rightLayout->addWidget(chatScroll_, 1);

    chatInput_->setPlaceholderText("Tulis pesan...");
    chatSendBtn_->setMinimumHeight(38);
    chatSendBtn_->setStyleSheet(QStringLiteral(
        "QPushButton { background:%1; color:white; border:none; border-radius:19px; "
        "padding:8px 22px; font-weight:700; }"
        "QPushButton:hover { background:%2; }").arg(kBlue, kBlueDk));
    auto* sendRow = new QHBoxLayout;
    sendRow->setSpacing(8);
    sendRow->addWidget(chatInput_, 1);
    sendRow->addWidget(chatSendBtn_);
    rightLayout->addLayout(sendRow);

    outer->addWidget(leftCard);
    outer->addWidget(rightCard, 1);

    connect(chatList_,     &QListWidget::itemSelectionChanged, this, &MainWindow::handleSelectChatPartner);
    connect(chatSendBtn_,  &QPushButton::clicked,              this, &MainWindow::handleSendMessage);
    connect(chatInput_,    &QLineEdit::returnPressed,          this, &MainWindow::handleSendMessage);
    connect(newChatBtn_,   &QPushButton::clicked,              this, &MainWindow::handleStartNewChat);
    connect(newChatInput_, &QLineEdit::returnPressed,          this, &MainWindow::handleStartNewChat);

    return page;
}

QWidget* MainWindow::buildProfilePage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 4, 0);
    layout->setSpacing(12);

    auto* headerCard = new QFrame;
    headerCard->setStyleSheet(cardStyle(16));
    auto* headerLayout = new QHBoxLayout(headerCard);
    headerLayout->setContentsMargins(20, 18, 20, 18);
    headerLayout->setSpacing(18);

    profileAvatarLabel_->setFixedSize(96, 96);
    profileAvatarLabel_->setAlignment(Qt::AlignCenter);
    profileAvatarLabel_->setStyleSheet(
        "background:#EEF2FF; border-radius:48px; font-size:46px; border:3px solid #D4E3FF;");
    headerLayout->addWidget(profileAvatarLabel_);

    auto* infoBox = new QVBoxLayout;
    infoBox->setSpacing(4);
    profileNameLabel_->setStyleSheet("font-size:22px; font-weight:900; letter-spacing:-0.3px;");
    profileBioLabel_->setStyleSheet(QStringLiteral("color:%1; font-size:13px;").arg(kText));
    profileBioLabel_->setWordWrap(true);
    profileStatsLabel_->setStyleSheet(QStringLiteral(
        "color:%1; font-size:12px; margin-top:4px;").arg(kMuted));
    infoBox->addWidget(profileNameLabel_);
    infoBox->addWidget(profileBioLabel_);
    infoBox->addWidget(profileStatsLabel_);

    auto* btnRow = new QHBoxLayout;
    btnRow->setSpacing(8);
    profileEditBtn_->setMinimumHeight(36);
    profileFollowBtn_->setMinimumHeight(36);
    profileMessageBtn_->setMinimumHeight(36);
    profileEditBtn_->setStyleSheet(primaryBtnStyle());
    profileFollowBtn_->setStyleSheet(primaryBtnStyle());
    profileMessageBtn_->setStyleSheet(secondaryBtnStyle());
    btnRow->addWidget(profileEditBtn_);
    btnRow->addWidget(profileFollowBtn_);
    btnRow->addWidget(profileMessageBtn_);
    btnRow->addStretch();
    infoBox->addLayout(btnRow);
    headerLayout->addLayout(infoBox, 1);

    connect(profileEditBtn_,    &QPushButton::clicked, this, &MainWindow::handleEditProfile);
    connect(profileFollowBtn_,  &QPushButton::clicked, this, &MainWindow::handleProfileFollowToggle);
    connect(profileMessageBtn_, &QPushButton::clicked, this, &MainWindow::handleMessageProfile);

    auto* contentRow = new QHBoxLayout;
    contentRow->setSpacing(12);

    auto* postsScroll = new QScrollArea;
    postsScroll->setWidgetResizable(true);
    postsScroll->setFrameShape(QFrame::NoFrame);
    auto* postsContainer = new QWidget;
    auto* postsOuter = new QVBoxLayout(postsContainer);
    postsOuter->setContentsMargins(0, 0, 0, 0);
    postsOuter->setSpacing(10);
    auto* postsTitle = new QLabel("📝  Postingan");
    postsTitle->setStyleSheet("font-weight:800; font-size:15px;");
    postsOuter->addWidget(postsTitle);
    profilePostsLayout_ = new QVBoxLayout;
    profilePostsLayout_->setSpacing(12);
    postsOuter->addLayout(profilePostsLayout_);
    postsOuter->addStretch();
    postsScroll->setWidget(postsContainer);

    auto* relCol = new QVBoxLayout;
    relCol->setSpacing(12);

    auto mkRelCard = [&](const QString& title, QVBoxLayout*& target) {
        auto* card = new QFrame;
        card->setStyleSheet(cardStyle());
        auto* cl = new QVBoxLayout(card);
        cl->setContentsMargins(14, 12, 14, 12);
        auto* t = new QLabel(title);
        t->setStyleSheet("font-weight:800; font-size:13px;");
        cl->addWidget(t);
        target = new QVBoxLayout;
        target->setSpacing(4);
        cl->addLayout(target);
        return card;
    };

    relCol->addWidget(mkRelCard("➡️  Following", profileFollowingLayout_));
    relCol->addWidget(mkRelCard("⬅️  Followers", profileFollowersLayout_));
    relCol->addStretch();

    auto* relScroll = new QScrollArea;
    relScroll->setWidgetResizable(true);
    relScroll->setFrameShape(QFrame::NoFrame);
    auto* relContainer = new QWidget;
    auto* relContainerLayout = new QVBoxLayout(relContainer);
    relContainerLayout->setContentsMargins(0, 0, 0, 0);
    relContainerLayout->addLayout(relCol);
    relScroll->setWidget(relContainer);
    relScroll->setFixedWidth(256);

    contentRow->addWidget(postsScroll, 1);
    contentRow->addWidget(relScroll);

    layout->addWidget(headerCard);
    layout->addLayout(contentRow, 1);

    return page;
}

QWidget* MainWindow::buildSearchPage() {
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 4, 0);
    layout->setSpacing(12);

    searchHeader_->setStyleSheet("font-size:20px; font-weight:800;");
    layout->addWidget(searchHeader_);

    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* container = new QWidget;
    searchResultsLayout_ = new QVBoxLayout(container);
    searchResultsLayout_->setSpacing(8);
    searchResultsLayout_->addStretch();
    scroll->setWidget(container);
    layout->addWidget(scroll, 1);

    return page;
}

void MainWindow::handleLogin() {
    QString u = loginUser_->text().trimmed();
    QString p = loginPass_->text();
    if (u.isEmpty() || p.isEmpty()) {
        loginStatus_->setText("Username dan password tidak boleh kosong.");
        return;
    }
    if (!sm_.validateLogin(u, p)) {
        loginStatus_->setText("Login gagal — username atau password salah.");
        return;
    }
    currentUser_     = u.toLower();
    viewingProfile_  = currentUser_;
    loginStatus_->clear();
    loginUser_->clear();
    loginPass_->clear();
    rootStack_->setCurrentWidget(appShell_);
    switchTab(TabHome);
    refreshAll();
}

void MainWindow::handleRegister() {
    QString u = regUser_->text().trimmed();
    QString p = regPass_->text();
    if (u.isEmpty() || p.isEmpty()) {
        regStatus_->setText("Lengkapi username dan password.");
        regStatus_->setStyleSheet("color:#D32F2F; font-size:12px;");
        return;
    }
    if (!sm_.registerUser(u, p)) {
        regStatus_->setText("Username sudah dipakai (periksa huruf besar/kecil).");
        regStatus_->setStyleSheet("color:#D32F2F; font-size:12px;");
        return;
    }
    regStatus_->setText("✓  Akun berhasil dibuat. Silakan login.");
    regStatus_->setStyleSheet("color:#2E7D32; font-size:12px;");
    regUser_->clear();
    regPass_->clear();
}

void MainWindow::handleLogout() {
    currentUser_.clear();
    viewingProfile_.clear();
    currentChatPartner_.clear();
    rootStack_->setCurrentWidget(authScreen_);
}

void MainWindow::switchTab(Tab tab) {
    pageStack_->setCurrentIndex(tab);
    for (int i = 0; i < TabCount; ++i)
        if (navButtons_[i]) navButtons_[i]->setChecked(i == tab);
}

void MainWindow::goHome()          { switchTab(TabHome);          refreshHome(); }
void MainWindow::goExplore()       { switchTab(TabExplore);       refreshExplore(); }
void MainWindow::goNotifications() { switchTab(TabNotifications); refreshNotifications(); }
void MainWindow::goMessages()      { switchTab(TabMessages);      refreshMessages(); }
void MainWindow::goProfile()       { viewingProfile_ = currentUser_; switchTab(TabProfile); refreshProfile(); }

void MainWindow::doSearch() {
    if (topSearchBar_->text().trimmed().isEmpty()) return;
    switchTab(TabSearch);
    refreshSearch();
}

void MainWindow::viewProfileOf(const QString& user) {
    viewingProfile_ = user;
    switchTab(TabProfile);
    refreshProfile();
}

void MainWindow::startChatWith(const QString& partner) {
    currentChatPartner_ = partner;
    switchTab(TabMessages);
    refreshMessages();
}

void MainWindow::refreshAll() {
    refreshTopBar();
    refreshHome();
    refreshExplore();
    refreshNotifications();
    refreshMessages();
    refreshProfile();
    refreshSearch();
    refreshRightPanel();
}

void MainWindow::refreshTopBar() {
    if (currentUser_.isEmpty()) return;
    User me = sm_.getUser(currentUser_);
    meBadgeAvatar_->setText(me.avatarEmoji.isEmpty() ? "😀" : me.avatarEmoji);
    meBadgeName_->setTextFormat(Qt::RichText);
    meBadgeName_->setText(QStringLiteral(
        "<b>%1</b><br><span style='color:%2; font-size:10px;'>● online</span>")
        .arg(me.username, kBlue));

    int unread = sm_.unreadCount(currentUser_);
    if (navButtons_[TabNotifications]) {
        navButtons_[TabNotifications]->setText(unread > 0
            ? QStringLiteral("  🔔   Notifikasi  (%1)").arg(unread)
            : "  🔔   Notifikasi");
    }
}

void MainWindow::clearLayout(QVBoxLayout* layout) {
    if (!layout) return;
    while (layout->count() > 0) {
        QLayoutItem* item = layout->takeAt(0);
        if (auto* w = item->widget()) {
            w->hide();
            w->setParent(nullptr);
            w->deleteLater();
        } else if (auto* l = item->layout()) {
            while (l->count() > 0) {
                QLayoutItem* sub = l->takeAt(0);
                if (auto* sw = sub->widget()) {
                    sw->hide();
                    sw->setParent(nullptr);
                    sw->deleteLater();
                }
                delete sub;
            }
            delete l;
        }
        delete item;
    }
}

void MainWindow::renderPostsInto(QVBoxLayout* layout, const QVector<Post*>& posts) {
    if (!layout) return;
    clearLayout(layout);
    for (Post* p : posts) {
        auto* w = new PostWidget(&sm_, currentUser_, p);
        connect(w, &PostWidget::changed, this, [this]() {
            refreshTopBar(); refreshRightPanel(); refreshHome();
        });
        connect(w, &PostWidget::deleted, this, [this](int) { refreshAll(); });
        connect(w, &PostWidget::authorClicked, this, &MainWindow::viewProfileOf);
        layout->addWidget(w);
    }
    layout->addStretch();
}

void MainWindow::refreshHome() {
    if (currentUser_.isEmpty()) return;

    clearLayout(homeFeedLayout_);

    auto feed     = sm_.getFeedFor(currentUser_);
    auto discover = sm_.getDiscoverPosts(currentUser_, 6);

    if (feed.isEmpty() && discover.isEmpty()) {
        auto* empty = new QLabel("Belum ada postingan. Follow orang lain untuk melihat feed.");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet(QStringLiteral("color:%1; padding:32px; font-size:13px;").arg(kMuted));
        homeFeedLayout_->addWidget(empty);
    }

    for (Post* p : feed) {
        auto* w = new PostWidget(&sm_, currentUser_, p);
        connect(w, &PostWidget::changed, this, [this]() {
            refreshTopBar(); refreshRightPanel(); refreshHome();
        });
        connect(w, &PostWidget::deleted, this, [this](int) {
            QTimer::singleShot(0, this, [this]() { refreshAll(); });
        });
        connect(w, &PostWidget::authorClicked, this, &MainWindow::viewProfileOf);
        homeFeedLayout_->addWidget(w);
    }

    if (!discover.isEmpty()) {
        auto* divider = new QFrame;
        divider->setObjectName("discoverDivider");
        divider->setStyleSheet(R"(
            QFrame#discoverDivider {
                background: #F0F4F8;
                border: 1px solid #E2E8F0;
                border-radius: 12px;
            }
        )");
        auto* dl = new QHBoxLayout(divider);
        dl->setContentsMargins(16, 10, 16, 10);
        auto* icon = new QLabel("✨");
        icon->setStyleSheet("font-size:16px; background:transparent;");
        auto* txt = new QLabel("Temukan postingan baru");
        txt->setStyleSheet(QStringLiteral(
            "font-weight:800; font-size:13px; background:transparent;"));
        auto* sub = new QLabel("Dari pengguna yang belum kamu ikuti");
        sub->setStyleSheet(QStringLiteral(
            "color:%1; font-size:11px; background:transparent;").arg(kMuted));
        auto* tvbox = new QVBoxLayout;
        tvbox->setSpacing(1);
        tvbox->addWidget(txt);
        tvbox->addWidget(sub);
        dl->addWidget(icon);
        dl->addLayout(tvbox, 1);
        homeFeedLayout_->addWidget(divider);

        for (Post* p : discover) {
            auto* w = new PostWidget(&sm_, currentUser_, p);
            connect(w, &PostWidget::changed, this, [this]() {
                refreshTopBar(); refreshRightPanel(); refreshHome();
            });
            connect(w, &PostWidget::deleted, this, [this](int) {
                QTimer::singleShot(0, this, [this]() { refreshAll(); });
            });
            connect(w, &PostWidget::authorClicked, this, &MainWindow::viewProfileOf);
            homeFeedLayout_->addWidget(w);
        }
    }

    homeFeedLayout_->addStretch();

    clearLayout(likeStackLayout_);
    int stackSize = sm_.likeStackSize(currentUser_);
    likeStackHeaderLabel_->setText(stackSize == 0
        ? "Stack kosong. Like postingan untuk mengisi."
        : QStringLiteral("%1 like tercatat · TOP = like terbaru").arg(stackSize));
    undoLikeBtn_->setEnabled(stackSize > 0);

    auto* topMarker = new QLabel(
        "<span style='color:#E0245E; font-size:10px; font-weight:700;'>▲  TOP  –  like terbaru</span>");
    topMarker->setTextFormat(Qt::RichText);
    topMarker->setAlignment(Qt::AlignCenter);
    topMarker->setStyleSheet(
        "border:1px dashed #E0245E; border-radius:4px; padding:2px 4px; background:#FFF0F3;");
    likeStackLayout_->addWidget(topMarker);

    auto entries = sm_.getLikeStack(currentUser_, 5);
    for (int i = 0; i < entries.size(); ++i) {
        const LikeStackEntry& e = entries[i];
        bool isTop = (i == 0);
        QString style = isTop
            ? "border-left:3px solid #E0245E; background:#FFF5F7; border-radius:4px; padding:4px 8px; margin:1px 0;"
            : "padding:3px 8px; margin:1px 0;";
        QString prefixColor = isTop ? "#E0245E" : kMuted;
        QString prefix      = isTop ? "[TOP]" : QStringLiteral("[%1]").arg(i + 1);
        QString likedMark   = e.stillLiked ? "❤️" : "💔";
        QString preview     = e.postPreview.left(50) + (e.postPreview.length() > 50 ? "…" : "");

        auto* row = new QLabel(QStringLiteral(
            "<span style='color:%4; font-weight:700; font-size:11px;'>%1</span> "
            "%2 <b>@%3</b> "
            "<span style='color:%5; font-size:11px;'>· %6</span>")
            .arg(prefix, likedMark, e.postAuthor, prefixColor, kMuted, preview));
        row->setTextFormat(Qt::RichText);
        row->setWordWrap(true);
        row->setStyleSheet(style);
        likeStackLayout_->addWidget(row);
    }

    if (stackSize > entries.size()) {
        auto* more = new QLabel(QStringLiteral(
            "<i style='color:%1; font-size:11px;'>… %2 entry lainnya</i>")
            .arg(kMuted).arg(stackSize - entries.size()));
        more->setTextFormat(Qt::RichText);
        likeStackLayout_->addWidget(more);
    }

    if (stackSize > 0) {
        auto* botMarker = new QLabel(
            "<span style='color:#65676B; font-size:10px;'>▼  BOTTOM  –  like terlama</span>");
        botMarker->setTextFormat(Qt::RichText);
        botMarker->setAlignment(Qt::AlignCenter);
        botMarker->setStyleSheet(
            "border:1px dashed #DADDE1; border-radius:4px; padding:2px 4px;");
        likeStackLayout_->addWidget(botMarker);
    }
}

void MainWindow::refreshExplore() {
    renderPostsInto(exploreFeedLayout_, sm_.getAllPostsNewestFirst());
}

void MainWindow::refreshNotifications() {
    clearLayout(notifListLayout_);
    if (currentUser_.isEmpty()) return;
    auto notifs = sm_.getNotifications(currentUser_, true);
    if (notifs.isEmpty()) {
        auto* empty = new QLabel("Belum ada notifikasi.");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet(QStringLiteral("color:%1; padding:20px;").arg(kMuted));
        notifListLayout_->addWidget(empty);
    }
    for (const auto& n : notifs) {
        auto* card = new QFrame;
        bool unread = !n.read;
        card->setStyleSheet(QStringLiteral(
            "QFrame { background:%1; border:1.5px solid %2; border-radius:12px; }")
            .arg(unread ? "#EBF3FF" : kCard, unread ? kBlue : kBorder));
        auto* h = new QHBoxLayout(card);
        h->setContentsMargins(14, 10, 14, 10);
        h->setSpacing(12);

        QString icon = "🔔";
        if      (n.type == "like")    icon = "❤️";
        else if (n.type == "comment") icon = "💬";
        else if (n.type == "follow")  icon = "👤";
        else if (n.type == "message") icon = "✉️";

        auto* iconLabel = new QLabel(icon);
        iconLabel->setFixedSize(32, 32);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet("font-size:20px;");

        auto* textLabel = new QLabel(QStringLiteral(
            "<b style='font-size:13px;'>%1</b>"
            "<br><span style='color:%2; font-size:11px;'>%3</span>")
            .arg(n.text, kMuted, n.timestamp.toString("dd MMM yyyy · HH:mm")));
        textLabel->setTextFormat(Qt::RichText);
        textLabel->setWordWrap(true);

        h->addWidget(iconLabel);
        h->addWidget(textLabel, 1);
        notifListLayout_->addWidget(card);
    }
    notifListLayout_->addStretch();
    refreshTopBar();
}

void MainWindow::refreshMessages() {
    if (currentUser_.isEmpty()) return;

    chatList_->blockSignals(true);
    chatList_->clear();

    auto partners  = sm_.getChatPartners(currentUser_);
    auto following = sm_.getFollowing(currentUser_);

    QSet<QString> shown;
    QListWidgetItem* selectedItem = nullptr;

    auto addToList = [&](const QString& p, bool hasMessages) {
        if (shown.contains(p)) return;
        shown.insert(p);
        User u = sm_.getUser(p);
        QString avatar = u.avatarEmoji.isEmpty() ? "😀" : u.avatarEmoji;
        QString label  = hasMessages
            ? QStringLiteral("%1  %2").arg(avatar, p)
            : QStringLiteral("%1  %2  ·  Mulai percakapan").arg(avatar, p);
        auto* item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, p);
        if (!hasMessages)
            item->setForeground(QColor("#718096"));
        chatList_->addItem(item);
        if (p == currentChatPartner_) selectedItem = item;
    };

    for (const auto& p : partners)  addToList(p, true);
    for (const auto& f : following) addToList(f, false);

    if (selectedItem) chatList_->setCurrentItem(selectedItem);
    chatList_->blockSignals(false);

    clearLayout(chatMessagesLayout_);
    if (currentChatPartner_.isEmpty()) {
        chatHeaderLabel_->setText("Pilih percakapan");
        auto* hint = new QLabel("Pilih kontak di kiri atau mulai percakapan baru.");
        hint->setStyleSheet(QStringLiteral("color:%1; padding:24px;").arg(kMuted));
        hint->setAlignment(Qt::AlignCenter);
        chatMessagesLayout_->addWidget(hint);
        chatMessagesLayout_->addStretch();
        return;
    }
    User partner = sm_.getUser(currentChatPartner_);
    QString avatar = partner.avatarEmoji.isEmpty() ? "😀" : partner.avatarEmoji;
    chatHeaderLabel_->setText(QStringLiteral("%1  %2").arg(avatar, currentChatPartner_));

    for (const auto& m : sm_.getConversation(currentUser_, currentChatPartner_))
        chatMessagesLayout_->addWidget(makeChatBubble(m));
    chatMessagesLayout_->addStretch();

    QTimer::singleShot(50, this, [this]() {
        if (chatScroll_ && chatScroll_->verticalScrollBar())
            chatScroll_->verticalScrollBar()->setValue(chatScroll_->verticalScrollBar()->maximum());
    });
}

QWidget* MainWindow::makeChatBubble(const Message& m) {
    bool mine = m.sender == currentUser_;
    auto* row = new QWidget;
    auto* h = new QHBoxLayout(row);
    h->setContentsMargins(8, 3, 8, 3);

    auto* bubble = new QLabel(m.text);
    bubble->setWordWrap(true);
    bubble->setMaximumWidth(400);
    bubble->setTextFormat(Qt::PlainText);
    bubble->setStyleSheet(mine
        ? QStringLiteral("background:%1; color:white; border-radius:16px 16px 4px 16px; "
                         "padding:9px 14px; font-size:13px;").arg(kBlue)
        : "background:#E4E6EB; color:#050505; border-radius:16px 16px 16px 4px; "
          "padding:9px 14px; font-size:13px;");

    auto* time = new QLabel(m.timestamp.toString("HH:mm"));
    time->setStyleSheet(QStringLiteral("color:%1; font-size:10px;").arg(kMuted));

    auto* box = new QVBoxLayout;
    box->setSpacing(2);
    box->setContentsMargins(0, 0, 0, 0);
    box->addWidget(bubble);
    box->addWidget(time, 0, mine ? Qt::AlignRight : Qt::AlignLeft);

    if (mine) { h->addStretch(); h->addLayout(box); }
    else       { h->addLayout(box); h->addStretch(); }

    return row;
}

void MainWindow::refreshProfile() {
    if (viewingProfile_.isEmpty()) viewingProfile_ = currentUser_;
    if (viewingProfile_.isEmpty()) return;

    User u = sm_.getUser(viewingProfile_);
    profileAvatarLabel_->setText(u.avatarEmoji.isEmpty() ? "😀" : u.avatarEmoji);
    profileNameLabel_->setText(u.username);
    profileBioLabel_->setText(u.bio.isEmpty() ? "<i>Belum ada bio</i>" : u.bio);

    profileStatsLabel_->setText(QStringLiteral(
        "📝 %1 post   ·   ➡️ %2 following   ·   ⬅️ %3 followers   ·   bergabung %4")
        .arg(sm_.getPostsBy(viewingProfile_).size())
        .arg(sm_.getFollowing(viewingProfile_).size())
        .arg(sm_.getFollowers(viewingProfile_).size())
        .arg(u.joinedAt.toString("dd MMM yyyy")));

    bool isSelf = viewingProfile_ == currentUser_;
    profileEditBtn_->setVisible(isSelf);
    profileFollowBtn_->setVisible(!isSelf);
    profileMessageBtn_->setVisible(!isSelf);
    if (!isSelf)
        profileFollowBtn_->setText(sm_.isFollowing(currentUser_, viewingProfile_) ? "Unfollow" : "Follow");

    renderPostsInto(profilePostsLayout_, sm_.getPostsBy(viewingProfile_));

    clearLayout(profileFollowingLayout_);
    auto following = sm_.getFollowing(viewingProfile_);
    if (following.isEmpty()) {
        auto* empty = new QLabel("Belum mengikuti siapapun");
        empty->setStyleSheet(QStringLiteral("color:%1; font-size:12px;").arg(kMuted));
        profileFollowingLayout_->addWidget(empty);
    }
    for (const auto& f : following)
        profileFollowingLayout_->addWidget(makeUserCard(f, "", false));

    clearLayout(profileFollowersLayout_);
    auto followers = sm_.getFollowers(viewingProfile_);
    if (followers.isEmpty()) {
        auto* empty = new QLabel("Belum ada followers");
        empty->setStyleSheet(QStringLiteral("color:%1; font-size:12px;").arg(kMuted));
        profileFollowersLayout_->addWidget(empty);
    }
    for (const auto& f : followers)
        profileFollowersLayout_->addWidget(makeUserCard(f, "", false));
}

void MainWindow::refreshSearch() {
    QString q = topSearchBar_->text().trimmed();
    searchHeader_->setText(q.isEmpty()
        ? "🔍  Cari Pengguna"
        : QStringLiteral("🔍  Hasil untuk \"%1\"").arg(q));
    clearLayout(searchResultsLayout_);
    auto users = sm_.searchUsers(q);
    if (users.isEmpty()) {
        auto* empty = new QLabel("Tidak ada pengguna yang cocok.");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet(QStringLiteral("color:%1; padding:24px;").arg(kMuted));
        searchResultsLayout_->addWidget(empty);
    }
    for (const auto& u : users) {
        if (u == currentUser_) continue;
        searchResultsLayout_->addWidget(makeUserCard(u, sm_.getUser(u).bio, true));
    }
    searchResultsLayout_->addStretch();
}

void MainWindow::refreshRightPanel() {
    if (currentUser_.isEmpty()) return;

    clearLayout(trendingLayout_);
    auto trends = sm_.getTrendingTopics(8);
    if (trends.isEmpty()) {
        auto* empty = new QLabel("Belum ada trending.");
        empty->setStyleSheet(QStringLiteral("color:%1; font-size:12px;").arg(kMuted));
        trendingLayout_->addWidget(empty);
    }
    static const char* rankColors[] = { "#E53E3E","#DD6B20","#D69E2E","#38A169","#3182CE","#805AD5","#D53F8C","#2B6CB0" };
    int rank = 0;
    for (const auto& t : trends) {
        auto* row = new QFrame;
        row->setStyleSheet(QStringLiteral(
            "QFrame { background:%1; border:1px solid %2; border-radius:10px; }").arg(kBg, kBorder));
        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(10, 7, 10, 7);
        rl->setSpacing(10);

        auto* chip = new QLabel(QString::number(rank + 1));
        chip->setFixedSize(22, 22);
        chip->setAlignment(Qt::AlignCenter);
        const char* cc = rankColors[rank < 8 ? rank : 7];
        chip->setStyleSheet(QStringLiteral(
            "background:%1; color:white; border-radius:11px; font-weight:800; font-size:11px;").arg(cc));

        auto* topicLabel = new QLabel(t.first);
        topicLabel->setStyleSheet("font-weight:700; font-size:12px; background:transparent;");
        auto* scoreLabel = new QLabel(QStringLiteral("%1").arg(t.second));
        scoreLabel->setStyleSheet(QStringLiteral(
            "color:%1; font-size:10px; background:transparent;").arg(kMuted));

        rl->addWidget(chip);
        rl->addWidget(topicLabel, 1);
        rl->addWidget(scoreLabel);
        trendingLayout_->addWidget(row);
        ++rank;
    }

    clearLayout(suggestionsLayout_);
    auto sugs = sm_.getFollowSuggestions(currentUser_, 5);
    if (sugs.isEmpty()) {
        auto* empty = new QLabel("Tidak ada saran saat ini.");
        empty->setStyleSheet(QStringLiteral("color:%1; font-size:12px;").arg(kMuted));
        suggestionsLayout_->addWidget(empty);
    }
    for (const auto& u : sugs)
        suggestionsLayout_->addWidget(makeUserCard(u, sm_.getUser(u).bio, true));
}

QWidget* MainWindow::makeUserCard(const QString& username, const QString& subtitle, bool showFollow) {
    auto* card = new QFrame;
    card->setStyleSheet(QStringLiteral(
        "QFrame { background:%1; border:1px solid %2; border-radius:10px; }").arg(kBg, kBorder));
    auto* h = new QHBoxLayout(card);
    h->setContentsMargins(10, 8, 10, 8);
    h->setSpacing(10);

    User u = sm_.getUser(username);
    auto* avatar = new QLabel(u.avatarEmoji.isEmpty() ? "😀" : u.avatarEmoji);
    avatar->setFixedSize(36, 36);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setStyleSheet(QStringLiteral(
        "background:%1; border-radius:18px; font-size:18px; border:1.5px solid %2;").arg(kCard, kBorder));

    auto* textBox = new QVBoxLayout;
    textBox->setSpacing(1);
    auto* nameBtn = new QPushButton(username);
    nameBtn->setCursor(Qt::PointingHandCursor);
    nameBtn->setStyleSheet(QStringLiteral(
        "QPushButton { background:transparent; border:none; text-align:left; "
        "font-weight:700; font-size:13px; padding:0; color:%1; }"
        "QPushButton:hover { color:%2; }").arg(kText, kBlue));
    connect(nameBtn, &QPushButton::clicked, this, [this, username]() {
        topSearchBar_->clear();
        viewProfileOf(username);
    });
    textBox->addWidget(nameBtn);
    if (!subtitle.isEmpty()) {
        auto* sub = new QLabel(subtitle);
        sub->setStyleSheet(QStringLiteral("color:%1; font-size:11px;").arg(kMuted));
        sub->setWordWrap(true);
        textBox->addWidget(sub);
    }
    h->addWidget(avatar);
    h->addLayout(textBox, 1);

    if (showFollow && username != currentUser_) {
        bool following = sm_.isFollowing(currentUser_, username);
        auto* btn = new QPushButton(following ? "Following" : "Follow");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(30);
        btn->setStyleSheet(following
            ? "QPushButton { background:#E4E6EB; color:#050505; border:none; border-radius:8px; "
              "padding:4px 14px; font-weight:600; font-size:12px; }"
            : QStringLiteral("QPushButton { background:%1; color:white; border:none; border-radius:8px; "
              "padding:4px 14px; font-weight:700; font-size:12px; }"
              "QPushButton:hover { background:%2; }").arg(kBlue, kBlueDk));
        connect(btn, &QPushButton::clicked, this, [this, username]() {
            if (sm_.isFollowing(currentUser_, username)) sm_.unfollowUser(currentUser_, username);
            else sm_.followUser(currentUser_, username);
            QTimer::singleShot(0, this, [this]() { refreshAll(); });
        });
        h->addWidget(btn);
    }
    return card;
}

void MainWindow::handleCreatePost() {
    if (currentUser_.isEmpty()) return;
    QString content = composer_->toPlainText().trimmed();
    if (content.isEmpty()) return;
    sm_.addPost(currentUser_, content);
    composer_->clear();
    refreshHome();
    refreshExplore();
    refreshProfile();
    refreshRightPanel();
}

void MainWindow::handleUndoLike() {
    if (currentUser_.isEmpty()) return;
    if (sm_.undoLastLike(currentUser_) >= 0) {
        refreshHome();
        refreshExplore();
        refreshProfile();
        refreshRightPanel();
    }
}

void MainWindow::handleEditProfile() {
    if (currentUser_.isEmpty() || viewingProfile_ != currentUser_) return;
    User me = sm_.getUser(currentUser_);

    QDialog dlg(this);
    dlg.setWindowTitle("Edit Profil");
    dlg.setMinimumWidth(360);
    auto* form = new QFormLayout(&dlg);
    form->setSpacing(10);
    form->setContentsMargins(20, 20, 20, 20);

    auto* bioEdit    = new QLineEdit(me.bio);
    auto* avatarEdit = new QLineEdit(me.avatarEmoji);
    avatarEdit->setMaxLength(4);
    avatarEdit->setPlaceholderText("emoji, mis: 🤩");

    form->addRow("Bio:",    bioEdit);
    form->addRow("Avatar:", avatarEdit);

    auto* btnRow    = new QHBoxLayout;
    auto* saveBtn   = new QPushButton("Simpan");
    auto* cancelBtn = new QPushButton("Batal");
    saveBtn->setMinimumHeight(36);
    cancelBtn->setMinimumHeight(36);
    saveBtn->setStyleSheet(primaryBtnStyle());
    cancelBtn->setStyleSheet(secondaryBtnStyle());
    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(saveBtn);
    form->addRow(btnRow);

    connect(saveBtn,   &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        sm_.updateProfile(currentUser_, bioEdit->text(), avatarEdit->text());
        refreshAll();
    }
}

void MainWindow::handleProfileFollowToggle() {
    if (currentUser_.isEmpty() || viewingProfile_.isEmpty() || viewingProfile_ == currentUser_) return;
    if (sm_.isFollowing(currentUser_, viewingProfile_)) sm_.unfollowUser(currentUser_, viewingProfile_);
    else sm_.followUser(currentUser_, viewingProfile_);
    QTimer::singleShot(0, this, [this]() {
        refreshProfile();
        refreshRightPanel();
        refreshHome();
        refreshMessages();
    });
}

void MainWindow::handleMessageProfile() {
    if (viewingProfile_.isEmpty() || viewingProfile_ == currentUser_) return;
    startChatWith(viewingProfile_);
}

void MainWindow::handleSelectChatPartner() {
    QListWidgetItem* item = chatList_->currentItem();
    if (!item) return;
    QString partner = item->data(Qt::UserRole).toString();
    if (partner.isEmpty()) return;
    currentChatPartner_ = partner;
    refreshMessages();
}

void MainWindow::handleSendMessage() {
    if (currentUser_.isEmpty() || currentChatPartner_.isEmpty()) return;
    QString text = chatInput_->text().trimmed();
    if (text.isEmpty()) return;
    sm_.sendMessage(currentUser_, currentChatPartner_, text);
    chatInput_->clear();
    refreshMessages();
    refreshTopBar();
}

void MainWindow::handleStartNewChat() {
    QString partner = newChatInput_->text().trimmed().toLower();
    if (partner.isEmpty()) return;
    if (partner == currentUser_) {
        QMessageBox::information(this, "Info", "Tidak bisa chat ke diri sendiri.");
        return;
    }
    if (!sm_.userExists(partner)) {
        QMessageBox::warning(this, "Tidak ditemukan", "Pengguna tidak terdaftar.");
        return;
    }
    newChatInput_->clear();
    startChatWith(partner);
}

void MainWindow::handleMarkAllRead() {
    if (currentUser_.isEmpty()) return;
    sm_.markAllRead(currentUser_);
    refreshNotifications();
    refreshTopBar();
}
