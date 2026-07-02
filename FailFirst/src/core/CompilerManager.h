#ifndef COMPILERMANAGER_H
#define COMPILERMANAGER_H

#include <QString>
#include <QMap>

/**
 * @brief Manages portable and system compilers for code validation.
 * 
 * This class provides a unified interface for finding compilers that works
 * across different systems. On Windows, it supports embedded portable compilers
 * (TCC) stored alongside the .exe. Falls back to system compilers on all platforms.
 */
class CompilerManager {
public:
    static CompilerManager& instance();
    
    /**
     * @brief Get the path to a compiler for the specified language.
     * @param language "cpp", "c", "python", or "java"
     * @return Full path to compiler executable, or empty if not available
     */
    QString getCompilerPath(const QString& language);
    
    /**
     * @brief Get additional compiler arguments needed for the language.
     * @param language "cpp", "c", "python", or "java"
     * @return List of additional arguments (e.g., -std=c++17)
     */
    QStringList getCompilerArgs(const QString& language);
    
    /**
     * @brief Check if a compiler is available for the specified language.
     * @param language "cpp", "c", "python", or "java"
     * @return true if compiler is available
     */
    bool isCompilerAvailable(const QString& language);
    
    /**
     * @brief Get a user-friendly message when compiler is not available.
     * @param language "cpp", "c", "python", or "java"
     * @return Message explaining how to enable the feature
     */
    QString getUnavailableMessage(const QString& language);
    
    /**
     * @brief Get the runtime executable for interpreted/compiled languages.
     * @param language "java" -> "java", "python" -> "python3"
     * @return Path to runtime executable
     */
    QString getRuntimePath(const QString& language);
    
    /**
     * @brief Check if runtime is available (for Java execution after compile).
     */
    bool isRuntimeAvailable(const QString& language);
    
private:
    CompilerManager();
    ~CompilerManager() = default;
    
    void detectCompilers();
    QString findPortableCompiler(const QString& name);
    QString findSystemCompiler(const QString& name);
    bool testCompiler(const QString& path);
    bool extractEmbeddedCompiler();
    
    QMap<QString, QString> m_compilerPaths;  // language -> path
    QMap<QString, QString> m_runtimePaths;   // language -> runtime path
    QString m_portableDir;                    // Path to portable/ folder
    bool m_initialized = false;
};

#endif // COMPILERMANAGER_H
