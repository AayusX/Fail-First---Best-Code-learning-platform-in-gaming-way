#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QWidget>

class NotificationManager : public QObject {
    Q_OBJECT
public:
    static NotificationManager& instance();
    void setParentWidget(QWidget *parent);
    void showNotification(const QString& title, const QString& message, const QString& type = "info");

private:
    NotificationManager() {}
    QWidget *m_parent = nullptr;
};

#endif // NOTIFICATIONMANAGER_H
