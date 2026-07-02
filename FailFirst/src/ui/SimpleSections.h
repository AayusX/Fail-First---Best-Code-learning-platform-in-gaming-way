#ifndef SIMPLESECTIONS_H
#define SIMPLESECTIONS_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

// Home Section (Goals)
class HomeSection : public QWidget {
    Q_OBJECT
public:
    explicit HomeSection(QWidget *parent = nullptr);
    void refresh();
};

// Shop Section
class ShopSection : public QWidget {
    Q_OBJECT
public:
    explicit ShopSection(QWidget *parent = nullptr);
private:
    QWidget* createItemCard(const QString& id, const QString& name, const QString& desc, int price, const QString& icon);
};

// Leaderboard Section
class LeaderboardSection : public QWidget {
    Q_OBJECT
public:
    explicit LeaderboardSection(QWidget *parent = nullptr);
    void refresh();
private:
    QFrame* createRankCard(int rank, const QString& avatar, const QString& username, int xp, int streak, bool isCurrentUser);
    QWidget *m_contentWidget;
    QVBoxLayout *m_contentLayout;
};

// Profile Section
class ProfileSection : public QWidget {
    Q_OBJECT
public:
    explicit ProfileSection(QWidget *parent = nullptr);
    void refresh();
private:
    QWidget* createBadge(const QString& name, bool earned);
    QVBoxLayout *m_layout = nullptr;
    QLabel *m_avatarLabel = nullptr;
};

#endif // SIMPLESECTIONS_H
