#include "Dashboard.h"
#include "StyleHelper.h"
#include "LearnSection.h"
#include "ChallengeSection.h"
#include "SimpleSections.h"
#include "LoginWindow.h"
#include "LanguageSelection.h"
#include "core/EconomyManager.h"
#include "core/GamificationEngine.h"
#include "NotificationManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include "AnimationHelper.h"

Dashboard::Dashboard(QWidget *parent) : QMainWindow(parent) {
    NotificationManager::instance().setParentWidget(this);
    
    // Root Stack Setup
    m_rootStack = new QStackedWidget(this);
    setCentralWidget(m_rootStack);
    
    // 1. Login Screen
    m_loginWindow = new LoginWindow(this);
    m_rootStack->addWidget(m_loginWindow);
    connect(m_loginWindow, &LoginWindow::loginSuccess, this, &Dashboard::onLoginSuccess);
    
    // 2. Language Selection
    m_langSelect = new LanguageSelection(this);
    m_rootStack->addWidget(m_langSelect);
    connect(m_langSelect, &LanguageSelection::languageSelected, this, &Dashboard::onLanguageSelected);
    // 3. Main App (Placeholder for now, initialized on demand or here)
    m_mainAppWidget = new QWidget;
    setupUi(); // This now builds into m_mainAppWidget
    m_rootStack->addWidget(m_mainAppWidget);
    
    resize(1200, 800);
    setStyleSheet(StyleHelper::getMainWindowStyle());
    
    // Start at Login
    m_rootStack->setCurrentWidget(m_loginWindow);
    
    // Connect stats signals
    connect(&EconomyManager::instance(), &EconomyManager::bitsChanged, this, &Dashboard::updateStats);
    connect(&EconomyManager::instance(), &EconomyManager::heartsChanged, this, &Dashboard::updateStats);
    connect(&GamificationEngine::instance(), &GamificationEngine::xpChanged, this, &Dashboard::updateStats);
}

void Dashboard::onLoginSuccess() {
    NotificationManager::instance().showNotification("Welcome Back!", "Let's fix some bugs.", "success");
    showLanguageSelection();
}

void Dashboard::showLanguageSelection() {
    m_rootStack->setCurrentWidget(m_langSelect);
}

void Dashboard::onLanguageSelected(QString lang) {
    // Refresh content based on language?
    // LearnSection needs to know language changed.
    // For now, re-instantiate or refresh.
    loadSections(); // Reload content
    updateStats();
    showMainInterface();
    
    // Fade in the main interface
    AnimationHelper::fadeIn(m_mainAppWidget, 800);
}

void Dashboard::showMainInterface() {
    m_rootStack->setCurrentWidget(m_mainAppWidget);
}

void Dashboard::setupUi() {
    // Builds the Main App Interface into m_mainAppWidget
    
    QHBoxLayout *mainLayout = new QHBoxLayout(m_mainAppWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    createSidebar();
    mainLayout->addWidget(m_sidebar);
    
    QWidget *rightContainer = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    
    // White/Light Gray Header for clean look
    QFrame *headerFrame = new QFrame;
    headerFrame->setStyleSheet(
        "QFrame { "
        "  background-color: #ffffff; "
        "  border-bottom: 1px solid #e2e8f0; "
        "}"
    );
    headerFrame->setFixedHeight(80);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(30, 0, 30, 0);
    
    headerLayout->addStretch();
    
    // High Contrast Stat Labels
    auto createStatLabel = [](const QString &text, const QString &color) {
        QLabel *lbl = new QLabel(text);
        lbl->setStyleSheet(
            QString("color: %1; "
                   "font-weight: 700; "
                   "font-size: 15px; "
                   "background-color: #f7fafc; " // Very light gray
                   "border: 1px solid #edf2f7; "
                   "border-radius: 12px; "
                   "padding: 8px 16px; "
                   "margin-right: 12px;").arg(color)
        );
        return lbl;
    };
    
    // Use darker, richer colors for stats
    m_bitsLabel = createStatLabel("💎 0", "#3182ce");  // Blue
    headerLayout->addWidget(m_bitsLabel);

    m_streakLabel = createStatLabel("🔥 0", "#d69e2e"); // Orange/Gold
    headerLayout->addWidget(m_streakLabel);

    m_heartsLabel = createStatLabel("❤️ 5", "#e53e3e"); // Red
    headerLayout->addWidget(m_heartsLabel);
    
    m_xpLabel = createStatLabel("XP: 0 / Lvl 1", "#805ad5"); // Purple
    headerLayout->addWidget(m_xpLabel);
    
    rightLayout->addWidget(headerFrame);
    
    m_contentStack = new QStackedWidget;
    // loadSections called later
    rightLayout->addWidget(m_contentStack);
    
    mainLayout->addWidget(rightContainer);
}

void Dashboard::createSidebar() {
    m_sidebar = new QWidget;
    m_sidebar->setFixedWidth(280);
    m_sidebar->setStyleSheet(
        "QWidget { "
        "  background-color: #ffffff; " // White sidebar
        "  border-right: 1px solid #e2e8f0; "
        "}"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(m_sidebar);
    layout->setContentsMargins(20, 30, 20, 20);
    
    // Dark Logo Text
    QLabel *logo = new QLabel("Fail{First}");
    logo->setStyleSheet(
        "font-size: 28px; "
        "font-weight: 900; "
        "color: #1a202c; " // Dark text
        "padding: 12px; "
        "letter-spacing: -1px;"
    );
    layout->addWidget(logo);
    layout->addSpacing(20);
    
    m_navStats = new QListWidget;
    m_navStats->setStyleSheet(StyleHelper::getSidebarStyle());
    
    m_navStats->addItem(new QListWidgetItem("📘 Learning Path"));
    m_navStats->addItem(new QListWidgetItem("🏆 Leaderboard"));
    m_navStats->addItem(new QListWidgetItem("🛒 Marketplace"));
    m_navStats->addItem(new QListWidgetItem("👤 Profile"));
    
    connect(m_navStats, &QListWidget::itemClicked, this, &Dashboard::onNavClicked);
    
    layout->addWidget(m_navStats);
    
    // Animate Sidebar Entry
    AnimationHelper::slideInUp(m_sidebar, -50, 800);
}

void Dashboard::loadSections() {
    // Clear old
    while(m_contentStack->count() > 0) {
        QWidget *w = m_contentStack->widget(0);
        m_contentStack->removeWidget(w);
        w->deleteLater();
    }

    // 0: Learn
    LearnSection *learn = new LearnSection(this);
    m_learnSection = learn;
    m_contentStack->addWidget(learn);
    connect(learn, &LearnSection::startChallenge, [this](int id){
        // TODO: check type logic (Story vs Challenge)
        ChallengeSection *chall = qobject_cast<ChallengeSection*>(m_challengeSection);
        if(chall) {
            chall->loadChallenge(id);
            m_contentStack->setCurrentWidget(m_challengeSection);
        }
    });

    // 1: Leaderboard
    m_leaderboardSection = new LeaderboardSection(this);
    m_contentStack->addWidget(m_leaderboardSection);
    
    // 2: Shop
    m_shopSection = new ShopSection(this);
    m_contentStack->addWidget(m_shopSection);
    
    // 3: Profile
    m_profileSection = new ProfileSection(this);
    m_contentStack->addWidget(m_profileSection);
    
    // 4: Challenge
    ChallengeSection *challenge = new ChallengeSection(this);
    m_challengeSection = challenge;
    m_contentStack->addWidget(challenge);
    connect(challenge, &ChallengeSection::backToLearn, [this](){
        m_contentStack->setCurrentWidget(m_learnSection);
        static_cast<LearnSection*>(m_learnSection)->refresh();
    });
}

void Dashboard::onNavClicked(QListWidgetItem *item) {
    if(!m_learnSection) return;

    QString txt = item->text();
    if (txt.contains("Learning")) {
        m_contentStack->setCurrentWidget(m_learnSection);
    } else if (txt.contains("Leaderboard")) {
        m_contentStack->setCurrentWidget(m_leaderboardSection);
        static_cast<LeaderboardSection*>(m_leaderboardSection)->refresh();
    } else if (txt.contains("Marketplace")) {
        m_contentStack->setCurrentWidget(m_shopSection);
    } else if (txt.contains("Profile")) {
        m_contentStack->setCurrentWidget(m_profileSection);
        static_cast<ProfileSection*>(m_profileSection)->refresh();
    }
}

void Dashboard::updateStats() {
    EconomyManager &eco = EconomyManager::instance();
    m_bitsLabel->setText(QString("💎 %1").arg(eco.bits()));
    m_heartsLabel->setText(QString("❤️ %1").arg(eco.hearts()));
    m_streakLabel->setText(QString("🔥 %1").arg(GamificationEngine::instance().currentStreak()));
    m_xpLabel->setText(QString("XP: %1 / Lvl %2").arg(GamificationEngine::instance().currentXP()).arg(GamificationEngine::instance().currentLevel()));
    
    // Use bounce instead of pop for more energy
    AnimationHelper::bounce(m_bitsLabel);
    AnimationHelper::bounce(m_heartsLabel);
    AnimationHelper::bounce(m_streakLabel);
    AnimationHelper::bounce(m_xpLabel);
}

void Dashboard::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}
