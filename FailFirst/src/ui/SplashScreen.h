#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class SplashScreen : public QWidget {
    Q_OBJECT
    
public:
    explicit SplashScreen(QWidget *parent = nullptr);
    
signals:
    void finished();
    
private:
    void setupUi();
    void startAnimations();
    
    QLabel *m_titleLabel;
    QLabel *m_subtitleLabel;
    QLabel *m_loadingLabel;
    QTimer *m_timer;
    QGraphicsOpacityEffect *m_opacityEffect;
    QPropertyAnimation *m_fadeAnimation;
    
    int m_animationStep;
};

#endif // SPLASHSCREEN_H
