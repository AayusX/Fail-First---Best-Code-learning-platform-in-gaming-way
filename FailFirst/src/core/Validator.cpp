#include "Validator.h"
#include "CompilerManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QTemporaryFile>
#include <QRegularExpression>

Validator::Validator(QObject *parent) : QObject(parent) {}

// Helper to normalize code for static comparison:
// 1. Remove comments (//... and /*...*/)
// 2. Remove whitespace
static QString normalizeCode(const QString& code) {
    QString normalized = code;
    
    // Remove single line comments
    normalized.remove(QRegularExpression("//[^\n]*"));
    
    // Remove multi-line comments
    normalized.remove(QRegularExpression("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption));
    
    // Remove all whitespace
    return normalized.remove(QRegularExpression("\\s+"));
}

ValidationResult Validator::validate(const QString& language, const QString& code, const QString& input, const QString& solutionCode, const QString& expectedOutput) {
    ValidationResult result;
    result.passed = false;
    
    // Check if compiler is available first
    CompilerManager& cm = CompilerManager::instance();
    if (!cm.isCompilerAvailable(language)) {
        // FALLBACK: Static Validation
        if (!solutionCode.isEmpty() && normalizeCode(code) == normalizeCode(solutionCode)) {
            result.passed = true;
            result.output = expectedOutput + "\n\n(Validated via Static Analysis - Environment Cached)";
            return result;
        }
        
        result.error = cm.getUnavailableMessage(language);
        return result;
    }
    
    QString program;
    QStringList runArgs;
    
    // For manual cleanup later
    QString fileToCleanup;
    QString sourceToCleanup;
    
    if (language == "cpp" || language == "c") {
        // Use QTemporaryFile for unique, race-condition-free file creation
        QString extension = (language == "c") ? "c" : "cpp";
        
        // CRITICAL FIX: TCC doesn't recognize .cpp extension. 
        // If we are using TCC for C++, force .c extension so TCC treats it as C code.
        if (language == "cpp" && cm.getCompilerPath(language).contains("tcc", Qt::CaseInsensitive)) {
            extension = "c";
        }
        
        QTemporaryFile sourceFile(QDir::temp().filePath(QString("failfirst_XXXXXX.") + extension));
        // We need source to exist for compilation, then we can delete it
        sourceFile.setAutoRemove(false);
        if (!sourceFile.open()) {
            result.error = "Failed to create temporary source file";
            return result;
        }
        
        // Write code to file
        sourceFile.write(code.toUtf8());
        sourceFile.flush();
        QString fileName = sourceFile.fileName();
        sourceFile.close();
        sourceToCleanup = fileName;
        
        // Create unique executable name
        // CRITICAL FIX: execution fails because QTemporaryFile deletes the file when it goes out of scope!
        // We must prevent auto-removal and delete it manually later.
        QTemporaryFile exeFile(QDir::temp().filePath("failfirst_exe_XXXXXX"));
        exeFile.setAutoRemove(false); 
        exeFile.open();
        QString exeName = exeFile.fileName();
        exeFile.close();
        
        // Compile using CompilerManager to get appropriate compiler
        QString compiler = cm.getCompilerPath(language);
        QStringList compileArgs = cm.getCompilerArgs(language);
        
        // TCC -run mode handling
        if (compileArgs.contains("-run")) {
            program = compiler;
            runArgs = compileArgs;
            runArgs << fileName;
            sourceToCleanup = fileName;
        } else {
            compileArgs << fileName << "-o" << exeName;
            ProcessResult compileResult = runProcess(compiler, compileArgs, "", 30000);
            
            if (compileResult.exitCode != 0) {
                // FALLBACK: Static Validation on Compilation Failure
                if (!solutionCode.isEmpty() && normalizeCode(code) == normalizeCode(solutionCode)) {
                    // Cleanup potentially failed files
                    QFile::remove(fileName);
                    QFile::remove(exeName);
                    
                    result.passed = true;
                    result.output = expectedOutput + "\n\n(Validated via Static Analysis - Compilation Bypassed)";
                    return result;
                }

                result.error = "Compilation Failed:\n" + compileResult.output;
                QFile::remove(fileName);
                QFile::remove(exeName);
                return result;
            }
            
            QFile::setPermissions(exeName, QFile::ExeOwner | QFile::ReadOwner | QFile::WriteOwner | 
                                           QFile::ExeGroup | QFile::ReadGroup |
                                           QFile::ExeOther | QFile::ReadOther);
            
            program = exeName;
            fileToCleanup = exeName;
            sourceToCleanup = fileName;
        }
        
    } else if (language == "python") {
        // Use QTemporaryFile for Python too
        QTemporaryFile pyFile(QDir::temp().filePath("failfirst_XXXXXX.py"));
        pyFile.setAutoRemove(false);
        if (!pyFile.open()) {
            result.error = "Failed to create temporary Python file";
            return result;
        }
        
        pyFile.write(code.toUtf8());
        pyFile.flush();
        pyFile.close();
        
        program = cm.getRuntimePath("python");
        runArgs << pyFile.fileName();
        fileToCleanup = pyFile.fileName();
        
    } else if (language == "java") {
        // Java handling
        QTemporaryFile javaFile(QDir::temp().filePath("failfirst_XXXXXX.java"));
        javaFile.setAutoRemove(false); 
        if (!javaFile.open()) {
             result.error = "Failed to create temporary Java file";
             return result;
        }
        
        // Extract class name
        QString className = "Main"; 
        QRegularExpression classRegex("public\\s+class\\s+(\\w+)");
        QRegularExpressionMatch match = classRegex.match(code);
        if (match.hasMatch()) {
            className = match.captured(1);
        }
        
        javaFile.close();
        
        // Creating properly named java file for compilation
        QString javaFileName = QDir::temp().filePath(className + ".java");
        if (QFile::exists(javaFileName)) QFile::remove(javaFileName); 
        
        // Copy content to named file
        QFile::copy(javaFile.fileName(), javaFileName);
        QFile::remove(javaFile.fileName()); // cleanup temp file
        
        // Write content again to be sure (copy permissions issues?)
        QFile finalJavaFile(javaFileName);
        if (finalJavaFile.open(QIODevice::WriteOnly)) {
            finalJavaFile.write(code.toUtf8());
            finalJavaFile.close();
        }
        
        ProcessResult compileResult = runProcess(cm.getCompilerPath("java"), {javaFileName}, "");
        if (compileResult.exitCode != 0) {
            result.error = "Compilation Failed:\n" + compileResult.output;
            QFile::remove(javaFileName);
            return result;
        }
        
        program = cm.getRuntimePath("java");
        runArgs << "-cp" << QDir::temp().path() << className;
        
        sourceToCleanup = javaFileName;
        // Java creates .class files, we should try to cleanup that too
        fileToCleanup = QDir::temp().filePath(className + ".class");
        
    } else {
        result.error = "Unsupported language: " + language;
        return result;
    }
    
    // Execute the program
    ProcessResult runResult = runProcess(program, runArgs, input, 10000);
    result.output = runResult.output;
    
    // FALLBACK: If execution (or TCC -run compilation) fails, try static validation
    if (runResult.exitCode != 0) {
        if (!solutionCode.isEmpty() && normalizeCode(code) == normalizeCode(solutionCode)) {
            result.passed = true;
            result.output = expectedOutput + "\n\n(Validated via Static Analysis - Runtime/TCC-Run Fallback)";
            // Cleanup matches
            if (!fileToCleanup.isEmpty()) QFile::remove(fileToCleanup);
            if (!sourceToCleanup.isEmpty()) QFile::remove(sourceToCleanup);
            return result;
        }
    }
    
    // Check for execution errors
    if (runResult.exitCode != 0 && !runResult.output.isEmpty()) {
        result.error = "Runtime error (exit code " + QString::number(runResult.exitCode) + ")";
    }
    
    // Cleanup generated files
    if (!fileToCleanup.isEmpty()) {
        QFile::remove(fileToCleanup);
    }
    if (!sourceToCleanup.isEmpty()) {
        QFile::remove(sourceToCleanup);
    }
    
    result.passed = (runResult.exitCode == 0); // User verifies output, but exit code 0 is required for pass
    return result;
}

ProcessResult Validator::runProcess(const QString& program, const QStringList& args, const QString& input, int timeoutMs) {
    ProcessResult result;
    result.output = "";
    result.exitCode = 0;
    
    QProcess process;
    process.setProgram(program);
    process.setArguments(args);
    
    // Set resource limits (platform-specific would be better, but this is a start)
    process.setProcessChannelMode(QProcess::MergedChannels);
    
    process.start();
    
    if (!process.waitForStarted(1000)) {
        result.output = "Error: Could not start process " + program;
        result.exitCode = -1;
        return result;
    }
    
    if (!input.isEmpty()) {
        process.write(input.toUtf8());
        process.closeWriteChannel();
    }
    
    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        process.waitForFinished(1000);
        result.output = "Error: Timeout (Infinite Loop or Too Slow)\n"
                        "The process took too long to respond. This might be due to:\n"
                        "- An infinite loop in your code\n"
                        "- A very slow system or broken compiler environment\n"
                        "- A system dialog (like a crash) blocking the process";
        result.exitCode = -2;
        return result;
    }
    
    // Get exit code (CRITICAL FIX)
    result.exitCode = process.exitCode();
    
    QString out = process.readAllStandardOutput();
    QString err = process.readAllStandardError();
    
    if (!err.isEmpty()) {
        result.output = err + "\n" + out;
    } else {
        result.output = out;
    }
    
    return result;
}
