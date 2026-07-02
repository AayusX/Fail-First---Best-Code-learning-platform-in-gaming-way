#include "CompilerManager.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>

CompilerManager& CompilerManager::instance() {
    static CompilerManager instance;
    return instance;
}

CompilerManager::CompilerManager() {
#ifdef Q_OS_WIN
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
#endif
    // Set portable directory to a persistent cache location
    m_portableDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/portable";
    QDir().mkpath(m_portableDir);
    
    detectCompilers();
}

void CompilerManager::detectCompilers() {
    if (m_initialized) return;
    
    qDebug() << "CompilerManager: Detecting available compilers...";
    
    // First, try to extract embedded compiler if portable directory doesn't have TCC
    if (!QFile::exists(m_portableDir + "/tcc/tcc.exe")) {
        extractEmbeddedCompiler();
    }
    
    qDebug() << "Portable directory:" << m_portableDir;
    
    // --- C++ Compiler ---
    // Priority: System g++ > System clang++
    // NOTE: TCC is strictly a C compiler. Do NOT use it for C++.
    
    QString cppCompiler = findSystemCompiler("g++");
    
    // Fallback: Check common Qt/MinGW paths if not in PATH
    if (cppCompiler.isEmpty()) {
       QStringList knowPaths = {
           "C:/Qt/Tools/mingw1310_64/bin/g++.exe",
           "C:/Qt/Tools/mingw1120_64/bin/g++.exe",
           "C:/MinGW/bin/g++.exe"
       };
       for(const QString& p : knowPaths) {
           if(QFile::exists(p)) {
               cppCompiler = p;
               break;
           }
       }
    }

    if (cppCompiler.isEmpty()) {
        cppCompiler = findSystemCompiler("clang++");
    }
    
    if (!cppCompiler.isEmpty()) {
        m_compilerPaths["cpp"] = cppCompiler;
        qDebug() << "C++ compiler found:" << cppCompiler;
    }
    
    // --- C Compiler ---
    // Priority: Portable TCC > System gcc > System clang
    QString cCompiler = findPortableCompiler("tcc");
    if (cCompiler.isEmpty()) {
        cCompiler = findSystemCompiler("gcc");
    }
    if (cCompiler.isEmpty()) {
        cCompiler = findSystemCompiler("clang");
    }
    if (!cCompiler.isEmpty()) {
        m_compilerPaths["c"] = cCompiler;
        qDebug() << "C compiler found:" << cCompiler;
    }
    
    // --- Python ---
    // Priority: Portable > System
    QString portablePython = findPortableCompiler("python/python.exe"); // Windows portable
    if (portablePython.isEmpty()) portablePython = findPortableCompiler("python/bin/python3"); // Linux portable structure
    
    QString pythonPath = portablePython; 
    if (pythonPath.isEmpty()) pythonPath = findSystemCompiler("python3");
    if (pythonPath.isEmpty()) pythonPath = findSystemCompiler("python");
    
    if (!pythonPath.isEmpty()) {
        m_compilerPaths["python"] = pythonPath;
        m_runtimePaths["python"] = pythonPath;
        qDebug() << "Python found:" << pythonPath;
    }
    
    // --- Java ---
    // Priority: Portable > System
    QString portableJavac = findPortableCompiler("java/bin/javac.exe");
    QString portableJava = findPortableCompiler("java/bin/java.exe");
    
    QString javacPath = !portableJavac.isEmpty() ? portableJavac : findSystemCompiler("javac");
    QString javaPath = !portableJava.isEmpty() ? portableJava : findSystemCompiler("java");
    
    if (!javacPath.isEmpty() && !javaPath.isEmpty()) {
        m_compilerPaths["java"] = javacPath;
        m_runtimePaths["java"] = javaPath;
        qDebug() << "Java found:" << javacPath;
    }
    
    m_initialized = true;
    qDebug() << "CompilerManager: Detection complete. Available:" << m_compilerPaths.keys();
}

QString CompilerManager::findPortableCompiler(const QString& name) {
    // Check for portable compiler in app directory
    QStringList possiblePaths;
    
#ifdef Q_OS_WIN
    // Windows paths
    possiblePaths << m_portableDir + "/tcc/tcc.exe"
                  << m_portableDir + "/tcc-win64/tcc.exe"
                  << m_portableDir + "/" + name + ".exe"
                  << m_portableDir + "/mingw/bin/g++.exe"
                  << m_portableDir + "/mingw/bin/gcc.exe";
#else
    // Linux/Mac paths
    possiblePaths << m_portableDir + "/tcc/tcc"
                  << m_portableDir + "/bin/" + name
                  << m_portableDir + "/" + name;
#endif
    
    for (const QString& path : possiblePaths) {
        if (path.contains(name, Qt::CaseInsensitive)) {
            QFileInfo info(path);
            if (info.exists() && info.isExecutable()) {
                return path;
            }
        }
    }
    
    return QString();
}

QString CompilerManager::findSystemCompiler(const QString& name) {
    // Use QProcess to find compiler in system PATH
    QProcess which;
    
#ifdef Q_OS_WIN
    which.start("where", {name});
#else
    which.start("which", {name});
#endif
    
    if (!which.waitForFinished(2000)) {
        return QString();
    }
    
    if (which.exitCode() != 0) {
        return QString();
    }
    
    QString path = QString::fromUtf8(which.readAllStandardOutput()).trimmed();
    
    // Handle multiple results (take first line)
    if (path.contains('\n')) {
        path = path.split('\n').first().trimmed();
    }
    
    // Verify the compiler works
    if (!path.isEmpty() && testCompiler(path)) {
        return path;
    }
    
    return QString();
}

bool CompilerManager::testCompiler(const QString& path) {
    // Basic version check first
    QProcess test;
    test.start(path, {"--version"});
    if (!test.waitForFinished(2000)) return false;
    if (test.exitCode() != 0) return false;

    // Advanced check: try to compile and run a tiny program
    // This catches DLL entry point issues that --version might miss
    QString tempDir = QDir::tempPath();
    QString sourceFile = tempDir + "/test_compiler.c";
    QString exeFile = tempDir + "/test_compiler.exe";

    QFile file(sourceFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write("int main() { return 0; }");
        file.close();
    } else {
        return true; // Fallback to basic check if file IO fails
    }

    QProcess compile;
    QStringList args;
    if (path.contains("tcc", Qt::CaseInsensitive)) {
        args << "-run" << sourceFile;
    } else {
        args << sourceFile << "-o" << exeFile;
    }

    compile.start(path, args);
    bool finished = compile.waitForFinished(5000);
    
    // Cleanup
    QFile::remove(sourceFile);
    if (QFile::exists(exeFile)) QFile::remove(exeFile);

    return finished && compile.exitCode() == 0;
}

QString CompilerManager::getCompilerPath(const QString& language) {
    if (!m_initialized) {
        detectCompilers();
    }
    return m_compilerPaths.value(language);
}

QStringList CompilerManager::getCompilerArgs(const QString& language) {
    QStringList args;
    
    QString compiler = m_compilerPaths.value(language);
    bool isTcc = compiler.contains("tcc", Qt::CaseInsensitive);
    
    if (language == "cpp") {
        if (isTcc) {
            // TCC doesn't support full C++17, use C mode for basic code
            args << "-run";  // TCC can run directly without separate compile step
        } else {
            args << "-std=c++17" << "-w"; // Suppress warnings to avoid noise
        }
    } else if (language == "c") {
        if (isTcc) {
            args << "-run";
        } else {
            args << "-std=c11" << "-w";
        }
    }
    
    return args;
}

bool CompilerManager::isCompilerAvailable(const QString& language) {
    if (!m_initialized) {
        detectCompilers();
    }
    return m_compilerPaths.contains(language) && !m_compilerPaths[language].isEmpty();
}

QString CompilerManager::getUnavailableMessage(const QString& language) {
    if (language == "cpp" || language == "c") {
        return QString(
            "⚠️ C/C++ Compiler Not Found\n\n"
            "To run code challenges, please install a compiler:\n\n"
            "• Windows: Install MinGW-w64 from https://winlibs.com\n"
            "• Linux: Run 'sudo apt install g++' or equivalent\n"
            "• macOS: Run 'xcode-select --install'\n\n"
            "Or place TCC in the 'portable/' folder next to this app."
        );
    } else if (language == "python") {
        return QString(
            "⚠️ Python Not Found\n\n"
            "To run Python challenges, please install Python 3:\n\n"
            "• Windows: Download from https://python.org\n"
            "• Linux: Run 'sudo apt install python3'\n"
            "• macOS: Run 'brew install python3'"
        );
    } else if (language == "java") {
        return QString(
            "⚠️ Java Not Found\n\n"
            "To run Java challenges, please install JDK 11+:\n\n"
            "• Download from https://adoptium.net\n"
            "• Or run 'sudo apt install openjdk-17-jdk' on Linux"
        );
    }
    
    return "Unsupported language: " + language;
}

QString CompilerManager::getRuntimePath(const QString& language) {
    if (!m_initialized) {
        detectCompilers();
    }
    return m_runtimePaths.value(language);
}

bool CompilerManager::isRuntimeAvailable(const QString& language) {
    if (!m_initialized) {
        detectCompilers();
    }
    return m_runtimePaths.contains(language) && !m_runtimePaths[language].isEmpty();
}

bool CompilerManager::extractEmbeddedCompiler() {
    qDebug() << "CompilerManager: Extracting embedded TCC...";
    
    QString zipInRes = ":/resources/tcc.zip";
    if (!QFile::exists(zipInRes)) {
        qDebug() << "CompilerManager: Resource tcc.zip not found!";
        return false;
    }
    
    QString cachePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(cachePath);
    QString zipOut = cachePath + "/tcc_embedded.zip";
    
    // Copy zip from resource to disk (PowerShell needs a real file)
    if (QFile::exists(zipOut)) QFile::remove(zipOut);
    if (!QFile::copy(zipInRes, zipOut)) {
        qDebug() << "CompilerManager: Failed to copy tcc.zip from resources";
        return false;
    }
    
#ifdef Q_OS_WIN
    // Use PowerShell to extract for zero-dependency portability
    QProcess extract;
    QString targetDir = m_portableDir + "/tcc";
    QDir().mkpath(targetDir);
    
    QString cmd = QString("Expand-Archive -Path '%1' -DestinationPath '%2' -Force")
                    .arg(QDir::toNativeSeparators(zipOut))
                    .arg(QDir::toNativeSeparators(targetDir));
    
    extract.start("powershell", {"-Command", cmd});
    if (!extract.waitForFinished(30000)) {
        qDebug() << "CompilerManager: TCC extraction timed out";
        return false;
    }
#endif

    QFile::remove(zipOut);
    return QFile::exists(m_portableDir + "/tcc/tcc.exe");
}
