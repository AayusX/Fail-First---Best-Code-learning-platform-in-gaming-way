#ifndef GAMIFICATIONENGINE_H
#define GAMIFICATIONENGINE_H

#include <QObject>
#include "database/DatabaseManager.h"

class GamificationEngine : public QObject {
    Q_OBJECT
    Q_PROPERTY(int currentXP READ currentXP NOTIFY xpChanged)
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY levelChanged)
    Q_PROPERTY(int currentStreak READ currentStreak NOTIFY streakChanged)

public:
    static GamificationEngine& instance();
    
    int currentXP();
    int currentLevel();
    int currentStreak();
    
    void addXP(int amount);
    void completeDailyGoal();
    double getLevelProgress(); // 0.0 to 1.0

signals:
    void xpChanged(int newXP);
    void levelChanged(int newLevel);
    void streakChanged(int newStreak);
    void levelUp(int newLevel);

private:
    GamificationEngine();
    int calculateLevel(int xp);
};

#endif // GAMIFICATIONENGINE_H
