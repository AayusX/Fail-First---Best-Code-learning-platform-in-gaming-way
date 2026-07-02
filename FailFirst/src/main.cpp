#include <QApplication>
#include <QFontDatabase>
#include <QScreen>
#include <QMessageBox>
#include "ui/Dashboard.h"
#include "ui/SplashScreen.h"
#include "database/DatabaseManager.h"
#include "core/EconomyManager.h"
#include <QDebug>

void loadFonts() {
    // Attempt to load embedded fonts if available, otherwise rely on system
    // In a real scenario: QFontDatabase::addApplicationFont(":/resources/fonts/Inter-Regular.ttf");
}

int main(int argc, char *argv[]) {
    // Enable High DPI Scaling BEFORE creating QApplication
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication app(argc, argv);
    
    app.setOrganizationName("FailFirstEdu");
    app.setApplicationName("FailFirst");
    
    // Set application icon (shows in taskbar, window title, .exe file)
    app.setWindowIcon(QIcon(":/resources/icons/app_icon.png"));
    
    qDebug() << "Starting FailFirst...";
    
    // Init Database
    if (!DatabaseManager::instance().init()) {
        QMessageBox::critical(nullptr, "FailFirst - Startup Error",
            "Failed to initialize the database.\n\n"
            "Please check that the application has write permissions to its directory.\n"
            "Path: " + QCoreApplication::applicationDirPath());
        return 1;
    }
    
    // Init Singletons
    EconomyManager::instance();
    
    qDebug() << "Creating Dashboard and Splash...";
    
    // Create Dashboard (but don't show yet)
    Dashboard *dashboard = new Dashboard;
    
    // Show Splash Screen first with "Welcome Programmers"
    SplashScreen *splash = new SplashScreen;
    
    // Center splash screen on screen
    QRect screenGeometry = app.primaryScreen()->geometry();
    int x = (screenGeometry.width() - splash->width()) / 2;
    int y = (screenGeometry.height() - splash->height()) / 2;
    splash->move(x, y);
    splash->show();
    
    qDebug() << "Splash shown, waiting for finish signal...";
    
    // When splash finishes, show Dashboard
    QObject::connect(splash, &SplashScreen::finished, [dashboard, splash]() {
        qDebug() << "Splash finished! Showing dashboard...";
        
        // Delete splash to free memory
        splash->deleteLater();
        
        // Show and activate dashboard
        dashboard->showMaximized();  // Show maximized for better fit
        dashboard->raise();
        dashboard->activateWindow();
        
        qDebug() << "Dashboard is now active!";
    });
    
    return app.exec();
}
