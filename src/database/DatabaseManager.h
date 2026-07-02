#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>
#include <QRandomGenerator>

struct Challenge {
    int id;
    QString title;
    QString language;
    QString difficulty;
    QString description;
    QString story_text; // New
    QString quiz_data;  // New
    QString buggyCode;
    QString solutionCode;
    QString tests; // JSON string
    QString hints; // JSON string
    int xp;
    bool solved = false;
};

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager& instance();
    bool init();

    QList<Challenge> getAllChallenges();
    Challenge getChallenge(int id);
    bool markSolved(int challengeId);
    
    // Auth
    bool login(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password);
    int currentUserId() const;
    QString getCurrentUsername();
    
    // Language
    void setLanguage(const QString& lang);
    QString getLanguage();

    // Avatar
    QString getAvatar();
    void setAvatar(const QString& avatar);
    
    // User Stats
    int getXP();
    void addXP(int amount);
    int getBits();
    void addBits(int amount);
    bool spendBits(int amount);
    int getStreak();
    void incrementStreak();
    int getHearts();
    void setHearts(int hearts);
    QString getLastActiveDate();
    void setLastActiveDate(const QString& date);
    bool hasItem(const QString& itemId);
    void addItem(const QString& itemId);

signals:
    void userChanged();

private:
    DatabaseManager();
    QSqlDatabase m_db;
    int m_currentUserId;
    void createTables();
    void loadInitialContent();
    void loadChallengesFromFile(const QString& filePath);
    
    // Password security
    QString hashPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& storedHash);
    QByteArray generateSalt();
};

#endif // DATABASEMANAGER_H
