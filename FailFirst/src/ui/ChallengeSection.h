#ifndef CHALLENGESECTION_H
#define CHALLENGESECTION_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QStackedWidget>
#include <QSplitter>
#include <QSyntaxHighlighter>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QList>
#include <QButtonGroup>
#include "database/DatabaseManager.h"

// Enum for Lesson Stage
enum class LessonStage {
    Story,
    Quiz,
    Code,
    Complete
};

class ChallengeSection : public QWidget {
    Q_OBJECT
public:
    explicit ChallengeSection(QWidget *parent = nullptr);
    void loadChallenge(int id);

signals:
    void backToLearn();

private slots:
    void onRunClicked();
    void onHintClicked();
    void onBackClicked();
    
    // Wizard Slots
    void onStoryNext();
    void onQuizSubmit();

private:
    void setupHighlighter(const QString& lang);
    void showStory();
    void showQuiz();
    void showCode();
    void showReward();
    QString processStoryText(const QString &raw); // Helper

    Challenge m_currentChallenge;
    LessonStage m_stage;
    
    // Layout Manager
    QStackedWidget *m_stack;

    // --- Story View ---
    QWidget *m_storyPage;
    QLabel *m_storyTitle;
    QTextBrowser *m_storyContent;
    QPushButton *m_storyNextBtn;
    
    // --- Quiz View ---
    QWidget *m_quizPage;
    QVBoxLayout *m_quizLayout; // Dynamic questions
    QPushButton *m_quizSubmitBtn;
    QList<int> m_quizAnswers; // Stores correct indices
    QList<QList<QRadioButton*>> m_quizOptionGroups;

    // --- Code View (Old ChallengeSection) ---
    QWidget *m_codePage;
    QLabel *m_title;
    QLabel *m_desc;
    QPlainTextEdit *m_editor;
    QTextBrowser *m_output;
    QPushButton *m_runBtn;
    QPushButton *m_hintBtn;
    
    // Syntax highlighter (prevent memory leak)
    QSyntaxHighlighter *m_highlighter = nullptr;
    // Tracks which hint to reveal next (cycles through all available hints)
    int m_hintIndex = 0;
};

#endif // CHALLENGESECTION_H
