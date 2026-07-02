#include "SimpleSections.h"
#include "StyleHelper.h"
#include "AvatarSelector.h"
#include "core/EconomyManager.h"
#include "core/GamificationEngine.h"
#include "database/DatabaseManager.h"
#include "NotificationManager.h"
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QScrollArea>
#include <QSqlQuery>
#include <QSqlError>

// --- HomeSection --- (Actually might not be used if moved to a dashboard view)
HomeSection::HomeSection(QWidget *parent) : QWidget(parent) {
    new QLabel("Home", this);
}
void HomeSection::refresh() {}

// --- ShopSection ---
ShopSection::ShopSection(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *main = new QVBoxLayout(this);
    QLabel *title = new QLabel("✨ Marketplace");
    title->setStyleSheet(StyleHelper::getLabelStyle("header"));
    main->addWidget(title);
    
    // Scrollable area for all items
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background-color: transparent; border: none;");
    
    QWidget *scrollContent = new QWidget;
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    
    // Power-ups Section
    QLabel *powerUpsLabel = new QLabel("⚡ Power-Ups");
    powerUpsLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #fbbf24; margin-top: 10px;");
    scrollLayout->addWidget(powerUpsLabel);
    
    QGridLayout *powerUpsGrid = new QGridLayout;
    powerUpsGrid->setSpacing(20);
    powerUpsGrid->addWidget(createItemCard("health_potion", "Health Potion", "Restore all hearts", 50, "❤️"), 0, 0);
    powerUpsGrid->addWidget(createItemCard("xp_boost", "2x XP Boost", "Double XP for 1 hour", 150, "🔥"), 0, 1);
    powerUpsGrid->addWidget(createItemCard("instant_unlock", "Instant Unlock", "Skip to next lesson", 300, "⚡"), 0, 2);
    powerUpsGrid->addWidget(createItemCard("hint_master", "Hint Master", "Free hints for 24h", 100, "💡"), 1, 0);
    scrollLayout->addLayout(powerUpsGrid);
    
    // Themes Section
    QLabel *themesLabel = new QLabel("🎨 Themes");
    themesLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #a78bfa; margin-top: 20px;");
    scrollLayout->addWidget(themesLabel);
    
    QGridLayout *themesGrid = new QGridLayout;
    themesGrid->setSpacing(20);
    themesGrid->addWidget(createItemCard("theme_midnight", "Midnight Code", "Dark premium theme", 500, "🌙"), 0, 0);
    themesGrid->addWidget(createItemCard("theme_sakura", "Sakura Dreams", "Pink cherry blossom", 500, "🌸"), 0, 1);
    themesGrid->addWidget(createItemCard("theme_ocean", "Ocean Breeze", "Blue aquatic theme", 500, "🌊"), 0, 2);
    themesGrid->addWidget(createItemCard("theme_purple", "Purple Haze", "Violet mystique", 500, "🔮"), 1, 0);
    scrollLayout->addLayout(themesGrid);
    
    // Streak Protection
    QLabel *streakLabel = new QLabel("🛡️ Streak Protection");
    streakLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #06b6d4; margin-top: 20px;");
    scrollLayout->addWidget(streakLabel);
    
    QGridLayout *streakGrid = new QGridLayout;
    streakGrid->setSpacing(20);
    streakGrid->addWidget(createItemCard("streak_freeze", "Streak Freeze", "Protect 1 day", 200, "❄️"), 0, 0);
    streakGrid->addWidget(createItemCard("streak_shield", "Streak Shield", "Protect 7 days", 800, "🛡️"), 0, 1);
    scrollLayout->addLayout(streakGrid);
    
    // Profile Items
    QLabel *profileLabel = new QLabel("👤 Profile Enhancements");
    profileLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #10b981; margin-top: 20px;");
    scrollLayout->addWidget(profileLabel);
    
    QGridLayout *profileGrid = new QGridLayout;
    profileGrid->setSpacing(20);
    profileGrid->addWidget(createItemCard("badge_pack", "Badge Pack", "Unlock 5 badges", 300, "🎖️"), 0, 0);
    profileGrid->addWidget(createItemCard("trophy_case",  "Trophy Case", "Display achievements", 600, "🏆"), 0, 1);
    profileGrid->addWidget(createItemCard("profile_border", "Animated Border", "Glowing profile frame", 400, "✨"), 0, 2);
    scrollLayout->addLayout(profileGrid);
    
    scrollLayout->addStretch();
    scroll->setWidget(scrollContent);
    main->addWidget(scroll);
}

QWidget* ShopSection::createItemCard(const QString& id, const QString& name, const QString& desc, int price, const QString& icon) {
    QFrame *card = new QFrame;
    card->setStyleSheet(StyleHelper::getCardStyle());
    card->setFixedSize(220, 260);
    
    QVBoxLayout *v = new QVBoxLayout(card);
    QLabel *iconLbl = new QLabel(icon);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet("font-size: 48px;");
    
    QLabel *nameLbl = new QLabel(name);
    nameLbl->setStyleSheet("font-weight: bold; font-size: 16px;");
    nameLbl->setAlignment(Qt::AlignCenter);
    
    QLabel *descLbl = new QLabel(desc);
    descLbl->setWordWrap(true);
    descLbl->setAlignment(Qt::AlignCenter);
    descLbl->setStyleSheet("color: #64748b;");
    
    bool owned = DatabaseManager::instance().hasItem(id);
    QPushButton *buyBtn;
    if (owned) {
        buyBtn = new QPushButton("✓ Owned");
        buyBtn->setStyleSheet(
            "QPushButton { background: #d1fae5; color: #065f46; border: 2px solid #6ee7b7; "
            "border-radius: 12px; padding: 10px 20px; font-weight: 700; font-size: 14px; }"
        );
        buyBtn->setEnabled(false);
    } else {
        buyBtn = new QPushButton(QString("%1 Bits").arg(price));
        buyBtn->setStyleSheet(StyleHelper::getButtonStyle("primary"));
        connect(buyBtn, &QPushButton::clicked, [id, price](){
            EconomyManager::instance().purchaseItem(id, price);
        });
    }
    
    v->addWidget(iconLbl);
    v->addWidget(nameLbl);
    v->addWidget(descLbl);
    v->addWidget(buyBtn);
    
    return card;
}

// --- LeaderboardSection ---
LeaderboardSection::LeaderboardSection(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QLabel *title = new QLabel("🏆 CHAMPIONS LEADERBOARD");
    title->setStyleSheet("font-family: 'Inter'; font-size: 28px; font-weight: 900; color: #fbbf24; text-align: center; margin: 20px;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);
    
    // Scrollable area for rankings
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background-color: transparent; border: none;");
    
    m_contentWidget = new QWidget;
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setSpacing(10);
    m_contentLayout->setContentsMargins(20, 10, 20, 10);
    
    scroll->setWidget(m_contentWidget);
    layout->addWidget(scroll);
    
    refresh();
}

void LeaderboardSection::refresh() {
    // Clear existing items
    QLayoutItem *child;
    while ((child = m_contentLayout->takeAt(0)) != 0) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    
    // Fetch all users sorted by XP
    QSqlQuery query;
    query.prepare("SELECT username, avatar, xp, streak FROM users ORDER BY xp DESC LIMIT 10");
    query.exec();
    
    int rank = 1;
    QString currentUser = DatabaseManager::instance().getCurrentUsername();
    
    while (query.next()) {
        QString username = query.value(0).toString();
        QString avatar = query.value(1).toString();
        if (avatar.isEmpty()) avatar = "👤";
        int xp = query.value(2).toInt();
        int streak = query.value(3).toInt();
        
        QFrame *rankCard = createRankCard(rank, avatar, username, xp, streak, username == currentUser);
        m_contentLayout->addWidget(rankCard);
        rank++;
    }
    
    m_contentLayout->addStretch();
}

QFrame* LeaderboardSection::createRankCard(int rank, const QString& avatar, const QString& username, int xp, int streak, bool isCurrentUser) {
    QFrame *card = new QFrame;
    card->setFixedHeight(80);
    
    // Gradient backgrounds for top 3
    QString bgColor;
    QString rankIcon;
    if (rank == 1) {
        bgColor = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fbbf24, stop:1 #f59e0b)"; // Gold
        rankIcon = "🥇";
    } else if (rank == 2) {
        bgColor = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #94a3b8, stop:1 #64748b)"; // Silver
        rankIcon = "🥈";
    } else if (rank == 3) {
        bgColor = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #d97706, stop:1 #92400e)"; // Bronze
        rankIcon = "🥉";
    } else {
        bgColor = "#f8f9fa"; // Light gray for light theme
        rankIcon = QString("#%1").arg(rank);
    }
    
    // Highlight current user
    QString borderColor = isCurrentUser ? "#667eea" : "#e2e8f0";
    QString borderWidth = isCurrentUser ? "3px" : "1px";
    
    QString textColor = (rank <= 3) ? "white" : "#1a202c"; // White text for top 3, dark for others
    
    card->setStyleSheet(QString(
        "QFrame { "
        "  background: %1; "
        "  border: %2 solid %3; "
        "  border-radius: 12px; "
        "  padding: 10px; "
        "}"
    ).arg(bgColor, borderWidth, borderColor));
    
    QHBoxLayout *layout = new QHBoxLayout(card);
    
    // Rank Icon
    QLabel *rankLabel = new QLabel(rankIcon);
    rankLabel->setStyleSheet(QString("font-size: 32px; font-weight: 900; color: %1;").arg(textColor));
    rankLabel->setFixedWidth(60);
    rankLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(rankLabel);
    
    // Avatar
    QLabel *avatarLabel = new QLabel(avatar);
    avatarLabel->setStyleSheet("font-size: 48px; background-color: transparent;");
    avatarLabel->setFixedWidth(60);
    avatarLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(avatarLabel);
    
    // Username
    QLabel *nameLabel = new QLabel(username);
    nameLabel->setStyleSheet(QString("font-size: 20px; font-weight: 700; color: %1;").arg(textColor));
    layout->addWidget(nameLabel);
    
    layout->addStretch();
    
    // XP
    QLabel *xpLabel = new QLabel(QString("⭐ %1 XP").arg(xp));
    xpLabel->setStyleSheet(QString("font-size: 18px; font-weight: 600; color: %1;").arg(textColor));
    layout->addWidget(xpLabel);
    
    // Streak
    QLabel *streakLabel = new QLabel(QString("🔥 %1").arg(streak));
    streakLabel->setStyleSheet(QString("font-size: 18px; font-weight: 600; color: %1; margin-left: 20px;").arg(textColor));
    layout->addWidget(streakLabel);
    
    return card;
}

// --- ProfileSection ---
ProfileSection::ProfileSection(QWidget *parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);
    refresh();
}

void ProfileSection::refresh() {
    // Clear existing layout
    QLayoutItem *child;
    while ((child = m_layout->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    // Header
    QHBoxLayout *header = new QHBoxLayout;

    QString userAvatar = DatabaseManager::instance().getAvatar();
    m_avatarLabel = new QLabel(userAvatar);
    m_avatarLabel->setStyleSheet("font-size: 80px; background-color: #334155; border-radius: 50px; padding: 15px; border: 3px solid #818cf8;");
    m_avatarLabel->setFixedSize(120, 120);
    m_avatarLabel->setAlignment(Qt::AlignCenter);

    QPushButton *changeAvatarBtn = new QPushButton("Change");
    changeAvatarBtn->setStyleSheet("QPushButton { background-color: #6366f1; color: white; border-radius: 8px; padding: 8px 16px; font-size: 12px; font-weight: bold; } QPushButton:hover { background-color: #4f46e5; }");
    changeAvatarBtn->setCursor(Qt::PointingHandCursor);
    connect(changeAvatarBtn, &QPushButton::clicked, [this]() {
        AvatarSelector *selector = new AvatarSelector(this);
        if (selector->exec() == QDialog::Accepted) {
            QString newAvatar = selector->selectedAvatar();
            DatabaseManager::instance().setAvatar(newAvatar);
            m_avatarLabel->setText(newAvatar);
            NotificationManager::instance().showNotification("Avatar Updated!", "Looking good!", "success");
        }
        delete selector;
    });

    QVBoxLayout *avatarLayout = new QVBoxLayout;
    avatarLayout->addWidget(m_avatarLabel, 0, Qt::AlignCenter);
    avatarLayout->addWidget(changeAvatarBtn, 0, Qt::AlignCenter);

    QVBoxLayout *info = new QVBoxLayout;
    QString username = DatabaseManager::instance().getCurrentUsername();
    QLabel *name = new QLabel(username.isEmpty() ? "User" : username);
    name->setStyleSheet(StyleHelper::getLabelStyle("header"));

    int xp     = DatabaseManager::instance().getXP();
    int streak = DatabaseManager::instance().getStreak();
    int bits   = DatabaseManager::instance().getBits();
    int hearts = DatabaseManager::instance().getHearts();
    QLabel *meta = new QLabel(QString("💎 %1 Bits  |  🔥 %2 Day Streak  |  ⭐ %3 XP  |  ❤️ %4 Hearts")
                              .arg(bits).arg(streak).arg(xp).arg(hearts));
    meta->setStyleSheet("font-size: 14px; color: #94a3b8;");

    info->addWidget(name);
    info->addWidget(meta);

    header->addLayout(avatarLayout);
    header->addSpacing(20);
    header->addLayout(info);
    header->addStretch();

    QWidget *headerWidget = new QWidget;
    headerWidget->setLayout(header);
    m_layout->addWidget(headerWidget);

    // Badges Grid
    QLabel *badgeTitle = new QLabel("Achievement Gallery");
    badgeTitle->setStyleSheet(StyleHelper::getLabelStyle("subheader"));
    m_layout->addWidget(badgeTitle);

    QGridLayout *badges = new QGridLayout;
    badges->addWidget(createBadge("Bug Hunter", true), 0, 0);
    badges->addWidget(createBadge("Syntax King", true), 0, 1);
    badges->addWidget(createBadge("7-Day Warrior", true), 0, 2);
    badges->addWidget(createBadge("Flash Coder", false), 1, 0);
    badges->addWidget(createBadge("Null Pointer", true), 1, 1);

    QWidget *badgesWidget = new QWidget;
    badgesWidget->setLayout(badges);
    m_layout->addWidget(badgesWidget);
    m_layout->addStretch();
}

QWidget* ProfileSection::createBadge(const QString& name, bool earned) {
    QFrame *f = new QFrame;
    f->setFixedSize(120, 120);
    QVBoxLayout *v = new QVBoxLayout(f);
    
    QLabel *icon = new QLabel(earned ? "🏆" : "🔒");
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size: 32px;");
    
    QLabel *lbl = new QLabel(name);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setWordWrap(true);
    
    if (!earned) {
        f->setStyleSheet("color: gray;");
    }
    
    v->addWidget(icon);
    v->addWidget(lbl);
    return f;
}
