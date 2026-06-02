#include "PostWidget.h"
#include "SocialMedia.h"

#include <QBoxLayout>
#include <QDateTime>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

PostWidget::PostWidget(SocialMedia* sm,
                       const QString& currentUser,
                       Post* post,
                       QWidget* parent)
    : QFrame(parent)
    , sm_(sm)
    , currentUser_(currentUser)
    , postId_(post ? post->id : -1)
    , avatarLabel_(new QLabel(this))
    , nameLabel_(new QPushButton(this))
    , timeLabel_(new QLabel(this))
    , contentLabel_(new QLabel(this))
    , likeButton_(new QPushButton(this))
    , commentToggleButton_(new QPushButton(this))
    , deleteButton_(new QPushButton(this))
    , statsLabel_(new QLabel(this))
    , commentBox_(new QWidget(this))
    , commentListLayout_(nullptr)
    , commentInput_(new QLineEdit(this))
    , commentSendButton_(new QPushButton("Kirim", this))
    , commentsExpanded_(false) {

    setObjectName("postCard");
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(R"(
        QFrame#postCard {
            background: #FFFFFF;
            border: 1px solid #E2E8F0;
            border-radius: 16px;
        }
        QFrame#postCard:hover {
            border: 1px solid #CBD5E0;
        }
    )");

    avatarLabel_->setFixedSize(46, 46);
    avatarLabel_->setAlignment(Qt::AlignCenter);
    avatarLabel_->setStyleSheet(
        "background: #EBF4FF; border-radius: 23px; font-size: 24px; "
        "border: 2px solid #BEE3F8;");

    nameLabel_->setFlat(true);
    nameLabel_->setCursor(Qt::PointingHandCursor);
    nameLabel_->setStyleSheet(R"(
        QPushButton {
            font-weight: 800;
            font-size: 14px;
            color: #1A202C;
            background: transparent;
            border: none;
            text-align: left;
            padding: 0;
        }
        QPushButton:hover { color: #1877F2; }
    )");
    connect(nameLabel_, &QPushButton::clicked, this, [this]() {
        Post* p = sm_ ? sm_->findPost(postId_) : nullptr;
        if (p) emit authorClicked(p->author);
    });
    timeLabel_->setStyleSheet(
        "color: #718096; font-size: 11px; background: transparent;");

    contentLabel_->setWordWrap(true);
    contentLabel_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLabel_->setStyleSheet(
        "font-size: 14px; color: #2D3748; line-height: 1.5; padding: 6px 0 4px 0; "
        "background: transparent;");

    static const char* actionBtnStyle = R"(
        QPushButton {
            background: #F7FAFC;
            color: #4A5568;
            border: 1px solid #E2E8F0;
            border-radius: 20px;
            padding: 6px 16px;
            font-weight: 600;
            font-size: 12px;
        }
        QPushButton:hover {
            background: #EBF4FF;
            border-color: #BEE3F8;
            color: #1877F2;
        }
    )";

    likeButton_->setCursor(Qt::PointingHandCursor);
    likeButton_->setMinimumHeight(34);
    likeButton_->setStyleSheet(actionBtnStyle);

    commentToggleButton_->setCursor(Qt::PointingHandCursor);
    commentToggleButton_->setMinimumHeight(34);
    commentToggleButton_->setStyleSheet(actionBtnStyle);

    deleteButton_->setCursor(Qt::PointingHandCursor);
    deleteButton_->setText("🗑");
    deleteButton_->setToolTip("Hapus post");
    deleteButton_->setFixedSize(30, 30);
    deleteButton_->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            border: none;
            font-size: 14px;
            color: #A0AEC0;
            border-radius: 15px;
        }
        QPushButton:hover {
            background: #FFF5F5;
            color: #E53E3E;
        }
    )");

    statsLabel_->setStyleSheet("color: #A0AEC0; font-size: 12px; background: transparent;");

    commentInput_->setPlaceholderText("Tulis komentar...");
    commentInput_->setStyleSheet(R"(
        QLineEdit {
            background: #F7FAFC;
            border: 1.5px solid #E2E8F0;
            border-radius: 18px;
            padding: 8px 16px;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 2px solid #1877F2;
            background: #FFFFFF;
        }
    )");
    commentSendButton_->setMinimumHeight(34);
    commentSendButton_->setStyleSheet(R"(
        QPushButton {
            background: #1877F2;
            color: white;
            border: none;
            border-radius: 17px;
            padding: 6px 18px;
            font-weight: 700;
            font-size: 12px;
        }
        QPushButton:hover { background: #1464D2; }
    )");

    auto* sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("QFrame { background: #F0F4F8; border: none; max-height: 1px; }");

    auto* header = new QHBoxLayout();
    header->setSpacing(12);
    header->addWidget(avatarLabel_);

    auto* nameTimeBox = new QVBoxLayout();
    nameTimeBox->setSpacing(1);
    nameTimeBox->addWidget(nameLabel_);
    nameTimeBox->addWidget(timeLabel_);
    header->addLayout(nameTimeBox);
    header->addStretch();
    header->addWidget(deleteButton_);

    auto* actionRow = new QHBoxLayout();
    actionRow->setSpacing(8);
    actionRow->addWidget(likeButton_);
    actionRow->addWidget(commentToggleButton_);
    actionRow->addStretch();
    actionRow->addWidget(statsLabel_);

    auto* commentBoxLayout = new QVBoxLayout(commentBox_);
    commentBoxLayout->setContentsMargins(0, 8, 0, 0);
    commentBoxLayout->setSpacing(10);
    commentListLayout_ = new QVBoxLayout();
    commentListLayout_->setSpacing(6);
    commentBoxLayout->addLayout(commentListLayout_);
    auto* commentInputRow = new QHBoxLayout();
    commentInputRow->setSpacing(8);
    commentInputRow->addWidget(commentInput_, 1);
    commentInputRow->addWidget(commentSendButton_);
    commentBoxLayout->addLayout(commentInputRow);
    commentBox_->setVisible(false);

    auto* main = new QVBoxLayout(this);
    main->setContentsMargins(18, 16, 18, 16);
    main->setSpacing(10);
    main->addLayout(header);
    main->addWidget(contentLabel_);
    main->addWidget(sep);
    main->addLayout(actionRow);
    main->addWidget(commentBox_);

    connect(likeButton_,          &QPushButton::clicked, this, &PostWidget::onLikeClicked);
    connect(commentToggleButton_,  &QPushButton::clicked, this, &PostWidget::onToggleComments);
    connect(commentSendButton_,    &QPushButton::clicked, this, &PostWidget::onCommentSubmit);
    connect(commentInput_,         &QLineEdit::returnPressed, this, &PostWidget::onCommentSubmit);
    connect(deleteButton_,         &QPushButton::clicked, this, &PostWidget::onDeletePost);

    rebuild();
}

void PostWidget::rebuild() {
    if (!sm_) return;
    Post* p = sm_->findPost(postId_);
    if (!p) return;

    User author = sm_->getUser(p->author);
    avatarLabel_->setText(author.avatarEmoji.isEmpty() ? "😀" : author.avatarEmoji);
    nameLabel_->setText(p->author);
    timeLabel_->setText(timeAgo(p->timestamp));
    contentLabel_->setText(p->content);
    deleteButton_->setVisible(p->author == currentUser_);

    bool liked = p->likedBy.contains(currentUser_);
    if (liked) {
        likeButton_->setText("❤️  Liked");
        likeButton_->setStyleSheet(R"(
            QPushButton {
                background: #FFF5F7;
                color: #E0245E;
                border: 1.5px solid #FEB2C4;
                border-radius: 20px;
                padding: 6px 16px;
                font-weight: 700;
                font-size: 12px;
            }
            QPushButton:hover { background: #FFE4EC; }
        )");
    } else {
        likeButton_->setText("🤍  Like");
        likeButton_->setStyleSheet(R"(
            QPushButton {
                background: #F7FAFC;
                color: #4A5568;
                border: 1px solid #E2E8F0;
                border-radius: 20px;
                padding: 6px 16px;
                font-weight: 600;
                font-size: 12px;
            }
            QPushButton:hover {
                background: #EBF4FF;
                border-color: #BEE3F8;
                color: #1877F2;
            }
        )");
    }

    QVector<Comment> comments = sm_->getComments(p->id);
    int likeCount    = p->likedBy.size();
    int commentCount = comments.size();
    statsLabel_->setText(QStringLiteral("❤️ %1   💬 %2").arg(likeCount).arg(commentCount));

    commentToggleButton_->setText(commentsExpanded_
        ? "💬  Sembunyikan"
        : QStringLiteral("💬  Komentar (%1)").arg(commentCount));

    while (auto* item = commentListLayout_->takeAt(0)) {
        if (auto* w = item->widget()) w->deleteLater();
        delete item;
    }

    QString postAuthor = p->author;
    for (const auto& c : comments) {
        User cu = sm_->getUser(c.author);
        QString avatar = cu.avatarEmoji.isEmpty() ? "😀" : cu.avatarEmoji;

        auto* row = new QFrame(this);
        row->setObjectName("commentRow");
        row->setStyleSheet(R"(
            QFrame#commentRow {
                background: #F7FAFC;
                border: 1px solid #E2E8F0;
                border-radius: 12px;
            }
        )");
        auto* h = new QHBoxLayout(row);
        h->setContentsMargins(12, 8, 8, 8);
        h->setSpacing(10);

        auto* avLabel = new QLabel(avatar, row);
        avLabel->setFixedSize(28, 28);
        avLabel->setAlignment(Qt::AlignCenter);
        avLabel->setStyleSheet(
            "background: #EBF4FF; border-radius: 14px; font-size: 14px;");

        auto* line = new QLabel(row);
        line->setWordWrap(true);
        line->setTextFormat(Qt::RichText);
        line->setStyleSheet("background: transparent;");
        line->setText(QStringLiteral(
            "<span style='font-weight:700; font-size:12px; color:#1A202C;'>%1</span>"
            "<span style='color:#718096; font-size:11px;'> · %2</span>"
            "<br><span style='font-size:13px; color:#2D3748;'>%3</span>")
            .arg(c.author, timeAgo(c.timestamp), c.text.toHtmlEscaped()));

        h->addWidget(avLabel);
        h->addWidget(line, 1);

        if (c.author == currentUser_ || postAuthor == currentUser_) {
            auto* delBtn = new QPushButton("✕", row);
            delBtn->setFixedSize(22, 22);
            delBtn->setCursor(Qt::PointingHandCursor);
            delBtn->setToolTip("Hapus komentar");
            delBtn->setStyleSheet(R"(
                QPushButton {
                    background: transparent;
                    border: none;
                    color: #A0AEC0;
                    border-radius: 11px;
                    font-size: 10px;
                    font-weight: 700;
                }
                QPushButton:hover { background: #FFF5F5; color: #E53E3E; }
            )");
            int commentId = c.id;
            connect(delBtn, &QPushButton::clicked, this, [this, commentId]() {
                onDeleteComment(commentId);
            });
            h->addWidget(delBtn);
        }
        commentListLayout_->addWidget(row);
    }

    commentBox_->setVisible(commentsExpanded_);
}

void PostWidget::onLikeClicked() {
    if (!sm_ || currentUser_.isEmpty()) return;
    sm_->toggleLike(currentUser_, postId_);
    rebuild();
    emit changed();
}

void PostWidget::onCommentSubmit() {
    if (!sm_ || currentUser_.isEmpty()) return;
    QString text = commentInput_->text().trimmed();
    if (text.isEmpty()) return;
    sm_->addComment(currentUser_, postId_, text);
    commentInput_->clear();
    commentsExpanded_ = true;
    rebuild();
    emit changed();
}

void PostWidget::onToggleComments() {
    commentsExpanded_ = !commentsExpanded_;
    rebuild();
}

void PostWidget::onDeletePost() {
    if (!sm_) return;
    auto reply = QMessageBox::question(this, "Hapus Post",
        "Hapus post ini? Skor trending akan berkurang.",
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;
    if (sm_->deletePost(postId_, currentUser_)) emit deleted(postId_);
}

void PostWidget::onDeleteComment(int commentId) {
    if (!sm_) return;
    if (sm_->deleteComment(postId_, commentId, currentUser_)) {
        rebuild();
        emit changed();
    }
}

QString PostWidget::timeAgo(const QDateTime& dt) {
    if (!dt.isValid()) return {};
    qint64 secs = dt.secsTo(QDateTime::currentDateTime());
    if (secs < 5)       return "baru saja";
    if (secs < 60)      return QStringLiteral("%1 dtk lalu").arg(secs);
    if (secs < 3600)    return QStringLiteral("%1 mnt lalu").arg(secs / 60);
    if (secs < 86400)   return QStringLiteral("%1 jam lalu").arg(secs / 3600);
    if (secs < 604800)  return QStringLiteral("%1 hr lalu").arg(secs / 86400);
    return dt.toString("dd MMM yyyy");
}
