#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QPropertyAnimation>

class LoginWindow;
class LanguageSelection;

class Dashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onNavClicked(QListWidgetItem *item);
    void updateStats();
    
    // Auth Flow
    void onLoginSuccess();
    void onLanguageSelected(QString lang);
    void showLanguageSelection();
    void showMainInterface();

private:
    void setupUi();
    void createSidebar();
    
    // Containers
    QStackedWidget *m_rootStack; // Top level stack: Login -> LangSel -> MainApp
    QWidget *m_mainAppWidget;    // The actual dashboard interface (Sidebar + Content)
    
    LoginWindow *m_loginWindow;
    LanguageSelection *m_langSelect;

    // Main App Components
    QWidget *m_sidebar;
    QListWidget *m_navStats;
    QStackedWidget *m_contentStack; // Content stack: Learn, Shop, etc.
    
    // Header Stats
    QLabel *m_bitsLabel;
    QLabel *m_streakLabel;
    QLabel *m_heartsLabel;
    QLabel *m_xpLabel;
    
    // Sections
    QWidget *m_learnSection;
    QWidget *m_challengeSection;
    QWidget *m_shopSection;
    QWidget *m_leaderboardSection;
    QWidget *m_profileSection;
    
    void loadSections();
};

#endif // DASHBOARD_H
