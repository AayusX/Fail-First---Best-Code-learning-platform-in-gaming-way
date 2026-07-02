#ifndef AVATARSELECTOR_H
#define AVATARSELECTOR_H

#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

class AvatarSelector : public QDialog {
    Q_OBJECT
public:
    explicit AvatarSelector(QWidget *parent = nullptr);
    QString selectedAvatar() const { return m_selectedAvatar; }

signals:
    void avatarSelected(QString avatar);

private slots:
    void onAvatarClicked();

private:
    QString m_selectedAvatar;
    QLabel *m_previewLabel;
    QList<QString> getAvatarList();
};

#endif // AVATARSELECTOR_H
