#include "SplashScreen.h"
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QLinearGradient>

SplashScreen::SplashScreen(QWidget *parent) 
    : QWidget(parent), m_animationStep(0) {
    
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(800, 600);
    
    setupUi();
    startAnimations();
}

void SplashScreen::setupUi() {
    // Vibrant gradient background: Pink → Mint → Sage
    setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #e6c6d1, stop:0.5 #cbe0dc, stop:1 #e8e7cf); "
        "  border-radius: 24px; "
        "}"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(60, 0, 60, 60);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(24);
    
    // Main Title - "Welcome Programmers"
    m_titleLabel = new QLabel("Welcome Programmers", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "font-family: 'Inter', sans-serif; "
        "font-size: 56px; "
        "font-weight: 900; "
        "color: #6d8b85; "  // Deep teal
        "letter-spacing: -2px; "
        "background: transparent; "
        "padding: 20px;"
    );
    
    // Add shadow effect to title
    QGraphicsDropShadowEffect *titleShadow = new QGraphicsDropShadowEffect;
    titleShadow->setBlurRadius(40);
    titleShadow->setColor(QColor(109, 139, 133, 100));
    titleShadow->setOffset(0, 10);
    m_titleLabel->setGraphicsEffect(titleShadow);
    
    // Subtitle
    m_subtitleLabel = new QLabel("Master Debugging | Fix Bugs | Level Up", this);
    m_subtitleLabel->setAlignment(Qt::AlignCenter);
    m_subtitleLabel->setStyleSheet(
        "font-family: 'Inter', sans-serif; "
        "font-size: 22px; "
        "font-weight: 600; "
        "color: #87aca4; "  // Medium teal
        "background: transparent; "
        "letter-spacing: 1px;"
    );
    
    // Loading indicator
    m_loadingLabel = new QLabel("●●●", this);
    m_loadingLabel->setAlignment(Qt::AlignCenter);
    m_loadingLabel->setStyleSheet(
        "font-family: 'Inter', sans-serif; "
        "font-size: 32px; "
        "color: #c88ca0; "  // Pink
        "background: transparent; "
        "letter-spacing: 8px;"
    );
    
    layout->addStretch();
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_subtitleLabel);
    layout->addSpacing(40);
    layout->addWidget(m_loadingLabel);
    layout->addStretch();
    
    // Setup opacity effect for fade out
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(1.0);
}

void SplashScreen::startAnimations() {
    // Fade in first
    QPropertyAnimation *fadeIn = new QPropertyAnimation(m_opacityEffect, "opacity");
    fadeIn->setDuration(600);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutCubic);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    
    // Pulse animation for title
    QPropertyAnimation *titlePulse = new QPropertyAnimation(m_titleLabel, "geometry");
    titlePulse->setDuration(1500);
    titlePulse->setLoopCount(-1);  // Infinite loop
    QRect titleRect = m_titleLabel->geometry();
    QRect titlePulsed = titleRect.adjusted(-5, -5, 5, 5);
    titlePulse->setKeyValueAt(0.0, titleRect);
    titlePulse->setKeyValueAt(0.5, titlePulsed);
    titlePulse->setKeyValueAt(1.0, titleRect);
    titlePulse->setEasingCurve(QEasingCurve::InOutSine);
    titlePulse->start(QAbstractAnimation::DeleteWhenStopped);
    
    // Animated loading dots
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_animationStep = (m_animationStep + 1) % 4;
        QString dots;
        for (int i = 0; i < 3; i++) {
            if (i < m_animationStep) {
                dots += "●";
            } else {
                dots += "○";
            }
        }
        m_loadingLabel->setText(dots);
    });
    m_timer->start(300);
    
    // Auto-dismiss after 3 seconds
    QTimer::singleShot(3000, this, [this]() {
        m_timer->stop();
        
        // Fade out animation
        m_fadeAnimation = new QPropertyAnimation(m_opacityEffect, "opacity");
        m_fadeAnimation->setDuration(800);
        m_fadeAnimation->setStartValue(1.0);
        m_fadeAnimation->setEndValue(0.0);
        m_fadeAnimation->setEasingCurve(QEasingCurve::InCubic);
        
        connect(m_fadeAnimation, &QPropertyAnimation::finished, this, [this]() {
            emit finished();
            close();
        });
        
        m_fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
