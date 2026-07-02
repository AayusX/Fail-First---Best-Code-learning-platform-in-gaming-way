#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <functional>

class AnimationHelper {
public:
    // Fade in a widget nicely
    static void fadeIn(QWidget *widget, int duration = 500) {
        if (!widget) return;
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        
        QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(duration);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->setEasingCurve(QEasingCurve::OutQuad);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // Slide widget up (good for entering cards)
    static void slideInUp(QWidget *widget, int offset = 50, int duration = 600) {
        if (!widget) return;
        QRect end = widget->geometry();
        QRect start = end.adjusted(0, offset, 0, offset);
        
        widget->setGeometry(start);
        widget->setVisible(true);
        
        QPropertyAnimation *anim = new QPropertyAnimation(widget, "geometry");
        anim->setDuration(duration);
        anim->setStartValue(start);
        anim->setEndValue(end);
        anim->setEasingCurve(QEasingCurve::OutBack); // Bouncy effect
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        
        // Combine with fade for better look
        fadeIn(widget, duration);
    }
    
    // Shake effect for wrong answers
    static void shake(QWidget *widget) {
        if (!widget) return;
        QPropertyAnimation *anim = new QPropertyAnimation(widget, "pos");
        anim->setDuration(500);
        anim->setLoopCount(1);
        
        QPoint pos = widget->pos();
        anim->setKeyValueAt(0, pos);
        anim->setKeyValueAt(0.1, pos + QPoint(10, 0));
        anim->setKeyValueAt(0.2, pos + QPoint(-10, 0));
        anim->setKeyValueAt(0.3, pos + QPoint(10, 0));
        anim->setKeyValueAt(0.4, pos + QPoint(-10, 0));
        anim->setKeyValueAt(0.5, pos + QPoint(5, 0));
        anim->setKeyValueAt(0.6, pos + QPoint(-5, 0));
        anim->setKeyValueAt(1.0, pos);
        
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    // Pop effect for labels (e.g. +XP)
    static void pop(QWidget *widget) {
        if (!widget) return;
        QRect original = widget->geometry();
        QRect scaled = original.adjusted(-5, -5, 5, 5);
        
        QSequentialAnimationGroup *group = new QSequentialAnimationGroup(widget);
        
        QPropertyAnimation *grow = new QPropertyAnimation(widget, "geometry");
        grow->setDuration(100);
        grow->setStartValue(original);
        grow->setEndValue(scaled);
        
        QPropertyAnimation *shrink = new QPropertyAnimation(widget, "geometry");
        shrink->setDuration(100);
        shrink->setStartValue(scaled);
        shrink->setEndValue(original);
        
        group->addAnimation(grow);
        group->addAnimation(shrink);
        group->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    // Zoom In (Scale from 0 to 1)
    static void zoomIn(QWidget *widget, int duration = 500) {
        if (!widget) return;
        QRect finalGeo = widget->geometry();
        QRect startGeo = finalGeo;
        startGeo.setWidth(0);
        startGeo.setHeight(0);
        // Center the start rect
        startGeo.moveCenter(finalGeo.center());
        
        widget->setGeometry(startGeo);
        widget->setVisible(true);
        
        QPropertyAnimation *anim = new QPropertyAnimation(widget, "geometry");
        anim->setDuration(duration);
        anim->setStartValue(startGeo);
        anim->setEndValue(finalGeo);
        anim->setEasingCurve(QEasingCurve::OutBack);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    // Pulse Glow Effect (for important elements)
    static void pulseGlow(QWidget *widget, int duration = 1000) {
        if (!widget) return;
        
        QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect*>(widget->graphicsEffect());
        if (!effect) {
            effect = new QGraphicsOpacityEffect(widget);
            widget->setGraphicsEffect(effect);
        }
        
        QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(duration);
        anim->setLoopCount(-1);  // Infinite
        anim->setKeyValueAt(0.0, 1.0);
        anim->setKeyValueAt(0.5, 0.6);
        anim->setKeyValueAt(1.0, 1.0);
        anim->setEasingCurve(QEasingCurve::InOutSine);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    // Smooth Fade Out (for dismissing widgets)
    static void smoothFadeOut(QWidget *widget, int duration = 500, std::function<void()> onComplete = nullptr) {
        if (!widget) return;
        
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        
        QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(duration);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        anim->setEasingCurve(QEasingCurve::InCubic);
        
        if (onComplete) {
            QObject::connect(anim, &QPropertyAnimation::finished, onComplete);
        }
        
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    // Shimmer effect for loading states
    static void shimmer(QWidget *widget) {
        if (!widget) return;
        
        QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        
        QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
        anim->setDuration(1200);
        anim->setLoopCount(-1);
        anim->setKeyValueAt(0.0, 0.4);
        anim->setKeyValueAt(0.5, 1.0);
        anim->setKeyValueAt(1.0, 0.4);
        anim->setEasingCurve(QEasingCurve::InOutSine);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    // Bounce Effect (for success states)
    static void bounce(QWidget *widget) {
        if (!widget) return;
        
        QRect original = widget->geometry();
        QSequentialAnimationGroup *group = new QSequentialAnimationGroup(widget);
        
        // Jump up
        QPropertyAnimation *up = new QPropertyAnimation(widget, "geometry");
        up->setDuration(200);
        up->setStartValue(original);
        up->setEndValue(original.adjusted(0, -20, 0, -20));
        up->setEasingCurve(QEasingCurve::OutCubic);
        
        // Fall down with bounce
        QPropertyAnimation *down = new QPropertyAnimation(widget, "geometry");
        down->setDuration(400);
        down->setStartValue(original.adjusted(0, -20, 0, -20));
        down->setEndValue(original);
        down->setEasingCurve(QEasingCurve::OutBounce);
        
        group->addAnimation(up);
        group->addAnimation(down);
        group->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    // --- Creative: Particle Explosion (Confetti) ---
    static void confetti(QWidget *parent) {
        if (!parent) return;
        
        for (int i = 0; i < 30; i++) {
            QLabel *p = new QLabel(parent);
            p->resize(8, 8);
            
            // Random Color
            QStringList colors = {"#f472b6", "#fbbf24", "#60a5fa", "#a78bfa", "#34d399"}; // Pink, Gold, Blue, Purple, Green
            QString color = colors[rand() % colors.size()];
            p->setStyleSheet(QString("background-color: %1; border-radius: 4px;").arg(color));
            
            // Random Start Pos (Center-ish)
            int startX = parent->width() / 2 + (rand() % 40 - 20);
            int startY = parent->height() / 2 + (rand() % 40 - 20);
            p->move(startX, startY);
            p->show();
            
            // Random Trajectory
            int endX = startX + (rand() % 300 - 150);
            int endY = startY + (rand() % 300 - 150);
            
            QParallelAnimationGroup *group = new QParallelAnimationGroup(p);
            
            // Move
            QPropertyAnimation *move = new QPropertyAnimation(p, "pos");
            move->setDuration(800 + (rand() % 400));
            move->setStartValue(QPoint(startX, startY));
            move->setEndValue(QPoint(endX, endY));
            move->setEasingCurve(QEasingCurve::OutQuad);
            
            // Fade Out
            QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(p);
            p->setGraphicsEffect(eff);
            QPropertyAnimation *fade = new QPropertyAnimation(eff, "opacity");
            fade->setDuration(800);
            fade->setStartValue(1.0);
            fade->setEndValue(0.0);
            
            group->addAnimation(move);
            group->addAnimation(fade);
            
            // Cleanup particle after animation
            QObject::connect(group, &QAbstractAnimation::finished, [p](){
                delete p;
            });
            
            group->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
};

#endif // ANIMATIONHELPER_H
