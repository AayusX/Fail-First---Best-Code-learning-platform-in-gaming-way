#include "LoginWindow.h"
#include "StyleHelper.h"
#include "AnimationHelper.h"
#include "database/DatabaseManager.h"
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QScrollArea>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Fail{First} - Login");
    
    // Darker gradient background for better contrast
    setStyleSheet("QWidget#LoginWindow { "
                  "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
                  "    stop:0 #2d3748, stop:1 #1a202c); "  // Dark blue-gray
                  "}");
    setObjectName("LoginWindow"); // For styling to target correctly
    
    // Main Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // No margins for scroll area context
    
    // Scroll Area for responsiveness
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;"); // Let gradient show through
    
    // Content Widget inside ScrollArea
    QWidget *contentWidget = new QWidget;
    contentWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setAlignment(Qt::AlignCenter);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    
    // White card with strong contrast
    QFrame *card = new QFrame;
    card->setFixedWidth(420); // Fixed width for consistency
    card->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    card->setStyleSheet(
        "QFrame { "
        "  background-color: #ffffff; "
        "  border: 2px solid #cbd5e0; "
        "  border-radius: 20px; "
        "}"
    );
    
    // Enhanced shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(50);
    shadow->setColor(QColor(0,0,0,80));
    shadow->setOffset(0, 15);
    card->setGraphicsEffect(shadow);
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(18);
    layout->setContentsMargins(35, 40, 35, 40);
    
    // Title with strong dark color
    QLabel *title = new QLabel("Fail First");
    title->setStyleSheet(
        "font-family: 'Inter'; "
        "font-size: 38px; "
        "font-weight: 900; "
        "color: #1a202c; "  // Very dark
        "letter-spacing: -1px;"
        "background: transparent;"
    );
    title->setAlignment(Qt::AlignCenter);
    
    // Subtitle with colored background for visibility
    QLabel *subtitle = new QLabel("Master Programming\nThrough Debugging");
    subtitle->setWordWrap(true);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(
        "font-family: 'Inter'; "
        "font-size: 14px; "
        "color: #ffffff; "  // White text
        "background-color: #4299e1; "  // Blue background
        "border-radius: 10px; "
        "padding: 12px; "
        "line-height: 1.4;"
    );
    
    // Username Label - strong color
    QLabel *userLbl = new QLabel("Username");
    userLbl->setStyleSheet(
        "font-family: 'Inter'; "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #2d3748; "  // Dark gray
        "margin-bottom: 4px;"
        "background: transparent;"
    );
    
    m_userEdit = new QLineEdit;
    m_userEdit->setPlaceholderText("Enter your username");
    m_userEdit->setFixedHeight(50);  // Fixed height to prevent overflow

    // Password label & field
    QLabel *passLbl = new QLabel("Password");
    passLbl->setStyleSheet(
        "font-family: 'Inter'; "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #2d3748; "
        "margin-bottom: 4px;"
        "background: transparent;"
    );
    m_passEdit = new QLineEdit;
    m_passEdit->setPlaceholderText("Enter your password");
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setFixedHeight(50);
    m_passEdit->setStyleSheet(
        "QLineEdit { "
        "  padding: 12px 18px; "
        "  border: 2px solid #cbd5e0; "
        "  border-radius: 10px; "
        "  font-size: 15px; "
        "  background: #f7fafc; "
        "  color: #1a202c; "
        "  font-weight: 500; "
        "} "
        "QLineEdit:focus { "
        "  border-color: #4299e1; "
        "  background: #ffffff; "
        "}"
    );
    m_userEdit->setStyleSheet(
        "QLineEdit { "
        "  padding: 12px 18px; "
        "  border: 2px solid #cbd5e0; "
        "  border-radius: 10px; "
        "  font-size: 15px; "
        "  background: #f7fafc; "  // Light gray
        "  color: #1a202c; "  // Dark text
        "  font-weight: 500; "
        "} "
        "QLineEdit:focus { "
        "  border-color: #4299e1; "  // Blue border on focus
        "  background: #ffffff; "
        "}"
    );
    
    // Vibrant primary button
    QPushButton *startBtn = new QPushButton("Start Debugging");
    startBtn->setFixedHeight(50);  // Fixed height
    startBtn->setStyleSheet(
        "QPushButton { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "  // Purple gradient
        "  color: #ffffff; "  // White text
        "  border: none; "
        "  border-radius: 10px; "
        "  padding: 12px 24px; "
        "  font-size: 15px; "
        "  font-weight: 700; "
        "} "
        "QPushButton:hover { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #5568d3, stop:1 #6a3d91); "
        "} "
        "QPushButton:pressed { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #4c5ed0, stop:1 #5a3380); "
        "}"
    );
    startBtn->setCursor(Qt::PointingHandCursor);
    connect(startBtn, &QPushButton::clicked, this, &LoginWindow::onSignInClicked);
    
    // Ghost Create Button with visible border
    QPushButton *createBtn = new QPushButton("Create Account");
    createBtn->setFixedHeight(48);  // Slightly smaller than primary
    createBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  color: #667eea; "  // Purple text
        "  border: 2px solid #667eea; "
        "  border-radius: 10px; "
        "  padding: 10px 24px; "
        "  font-size: 15px; "
        "  font-weight: 600; "
        "} "
        "QPushButton:hover { "
        "  background-color: #edf2f7; "
        "  color: #5568d3; "
        "  border-color: #5568d3; "
        "}"
    );
    createBtn->setCursor(Qt::PointingHandCursor);
    connect(createBtn, &QPushButton::clicked, this, &LoginWindow::onSignUpClicked);
    
    m_statusLbl = new QLabel("");
    m_statusLbl->setWordWrap(true);  // Allow wrapping for long messages
    m_statusLbl->setMaximumHeight(60);  // Limit height
    m_statusLbl->setStyleSheet(
        "color: #e53e3e; "  // Red for errors (will be changed dynamically)
        "font-size: 13px; "
        "font-weight: 600;"
        "background: transparent;"
    );
    m_statusLbl->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(10);
    layout->addWidget(userLbl);
    layout->addWidget(m_userEdit);
    layout->addWidget(passLbl);
    layout->addWidget(m_passEdit);
    layout->addWidget(m_statusLbl);
    layout->addSpacing(6);
    layout->addWidget(startBtn);
    layout->addWidget(createBtn);
    layout->addStretch();
    
    contentLayout->addWidget(card);
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    
    // Fade in the entire window on show
    QTimer::singleShot(100, this, [this]() {
        AnimationHelper::fadeIn(this, 600);
    });
}

void LoginWindow::onSignInClicked() {
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text();

    if (user.isEmpty()) {
        m_statusLbl->setText("⚠️ Please enter a username.");
        m_statusLbl->setStyleSheet("color: #e53e3e; font-size: 14px; font-weight: 600;");
        AnimationHelper::shake(m_userEdit);
        return;
    }
    if (pass.isEmpty()) {
        m_statusLbl->setText("⚠️ Please enter your password.");
        m_statusLbl->setStyleSheet("color: #e53e3e; font-size: 14px; font-weight: 600;");
        AnimationHelper::shake(m_passEdit);
        return;
    }

    // Show loading state
    m_statusLbl->setText("🔄 Signing in...");
    m_statusLbl->setStyleSheet("color: #4299e1; font-size: 14px; font-weight: 600;");  // Blue

    QTimer::singleShot(300, this, [this, user, pass]() {
        if (DatabaseManager::instance().login(user, pass)) {
            m_statusLbl->setText("✅ Success!");
            m_statusLbl->setStyleSheet("color: #48bb78; font-size: 14px; font-weight: 600;");  // Green
            emit loginSuccess();
        } else {
            m_statusLbl->setText("❌ Wrong username or password.");
            m_statusLbl->setStyleSheet("color: #e53e3e; font-size: 14px; font-weight: 600;");  // Red
            AnimationHelper::shake(m_userEdit);
        }
    });
}

void LoginWindow::onSignUpClicked() {
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text();

    if (user.isEmpty()) {
        m_statusLbl->setText("⚠️ Enter a username to create account.");
        m_statusLbl->setStyleSheet("color: #e53e3e; font-size: 14px; font-weight: 600;");
        AnimationHelper::shake(m_userEdit);
        return;
    }
    if (pass.length() < 4) {
        m_statusLbl->setText("⚠️ Password must be at least 4 characters.");
        m_statusLbl->setStyleSheet("color: #e53e3e; font-size: 14px; font-weight: 600;");
        AnimationHelper::shake(m_passEdit);
        return;
    }

    m_statusLbl->setText("🔄 Creating account...");
    m_statusLbl->setStyleSheet("color: #4299e1; font-size: 14px; font-weight: 600;");  // Blue

    QTimer::singleShot(300, this, [this, user, pass]() {
        if (DatabaseManager::instance().registerUser(user, pass)) {
            m_statusLbl->setText("✅ Account created! Please sign in.");
            m_statusLbl->setStyleSheet("color: #48bb78; font-size: 14px; font-weight: 600;");  // Green
            // Clear fields for re-login
            m_userEdit->clear();
            m_passEdit->clear();
        } else {
            m_statusLbl->setText("❌ Username already taken.");
            m_statusLbl->setStyleSheet("color: #e53e3e; font-size: 14px; font-weight: 600;");  // Red
            AnimationHelper::shake(m_userEdit);
        }
    });
}
