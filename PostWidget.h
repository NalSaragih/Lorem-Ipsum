#ifndef POSTWIDGET_H
#define POSTWIDGET_H

#include <QFrame>
#include <QString>

class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class SocialMedia;
struct Post;

class PostWidget : public QFrame {
    Q_OBJECT
public:
    PostWidget(SocialMedia* sm,
               const QString& currentUser,
               Post* post,
               QWidget* parent = nullptr);

    int postId() const { return postId_; }

signals:
    void changed();
    void deleted(int postId);
    void authorClicked(const QString& author);

private slots:
    void onLikeClicked();
    void onCommentSubmit();
    void onToggleComments();
    void onDeletePost();
    void onDeleteComment(int commentId);

private:
    void rebuild();
    static QString timeAgo(const QDateTime& dt);

    SocialMedia* sm_;
    QString currentUser_;
    int postId_;

    QLabel*      avatarLabel_;
    QPushButton* nameLabel_;
    QLabel*      timeLabel_;
    QLabel* contentLabel_;
    QPushButton* likeButton_;
    QPushButton* commentToggleButton_;
    QPushButton* deleteButton_;
    QLabel* statsLabel_;
    QWidget* commentBox_;
    QVBoxLayout* commentListLayout_;
    QLineEdit* commentInput_;
    QPushButton* commentSendButton_;

    bool commentsExpanded_;
};

#endif // POSTWIDGET_H
