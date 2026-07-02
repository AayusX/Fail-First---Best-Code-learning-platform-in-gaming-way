#include "GamificationEngine.h"
#include <QDate>

GamificationEngine& GamificationEngine::instance() {
    static GamificationEngine instance;
    return instance;
}

GamificationEngine::GamificationEngine() {}

int GamificationEngine::currentXP() {
    return DatabaseManager::instance().getXP();
}

int GamificationEngine::currentLevel() {
    return calculateLevel(currentXP());
}

int GamificationEngine::currentStreak() {
    return DatabaseManager::instance().getStreak();
}

void GamificationEngine::addXP(int amount) {
    int oldLevel = currentLevel();
    DatabaseManager::instance().addXP(amount);
    emit xpChanged(currentXP());
    
    int newLevel = currentLevel();
    if (newLevel > oldLevel) {
        emit levelChanged(newLevel);
        emit levelUp(newLevel);
    }
}

void GamificationEngine::completeDailyGoal() {
    // Only increment streak once per calendar day
    QString today = QDate::currentDate().toString(Qt::ISODate);
    QString lastActive = DatabaseManager::instance().getLastActiveDate();
    if (lastActive != today) {
        DatabaseManager::instance().incrementStreak();
        DatabaseManager::instance().setLastActiveDate(today);
        emit streakChanged(currentStreak());
    }
}

double GamificationEngine::getLevelProgress() {
    int xp = currentXP();
    int leveXPStart = (currentLevel() - 1) * 1000; // Simplified logic
    int nextLevelXP = currentLevel() * 1000;
    return (double)(xp - leveXPStart) / (nextLevelXP - leveXPStart);
}

int GamificationEngine::calculateLevel(int xp) {
    return (xp / 1000) + 1; // Simple linear scaling for demo
}
