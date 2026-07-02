#include "LearnSection.h"
#include "StyleHelper.h"

#include "AnimationHelper.h"
#include <QScrollArea>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <cmath>
#include <QtMath> // For sin/cos


// LearnSection main implementation


LearnSection::LearnSection(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    // Vibrant sage to mint gradient
    scroll->setStyleSheet(
        "QScrollArea { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #d8d6ab, stop:1 #a3cac3); "
        "  border: none; "
        "} "
        "QScrollBar:vertical { "
        "  background: #e8e7cf; "
        "  width: 14px; "
        "  border-radius: 7px; "
        "} "
        "QScrollBar::handle:vertical { "
        "  background: #bebd92; "
        "  border-radius: 7px; "
        "  min-height: 30px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "  background: #a3a27a; "
        "}"
    );
    
    m_mapWidget = new LessonMapWidget;
    m_mapWidget->setStyleSheet("background: transparent;");
    m_mapWidget->setMinimumWidth(600);

    scroll->setWidget(m_mapWidget);
    mainLayout->addWidget(scroll);

    refresh();
}

void LearnSection::refresh() {
    clearLayout();
    renderPath();
}

void LearnSection::clearLayout() {
    qDeleteAll(m_mapWidget->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
    m_nodes.clear();
}

void LearnSection::renderPath() {
    QList<Challenge> challenges = DatabaseManager::instance().getAllChallenges();
    int currentLevel = 0;
    bool previousSolved = true;
    
    // Zig-Zag Parameters
    int startY = 50;
    int spacingY = 160;
    int centerX = m_mapWidget->width() > 0 ? m_mapWidget->width() / 2 : 300; // Fallback or dynamic
    // Actually we need to force m_mapWidget size if not shown yet. 
    // Let's assume a "virtual" width of 600 for calc, and mapWidget centers itself in scrollarea naturally.
    centerX = 300; 
    int amplitude = 120; // How wide the zigzag is

    for (const Challenge &c : challenges) {
        bool locked = !previousSolved;
        if (c.solved) previousSolved = true;
        else previousSolved = false;
        if (c.id == 1) locked = false;

        QWidget *node = createLevelNode(c, locked, currentLevel);
        node->setParent(m_mapWidget); // Important manual parenting
        
        // Calculate Position (Sine wave)
        // x = center + sin(index) * amp
        int x = centerX + (int)(std::sin(currentLevel * 0.8) * amplitude) - (node->width() / 2);
        int y = startY + (currentLevel * spacingY);
        
        node->move(x, y);
        
        m_nodes.append(node);

        // Staggered Animation — start hidden, zoom in after delay
        node->hide();
        QTimer::singleShot(currentLevel * 100, [node](){
            AnimationHelper::zoomIn(node); // New animation type? Or just slide
        });
        
        currentLevel++;
    }
    
    // Resize map widget to fit all nodes
    m_mapWidget->setFixedSize(600, startY + (currentLevel * spacingY) + 200);
}

QWidget* LearnSection::createLevelNode(const Challenge& c, bool locked, int index) {
    QWidget *wrapper = new QWidget;
    wrapper->setFixedSize(200, 160);
    QVBoxLayout *v = new QVBoxLayout(wrapper);
    v->setSpacing(10);
    v->setAlignment(Qt::AlignCenter);

    QPushButton *btn = new QPushButton;
    btn->setFixedSize(80, 80);
    
    // Dual-tone colors for 3D effect
    QString baseColor = locked ? "#94a3b8" : "#6366f1"; // Slate-400 vs Indigo-500
    QString shadowColor = locked ? "#64748b" : "#4f46e5"; // Slate-500 vs Indigo-600
    
    if (c.solved) {
        baseColor = "#fbbf24"; // Amber-400
        shadowColor = "#d97706"; // Amber-600
    }
    
    btn->setStyleSheet(QString(
        "QPushButton { "
        "  background-color: %1; "
        "  border-radius: 40px; "
        "  border: none; "
        "  border-bottom: 8px solid %2; " // 3D Effect
        "  color: white; "
        "  font-weight: bold; "
        "  font-size: 24px; "
        "  margin-bottom: 4px; "
        "}"
        "QPushButton:pressed { "
        "  border-bottom: 0px solid %2; "
        "  margin-top: 8px; " // Push down effect
        "  margin-bottom: 0px; "
        "}"
    ).arg(baseColor).arg(shadowColor));
    
    // Icon
    if (locked) btn->setText("🔒");
    else if (c.solved) btn->setText("✅");
    else btn->setText("★");

    connect(btn, &QPushButton::clicked, [this, c, locked](){
        if (!locked) emit startChallenge(c.id);
    });

    QLabel *lbl = new QLabel(c.title);
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setWordWrap(true);
    lbl->setStyleSheet("font-weight: 600; font-size: 14px; color: #3a4a47;"); // Dark green for readability

    v->addWidget(btn);
    v->addWidget(lbl);

    // Tag for painting
    wrapper->setProperty("isLevelNode", true);
    wrapper->setProperty("levelIndex", index);
    wrapper->setObjectName("LevelNode"); // for findChildren

    return wrapper;
}
