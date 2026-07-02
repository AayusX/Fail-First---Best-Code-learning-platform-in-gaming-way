#include "DatabaseManager.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QThread>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() : m_currentUserId(-1) {}

bool DatabaseManager::init() {
    // PORTABLE DATABASE: Store next to .exe for true portability
    QString appDir = QCoreApplication::applicationDirPath();
    QString dataDir = appDir + "/FailFirstData";
    
    // Create directory if it doesn't exist
    QDir dir;
    if (!dir.mkpath(dataDir)) {
        qWarning() << "Failed to create data directory:" << dataDir;
        qWarning() << "Please check write permissions for:" << appDir;
        return false;
    }
    
    QString dbPath = dataDir + "/failfirst.db";
    qDebug() << "Database location:" << dbPath;
    
    // Remove old connection if it exists (for hot-reload scenarios)
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    }
    
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
    
    // Try to open database with retry
    int retries = 3;
    while (retries > 0) {
        if (m_db.open()) {
            break;
        }
        retries--;
        qWarning() << "Database open attempt failed. Retries left:" << retries;
        QThread::msleep(100);
    }
    
    if (!m_db.isOpen()) {
        qWarning() << "❌ Failed to open database after retries!";
        qWarning() << "Error:" << m_db.lastError().text();
        qWarning() << "Path:" << dbPath;
        return false;
    }
    
    qDebug() << "✅ Database opened successfully";

    
    // Performance optimizations — bound to m_db to avoid silent failures
    QSqlQuery pragma(m_db);
    pragma.exec("PRAGMA synchronous = NORMAL");
    pragma.exec("PRAGMA journal_mode = WAL");
    pragma.exec("PRAGMA cache_size = 10000");
    pragma.exec("PRAGMA foreign_keys = ON");
    
    createTables();
    loadInitialContent();
    return true;
}

void DatabaseManager::createTables() {
    QSqlQuery query(m_db);
    // Users table with NOT NULL on critical fields, hearts for persistence, last_active for streak
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT NOT NULL UNIQUE, "
               "password TEXT NOT NULL, "
               "avatar TEXT DEFAULT '\xf0\x9f\x91\xa8\xe2\x80\x8d\xf0\x9f\x92\xbb', "
               "xp INTEGER DEFAULT 0, "
               "bits INTEGER DEFAULT 0, "
               "hearts INTEGER DEFAULT 5, "
               "streak INTEGER DEFAULT 0, "
               "current_language TEXT, "
               "last_active_date TEXT DEFAULT '')");

    // Migrate: add hearts/last_active if upgrading from old schema
    query.exec("ALTER TABLE users ADD COLUMN hearts INTEGER DEFAULT 5");
    query.exec("ALTER TABLE users ADD COLUMN last_active_date TEXT DEFAULT ''");
    // ^^ These will silently fail if columns already exist, which is fine.

    // Challenges table
    query.exec("CREATE TABLE IF NOT EXISTS challenges ("
               "id INTEGER PRIMARY KEY, "
               "title TEXT NOT NULL, "
               "type TEXT DEFAULT 'lesson_unit', "
               "language TEXT NOT NULL, "
               "difficulty TEXT, "
               "description TEXT, "
               "story_text TEXT, "
               "quiz_data TEXT, "
               "buggyCode TEXT, "
               "solutionCode TEXT, "
               "tests TEXT, "
               "hints TEXT, "
               "xp INTEGER DEFAULT 0)");

    query.exec("CREATE TABLE IF NOT EXISTS solved (user_id INTEGER NOT NULL, challenge_id INTEGER NOT NULL, PRIMARY KEY(user_id, challenge_id))");
    query.exec("CREATE TABLE IF NOT EXISTS items (user_id INTEGER NOT NULL, item_id TEXT NOT NULL, PRIMARY KEY(user_id, item_id))");
}

bool DatabaseManager::login(const QString& username, const QString& password) {
    if (username.trimmed().isEmpty()) {
        qWarning() << "⚠️ Login failed: Empty username";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("SELECT id, password, xp, bits, streak FROM users WHERE username = ?");
    query.addBindValue(username.trimmed());
    
    if (!query.exec()) {
        qWarning() << "❌ Login query failed:" << query.lastError().text();
        qWarning() << "Username:" << username;
        return false;
    }
    
    if (query.next()) {
        QString storedHash = query.value("password").toString();
        
        // Verify password hash
        if (verifyPassword(password, storedHash)) {
            m_currentUserId = query.value("id").toInt();
            emit userChanged();
            return true;
        }
    }
    return false;
}

bool DatabaseManager::registerUser(const QString& username, const QString& password) {
    // Hash password before storing
    QString hashedPassword = hashPassword(password);
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, xp, bits, streak) VALUES (?, ?, 0, 100, 0)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    
    if (query.exec()) {
        m_currentUserId = query.lastInsertId().toInt();
        emit userChanged();
        return true;
    }
    return false;
}

int DatabaseManager::currentUserId() const {
    return m_currentUserId;
}

QString DatabaseManager::getCurrentUsername() {
    if (m_currentUserId == -1) return "";
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE id = ?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toString();
    return "";
}

void DatabaseManager::setLanguage(const QString& lang) {
    if (m_currentUserId == -1) return;
    QSqlQuery query;
    query.prepare("UPDATE users SET current_language = ? WHERE id = ?");
    query.addBindValue(lang);
    query.addBindValue(m_currentUserId);
    query.exec();
}

QString DatabaseManager::getLanguage() {
    if (m_currentUserId == -1) return "";
    QSqlQuery query;
    query.prepare("SELECT current_language FROM users WHERE id = ?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toString();
    return "";
}

QString DatabaseManager::getAvatar() {
    if (m_currentUserId == -1) return "👨‍💻";
    QSqlQuery query;
    query.prepare("SELECT avatar FROM users WHERE id = ?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toString();
    return "👨‍💻";
}

void DatabaseManager::setAvatar(const QString& avatar) {
    if (m_currentUserId == -1) return;
    QSqlQuery query;
    query.prepare("UPDATE users SET avatar = ? WHERE id = ?");
    query.addBindValue(avatar);
    query.addBindValue(m_currentUserId);
    query.exec();
}

// ... Rest of loadInitialContent ...

void DatabaseManager::loadInitialContent() {
    QSqlQuery checkQuery;
    checkQuery.exec("SELECT count(*) FROM challenges");
    if (checkQuery.next() && checkQuery.value(0).toInt() > 0) return; // Already loaded

    // Load C++ challenges
    loadChallengesFromFile(":/resources/challenges.json");
    
    // Load C challenges
    loadChallengesFromFile(":/resources/challenges_c.json");
    
    // Load Java challenges
    loadChallengesFromFile(":/resources/challenges_java.json");
}

void DatabaseManager::loadChallengesFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning() << "Invalid JSON format in" << filePath;
        return;
    }
    
    QJsonArray array = doc.array();

    m_db.transaction();
    QSqlQuery query;
    query.prepare("INSERT INTO challenges (id, title, language, difficulty, description, story_text, quiz_data, buggyCode, solutionCode, tests, hints, xp) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    for (const auto& val : array) {
        QJsonObject obj = val.toObject();
        query.addBindValue(obj["id"].toInt());
        query.addBindValue(obj["title"].toString());
        query.addBindValue(obj["language"].toString());
        query.addBindValue(obj["difficulty"].toString());
        query.addBindValue(obj["description"].toString());
        query.addBindValue(obj["story_text"].toString());
        query.addBindValue(obj["quiz_data"].toString());
        query.addBindValue(obj["buggy_code"].toString());
        query.addBindValue(obj["solution_code"].toString());
        query.addBindValue(QJsonDocument(obj["tests"].toArray()).toJson(QJsonDocument::Compact));
        query.addBindValue(QJsonDocument(obj["hints"].toArray()).toJson(QJsonDocument::Compact));
        query.addBindValue(obj["xp"].toInt());
        
        if (!query.exec()) {
            qWarning() << "Failed to insert challenge" << obj["id"].toInt() << ":" << query.lastError().text();
        }
    }
    m_db.commit();
    
    qDebug() << "Loaded" << array.size() << "challenges from" << filePath;
}

QList<Challenge> DatabaseManager::getAllChallenges() {
    QList<Challenge> list;
    QString lang = getLanguage();

    // Single LEFT JOIN query — avoids N+1 problem
    QSqlQuery query(m_db);
    if (!lang.isEmpty()) {
        query.prepare(
            "SELECT c.*, CASE WHEN s.challenge_id IS NOT NULL THEN 1 ELSE 0 END as is_solved "
            "FROM challenges c "
            "LEFT JOIN solved s ON s.challenge_id = c.id AND s.user_id = ? "
            "WHERE c.language = ? "
            "ORDER BY c.id ASC"
        );
        query.addBindValue(m_currentUserId);
        query.addBindValue(lang);
    } else {
        query.prepare(
            "SELECT c.*, CASE WHEN s.challenge_id IS NOT NULL THEN 1 ELSE 0 END as is_solved "
            "FROM challenges c "
            "LEFT JOIN solved s ON s.challenge_id = c.id AND s.user_id = ? "
            "ORDER BY c.id ASC"
        );
        query.addBindValue(m_currentUserId);
    }

    if (!query.exec()) {
        qWarning() << "getAllChallenges query failed:" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        Challenge c;
        c.id          = query.value("id").toInt();
        c.title       = query.value("title").toString();
        c.language    = query.value("language").toString();
        c.difficulty  = query.value("difficulty").toString();
        c.description = query.value("description").toString();
        c.story_text  = query.value("story_text").toString();
        c.quiz_data   = query.value("quiz_data").toString();
        c.buggyCode   = query.value("buggyCode").toString();
        c.solutionCode= query.value("solutionCode").toString();
        c.tests       = query.value("tests").toString();
        c.hints       = query.value("hints").toString();
        c.xp          = query.value("xp").toInt();
        c.solved      = query.value("is_solved").toInt() > 0;
        list.append(c);
    }

    qDebug() << "Loaded" << list.size() << "challenges for language:" << lang;
    return list;
}


Challenge DatabaseManager::getChallenge(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM challenges WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qWarning() << "getChallenge query failed:" << query.lastError().text();
        return Challenge();
    }
    
    if (query.next()) {
        Challenge c;
        c.id = query.value("id").toInt();
        c.title = query.value("title").toString();
        c.language = query.value("language").toString();
        c.difficulty = query.value("difficulty").toString();
        c.description = query.value("description").toString();
        
        // CRITICAL FIX: Add missing fields!
        c.story_text = query.value("story_text").toString();
        c.quiz_data = query.value("quiz_data").toString();
        
        c.buggyCode = query.value("buggyCode").toString();
        c.solutionCode = query.value("solutionCode").toString();
        c.tests = query.value("tests").toString();
        c.hints = query.value("hints").toString();
        c.xp = query.value("xp").toInt();
        
        qDebug() << "Loaded challenge:" << c.title << "with story_text length:" << c.story_text.length();
        return c;
    }
    return Challenge();
}

bool DatabaseManager::markSolved(int challengeId) {
    if (m_currentUserId == -1) return false;
    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO solved (user_id, challenge_id) VALUES (?, ?)");
    query.addBindValue(m_currentUserId);
    query.addBindValue(challengeId);
    return query.exec();
}

int DatabaseManager::getXP() {
    if (m_currentUserId == -1) return 0;
    QSqlQuery query;
    query.prepare("SELECT xp FROM users WHERE id=?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toInt();
    return 0;
}

void DatabaseManager::addXP(int amount) {
    if (m_currentUserId == -1) return;
    QSqlQuery query;
    query.prepare("UPDATE users SET xp = xp + ? WHERE id=?");
    query.addBindValue(amount);
    query.addBindValue(m_currentUserId);
    query.exec();
}

int DatabaseManager::getBits() {
    if (m_currentUserId == -1) return 0;
    QSqlQuery query;
    query.prepare("SELECT bits FROM users WHERE id=?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toInt();
    return 0;
}

void DatabaseManager::addBits(int amount) {
    if (m_currentUserId == -1) return;
    QSqlQuery query;
    query.prepare("UPDATE users SET bits = bits + ? WHERE id=?");
    query.addBindValue(amount);
    query.addBindValue(m_currentUserId);
    query.exec();
}

bool DatabaseManager::spendBits(int amount) {
    if (getBits() < amount) return false;
    QSqlQuery query;
    query.prepare("UPDATE users SET bits = bits - ? WHERE id=?");
    query.addBindValue(amount);
    query.addBindValue(m_currentUserId);
    return query.exec();
}

int DatabaseManager::getStreak() {
    if (m_currentUserId == -1) return 0;
    QSqlQuery query;
    query.prepare("SELECT streak FROM users WHERE id=?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toInt();
    return 0;
}

void DatabaseManager::incrementStreak() {
    if (m_currentUserId == -1) return;
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET streak = streak + 1 WHERE id=?");
    query.addBindValue(m_currentUserId);
    query.exec();
}

int DatabaseManager::getHearts() {
    if (m_currentUserId == -1) return 5;
    QSqlQuery query(m_db);
    query.prepare("SELECT hearts FROM users WHERE id=?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toInt();
    return 5;
}

void DatabaseManager::setHearts(int h) {
    if (m_currentUserId == -1) return;
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET hearts = ? WHERE id=?");
    query.addBindValue(qMax(0, h));
    query.addBindValue(m_currentUserId);
    query.exec();
}

QString DatabaseManager::getLastActiveDate() {
    if (m_currentUserId == -1) return QString();
    QSqlQuery query(m_db);
    query.prepare("SELECT last_active_date FROM users WHERE id=?");
    query.addBindValue(m_currentUserId);
    query.exec();
    if (query.next()) return query.value(0).toString();
    return QString();
}

void DatabaseManager::setLastActiveDate(const QString &date) {
    if (m_currentUserId == -1) return;
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET last_active_date = ? WHERE id=?");
    query.addBindValue(date);
    query.addBindValue(m_currentUserId);
    query.exec();
}

bool DatabaseManager::hasItem(const QString& itemId) {
    if (m_currentUserId == -1) return false;
    QSqlQuery query;
    query.prepare("SELECT count(*) FROM items WHERE item_id = ? AND user_id = ?");
    query.addBindValue(itemId);
    query.addBindValue(m_currentUserId);
    query.exec();
    return query.next() && query.value(0).toInt() > 0;
}

void DatabaseManager::addItem(const QString& itemId) {
    if (m_currentUserId == -1) return;
    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO items (user_id, item_id) VALUES (?, ?)");
    query.addBindValue(m_currentUserId);
    query.addBindValue(itemId);
    query.exec();
}

// --- Password Security Functions ---

QString DatabaseManager::hashPassword(const QString& password) {
    // Generate random salt (16 bytes)
    QByteArray salt = generateSalt();
    
    // Hash password with salt using SHA-256
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(salt);
    hash.addData(password.toUtf8());
    
    QByteArray hashedPassword = hash.result();
    
    // Store as: salt$hash (both base64 encoded)
    return salt.toBase64() + "$" + hashedPassword.toBase64();
}

bool DatabaseManager::verifyPassword(const QString& password, const QString& storedHash) {
    // Split stored hash into salt and hash
    QStringList parts = storedHash.split("$");
    if (parts.size() != 2) {
        qWarning() << "Invalid password hash format";
        return false;
    }
    
    QByteArray salt = QByteArray::fromBase64(parts[0].toUtf8());
    QByteArray storedPasswordHash = QByteArray::fromBase64(parts[1].toUtf8());
    
    // Hash the provided password with the stored salt
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(salt);
    hash.addData(password.toUtf8());
    
    QByteArray computedHash = hash.result();
    
    // Constant-time comparison to prevent timing attacks
    return computedHash == storedPasswordHash;
}

QByteArray DatabaseManager::generateSalt() {
    QByteArray salt;
    salt.resize(16);
    
    // Use QRandomGenerator for cryptographically secure random bytes (Qt 5.10+)
    for (int i = 0; i < 16; ++i) {
        salt[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    }
    
    return salt;
}
