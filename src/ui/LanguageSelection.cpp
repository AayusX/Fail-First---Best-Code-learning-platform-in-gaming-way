#include "LanguageSelection.h"
#include "database/DatabaseManager.h"
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>

LanguageSelection::LanguageSelection(QWidget *parent) : QWidget(parent) {
    // Vibrant pink to beige gradient
    setStyleSheet(
        "QWidget { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #e3b1c1, stop:0.5 #e8caba, stop:1 #d8d6ab); "
        "}"
    );
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    
    // Header
    QLabel *title = new QLabel("Your Debugging Journey");
    title->setStyleSheet("font-size: 32px; font-weight: 800; color: #0f172a;");
    title->setAlignment(Qt::AlignCenter);
    
    QLabel *sub = new QLabel("Pick up where you left off and master new languages through hands-on debugging challenges.");
    sub->setStyleSheet("font-size: 16px; color: #64748b;");
    sub->setAlignment(Qt::AlignCenter);
    
    mainLayout->addWidget(title);
    mainLayout->addWidget(sub);
    
    // Grid
    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(25);
    
    grid->addWidget(createCard("C++", "cpp", "Master pointers, memory management leaks, and segfaults.", 0, "#3b82f6"), 0, 0);
    grid->addWidget(createCard("C", "c", "Master pointers, malloc/free, and memory management in the mother of all languages.", 0, "#10b981"), 0, 1);
    grid->addWidget(createCard("Java", "java", "Master OOP, exceptions, and the JVM - write once, run anywhere!", 0, "#f59e0b"), 0, 2);
    // Add more languages as challenge sets are created...
    
    mainLayout->addLayout(grid);
    mainLayout->addStretch();
}

QWidget* LanguageSelection::createCard(const QString& name, const QString& code, const QString& desc, int progress, const QString& color) {
    QFrame *card = new QFrame;
    card->setFixedSize(300, 320);
    card->setStyleSheet("background-color: white; border-radius: 16px; border: 1px solid #e2e8f0;");
    
    QVBoxLayout *v = new QVBoxLayout(card);
    v->setContentsMargins(25, 25, 25, 25);
    
    // Top Row: Icon + Progress
    QHBoxLayout *top = new QHBoxLayout;
    QLabel *icon = new QLabel(name); // Placeholder for icon
    icon->setStyleSheet(QString("background-color: %1; color: white; padding: 5px 10px; border-radius: 8px; font-weight: bold;").arg(color));
    
    // Progress Circle logic (simplified as text for now)
    QLabel *prog = new QLabel(QString::number(progress) + "%");
    prog->setStyleSheet("color: #64748b; font-weight: bold;");
    
    top->addWidget(icon);
    top->addStretch();
    top->addWidget(prog);
    
    QLabel *title = new QLabel(name);
    title->setStyleSheet("font-size: 20px; font-weight: 800; margin-top: 10px; color: #1e293b;");
    
    QLabel *d = new QLabel(desc);
    d->setWordWrap(true);
    d->setStyleSheet("color: #475569; font-size: 14px; margin-top: 5px;");
    d->setAlignment(Qt::AlignTop);
        
    QPushButton *btn = new QPushButton("Continue Debugging →");
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; border-radius: 8px; padding: 12px; font-weight: bold; border-bottom: 3px solid #1d4ed8; }"
        "QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }"
    ).arg(color));
    
    // Adjust button border color darker
    
    connect(btn, &QPushButton::clicked, [this, code](){
        DatabaseManager::instance().setLanguage(code);
        emit languageSelected(code);
    });
    
    v->addLayout(top);
    v->addWidget(title);
    v->addWidget(d);
    v->addStretch();
    v->addWidget(btn);
    
    return card;
}
