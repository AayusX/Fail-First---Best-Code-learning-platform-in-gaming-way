#ifndef LANGUAGE_SELECTION_H
#define LANGUAGE_SELECTION_H

#include <QWidget>
#include <QPushButton>

class LanguageSelection : public QWidget {
    Q_OBJECT
public:
    explicit LanguageSelection(QWidget *parent = nullptr);

signals:
    void languageSelected(QString lang);

private:
    QWidget* createCard(const QString& name, const QString& code, const QString& desc, int progress, const QString& color);
};

#endif // LANGUAGE_SELECTION_H
