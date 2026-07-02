#include "AvatarSelector.h"
#include "StyleHelper.h"
#include "database/DatabaseManager.h"
#include <QVBoxLayout>

AvatarSelector::AvatarSelector(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Choose Your Avatar");
    setModal(true);
    resize(600, 500);
    setStyleSheet("background-color: #0f172a;");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel *title = new QLabel("Select Your Avatar");
    title->setStyleSheet("font-size: 24px; font-weight: 800; color: #818cf8; margin: 20px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);
    
    // Preview
    m_previewLabel = new QLabel("👨‍💻");
    m_previewLabel->setStyleSheet("font-size: 80px; background-color: #1e293b; border-radius: 60px; padding: 20px;");
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setFixedSize(140, 140);
    QHBoxLayout *previewLayout = new QHBoxLayout;
    previewLayout->addStretch();
    previewLayout->addWidget(m_previewLabel);
    previewLayout->addStretch();
    mainLayout->addLayout(previewLayout);
    
    // Avatar Grid
    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(15);
    
    QList<QString> avatars = getAvatarList();
    int row = 0, col = 0;
    for (const QString &avatar : avatars) {
        QPushButton *btn = new QPushButton(avatar);
        btn->setFixedSize(70, 70);
        btn->setStyleSheet(
            "QPushButton { "
            "  background-color: #1e293b; "
            "  border: 2px solid #334155; "
            "  border-radius: 35px; "
            "  font-size: 32px; "
            "} "
            "QPushButton:hover { "
            "  background-color: #334155; "
            "  border: 2px solid #818cf8; "
            "  transform: scale(1.1); "
            "} "
            "QPushButton:pressed { "
            "  background-color: #1e40af; "
            "}"
        );
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("avatar", avatar);
        connect(btn, &QPushButton::clicked, this, &AvatarSelector::onAvatarClicked);
        
        grid->addWidget(btn, row, col);
        col++;
        if (col >= 6) { col = 0; row++; }
    }
    
    mainLayout->addLayout(grid);
    mainLayout->addStretch();
    
    // Confirm Button
    QPushButton *confirmBtn = new QPushButton("Confirm Selection");
    confirmBtn->setStyleSheet(StyleHelper::getButtonStyle("primary"));
    confirmBtn->setFixedHeight(50);
    connect(confirmBtn, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(confirmBtn);
    
    m_selectedAvatar = DatabaseManager::instance().getAvatar();
    m_previewLabel->setText(m_selectedAvatar);
}

void AvatarSelector::onAvatarClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        m_selectedAvatar = btn->property("avatar").toString();
        m_previewLabel->setText(m_selectedAvatar);
        emit avatarSelected(m_selectedAvatar);
    }
}

QList<QString> AvatarSelector::getAvatarList() {
    return {
        "👨‍💻", "👩‍💻", "🧑‍💻",  // Programmers
        "🦊", "🐱", "🐶", "🐼", "🐨", "🦁",  // Animals
        "🤖", "👾", "🎮", "⚡", "🔥", "💎",  // Tech/Gaming
        "🌟", "✨", "🚀", "🎯", "💡", "🧠",  // Icons
        "🦄", "🐉", "🦅"  // Mystical
    };
}
