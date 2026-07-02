#ifndef LEARNSECTION_H
#define LEARNSECTION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include "database/DatabaseManager.h" // For Challenge struct

class LessonMapWidget : public QWidget {
public:
    explicit LessonMapWidget(QWidget *parent = nullptr) : QWidget(parent) {}
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Find nodes to connect
        QList<QWidget*> nodes = this->findChildren<QWidget*>("LevelNode"); // We'll tag them

        QMap<int, QWidget*> sortedNodes;
        for (QWidget* w : nodes) {
            if (w->property("isLevelNode").toBool()) {
                sortedNodes[w->property("levelIndex").toInt()] = w;
            }
        }

        if (sortedNodes.isEmpty()) return;

        QPainterPath path;
        QPen pen(QColor("#475569"), 12, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // Slate-600 thick line
        painter.setPen(pen);

        // Start at first node center
        if (sortedNodes.contains(0)) {
            path.moveTo(sortedNodes[0]->geometry().center());
        }

        for (int i = 0; i < sortedNodes.size() - 1; ++i) {
            if (!sortedNodes.contains(i) || !sortedNodes.contains(i+1)) continue;
            
            QPoint p1 = sortedNodes[i]->geometry().center();
            QPoint p2 = sortedNodes[i+1]->geometry().center();

            // Control points for bezier curve (vertical flow)
            QPoint c1(p1.x(), p1.y() + 60);
            QPoint c2(p2.x(), p2.y() - 60);

            path.cubicTo(c1, c2, p2);
        }
        

        // === Premium Gradient Path ===
        // 1. Draw darker border (shadow)
        QPen penBorder(QColor("#764ba2"), 14, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // Purple
        painter.setPen(penBorder);
        painter.drawPath(path);

        // 2. Draw main gradient path
        QPen penBody(QColor("#f093fb"), 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); // Pink
        painter.setPen(penBody);
        painter.drawPath(path);
    }
};

class LearnSection : public QWidget {
    Q_OBJECT
public:
    explicit LearnSection(QWidget *parent = nullptr);
    void refresh();

signals:
    void startChallenge(int id);

private:
    void clearLayout();
    void renderPath();
    QWidget* createLevelNode(const Challenge& c, bool locked, int index);

    LessonMapWidget *m_mapWidget;  // Replaces m_scrollLayout container
    QList<QWidget*> m_nodes;       // Keep track of nodes for drawing lines
};

#endif // LEARNSECTION_H
