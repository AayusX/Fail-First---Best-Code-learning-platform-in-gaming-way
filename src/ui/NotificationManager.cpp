#include "NotificationManager.h"
#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>

NotificationManager& NotificationManager::instance() {
    static NotificationManager instance;
    return instance;
}

void NotificationManager::setParentWidget(QWidget *parent) {
    m_parent = parent;
}

void NotificationManager::showNotification(const QString& title, const QString& message, const QString& type) {
    if (!m_parent) return;

    QWidget *popup = new QWidget(m_parent);
    popup->setStyleSheet("background-color: #ffffff; border-radius: 8px; border: 1px solid #cbd5e1;");
    if (type == "success") popup->setStyleSheet("background-color: #dcfce7; border: 1px solid #22c55e; border-radius: 8px;");
    if (type == "error") popup->setStyleSheet("background-color: #fee2e2; border: 1px solid #ef4444; border-radius: 8px;");

    QVBoxLayout *layout = new QVBoxLayout(popup);
    QLabel *titleLbl = new QLabel(title, popup);
    titleLbl->setStyleSheet("font-weight: bold; font-size: 14px; color: #1e293b; border: none;");
    QLabel *msgLbl = new QLabel(message, popup);
    msgLbl->setStyleSheet("font-size: 12px; color: #475569; border: none;");
    
    layout->addWidget(titleLbl);
    layout->addWidget(msgLbl);
    
    popup->resize(300, 80);
    // Position bottom right
    popup->move(m_parent->width() - 320, m_parent->height() - 100);
    popup->show();

    // Fade out
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(popup);
    popup->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
    a->setDuration(500);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutQuad);
    
    QTimer::singleShot(3000, [popup, a](){
        a->start(QAbstractAnimation::DeleteWhenStopped);
        connect(a, &QPropertyAnimation::finished, popup, &QWidget::deleteLater);
    });
}
