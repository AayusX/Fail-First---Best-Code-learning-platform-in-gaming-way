#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccess();

private slots:
    void onSignInClicked();
    void onSignUpClicked();

private:
    QLineEdit *m_userEdit;
    QLineEdit *m_passEdit;
    QLabel *m_statusLbl;
};

#endif // LOGINWINDOW_H
