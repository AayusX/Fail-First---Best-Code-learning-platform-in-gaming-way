#include "EconomyManager.h"
#include <QTimer>

EconomyManager& EconomyManager::instance() {
    static EconomyManager instance;
    return instance;
}

EconomyManager::EconomyManager() {
    // Hearts are now persisted in the database.
}

int EconomyManager::bits() const {
    return DatabaseManager::instance().getBits();
}

int EconomyManager::hearts() const {
    return DatabaseManager::instance().getHearts();
}

QString EconomyManager::nextHeartTime() const {
    return "25:00"; // Placeholder
}

void EconomyManager::earnBits(int amount) {
    DatabaseManager::instance().addBits(amount);
    emit bitsChanged();
}

bool EconomyManager::spendBits(int amount) {
    if (DatabaseManager::instance().spendBits(amount)) {
        emit bitsChanged();
        return true;
    }
    emit purchaseFailed("Not enough bits!");
    return false;
}

void EconomyManager::loseHeart() {
    int current = DatabaseManager::instance().getHearts();
    if (current > 0) {
        DatabaseManager::instance().setHearts(current - 1);
        emit heartsChanged();
    }
}

void EconomyManager::refillHearts() {
    DatabaseManager::instance().setHearts(5);
    emit heartsChanged();
}

bool EconomyManager::purchaseItem(const QString& itemId, int cost) {
    if (DatabaseManager::instance().hasItem(itemId)) {
        emit purchaseFailed("Already owned!");
        return false;
    }
    if (spendBits(cost)) {
        DatabaseManager::instance().addItem(itemId);
        emit purchaseSuccess(itemId);
        if (itemId == "health_potion") refillHearts();
        return true;
    }
    return false;
}
