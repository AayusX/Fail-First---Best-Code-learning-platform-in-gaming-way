#ifndef ECONOMYMANAGER_H
#define ECONOMYMANAGER_H

#include <QObject>
#include "database/DatabaseManager.h"

class EconomyManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(int bits READ bits NOTIFY bitsChanged)
    Q_PROPERTY(int hearts READ hearts NOTIFY heartsChanged)
    Q_PROPERTY(QString nextHeartTime READ nextHeartTime NOTIFY heartsChanged)

public:
    static EconomyManager& instance();
    
    int bits() const;
    int hearts() const;
    QString nextHeartTime() const;
    
    Q_INVOKABLE void earnBits(int amount);
    Q_INVOKABLE bool spendBits(int amount);
    Q_INVOKABLE void loseHeart();
    Q_INVOKABLE void refillHearts();
    
    Q_INVOKABLE bool purchaseItem(const QString& itemId, int cost);

signals:
    void bitsChanged();
    void heartsChanged();
    void purchaseSuccess(QString itemId);
    void purchaseFailed(QString reason);

private:
    EconomyManager();
};

#endif // ECONOMYMANAGER_H
