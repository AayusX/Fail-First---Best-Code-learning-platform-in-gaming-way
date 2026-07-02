#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QObject>
#include <QProcess>
#include <QTemporaryFile>

struct ProcessResult {
    QString output;
    int exitCode = 0;
};

struct ValidationResult {
    bool passed;
    QString output;
    QString error;
};

class Validator : public QObject {
    Q_OBJECT
public:
    explicit Validator(QObject *parent = nullptr);
    ValidationResult validate(const QString& language, const QString& code, const QString& input = "", const QString& solutionCode = "", const QString& expectedOutput = "");

private:
    ProcessResult runProcess(const QString& program, const QStringList& args, const QString& input, int timeoutMs = 5000);
};

#endif // VALIDATOR_H
