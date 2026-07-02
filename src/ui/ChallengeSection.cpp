#include "ChallengeSection.h"
#include "StyleHelper.h"
#include "AnimationHelper.h"
#include "core/Validator.h"
#include "core/EconomyManager.h"
#include "core/GamificationEngine.h"
#include "NotificationManager.h"
#include "core/CompilerManager.h"
#include <QButtonGroup>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QRadioButton>
#include <QRegularExpression>
#include <QSplitter>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QThread>
#include <QList>
#include <QTimer>

// --- Syntax Highlighter (Kept Inline for functionality) ---
class CppHighlighter : public QSyntaxHighlighter {
public:
    CppHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
        HighlightingRule rule;
        keywordFormat.setForeground(QColor("#7b1fa2")); // Purple 800 (Dark for light bg)
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywords = { "class", "const", "enum", "explicit", "friend", "inline", "namespace", "operator", "private", "protected", "public", "signals", "slots", "static", "struct", "template", "typedef", "typename", "union", "virtual", "volatile", "int", "char", "double", "float", "bool", "void", "if", "else", "for", "while", "return", "new", "delete", "true", "false", "nullptr" };
        for (const QString &pattern : keywords) {
            rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
        stringFormat.setForeground(QColor("#388e3c")); // Green 700 (Dark for light bg)
        rule.pattern = QRegularExpression("\".*\"");
        rule.format = stringFormat;
        highlightingRules.append(rule);
        commentFormat.setForeground(QColor("#757575")); // Gray 600
        rule.pattern = QRegularExpression("//[^\n]*");
        rule.format = commentFormat;
        highlightingRules.append(rule);
    }
protected:
    void highlightBlock(const QString &text) override {
        for (const HighlightingRule &rule : highlightingRules) {
            QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }
private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
};

class PythonHighlighter : public QSyntaxHighlighter {
public:
    PythonHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
        HighlightingRule rule;
        keywordFormat.setForeground(QColor("#7b1fa2")); 
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywords = { "and", "as", "assert", "async", "await", "break", "class", "continue", "def", "del", "elif", "else", "except", "False", "finally", "for", "from", "global", "if", "import", "in", "is", "lambda", "None", "nonlocal", "not", "or", "pass", "raise", "return", "True", "try", "while", "with", "yield" };
        for (const QString &pattern : keywords) {
            rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
        stringFormat.setForeground(QColor("#388e3c")); 
        rule.pattern = QRegularExpression("('|\").*?('|\")");
        rule.format = stringFormat;
        highlightingRules.append(rule);
        commentFormat.setForeground(QColor("#757575")); 
        rule.pattern = QRegularExpression("#[^\n]*");
        rule.format = commentFormat;
        highlightingRules.append(rule);
    }
protected:
    void highlightBlock(const QString &text) override {
        for (const HighlightingRule &rule : highlightingRules) {
            QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }
private:
    struct HighlightingRule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
};

class JavaHighlighter : public QSyntaxHighlighter {
public:
    JavaHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
        HighlightingRule rule;
        keywordFormat.setForeground(QColor("#7b1fa2")); 
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywords = { "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char", "class", "const", "continue", "default", "do", "double", "else", "enum", "extends", "final", "finally", "float", "for", "goto", "if", "implements", "import", "instanceof", "int", "interface", "long", "native", "new", "package", "private", "protected", "public", "return", "short", "static", "strictfp", "super", "switch", "synchronized", "this", "throw", "throws", "transient", "try", "void", "volatile", "while", "true", "false", "null", "String", "System" };
        for (const QString &pattern : keywords) {
            rule.pattern = QRegularExpression("\\b" + pattern + "\\b");
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }
        stringFormat.setForeground(QColor("#388e3c")); 
        rule.pattern = QRegularExpression("\".*?\"");
        rule.format = stringFormat;
        highlightingRules.append(rule);
        commentFormat.setForeground(QColor("#757575")); 
        rule.pattern = QRegularExpression("//[^\n]*");
        rule.format = commentFormat;
        highlightingRules.append(rule);
    }
protected:
    void highlightBlock(const QString &text) override {
        for (const HighlightingRule &rule : highlightingRules) {
            QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }
private:
    struct HighlightingRule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
};

// --- ChallengeSection Implementation ---

ChallengeSection::ChallengeSection(QWidget *parent) : QWidget(parent) {
    m_stack = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(m_stack);
    
    // --- 1. Story Page ---
    m_storyPage = new QWidget;
    QVBoxLayout *storyLayout = new QVBoxLayout(m_storyPage);
    storyLayout->setContentsMargins(40, 40, 40, 40);
    
    m_storyTitle = new QLabel("Story Title");
    m_storyTitle->setStyleSheet("font-size: 36px; font-weight: 900; color: #6d8b85; margin-bottom: 20px; letter-spacing: -0.5px;");
    
    m_storyContent = new QTextBrowser;
    m_storyContent->setStyleSheet("font-size: 18px; line-height: 1.7; color: #5a4a42; border: none; background: transparent;");
    
    m_storyNextBtn = new QPushButton("Next Challenge →");
    m_storyNextBtn->setCursor(Qt::PointingHandCursor);
    m_storyNextBtn->setStyleSheet(StyleHelper::getButtonStyle("primary"));
    connect(m_storyNextBtn, &QPushButton::clicked, this, &ChallengeSection::onStoryNext);
    
    storyLayout->addWidget(m_storyTitle);
    storyLayout->addWidget(m_storyContent);
    storyLayout->addWidget(m_storyNextBtn, 0, Qt::AlignRight);
    
    // --- 2. Quiz Page ---
    // --- 2. Quiz Page (Styled as cards in a white container) ---
    m_quizPage = new QWidget;
    m_quizPage->setStyleSheet("background-color: transparent;"); // Inherit main window
    QVBoxLayout *quizRoot = new QVBoxLayout(m_quizPage);
    quizRoot->setContentsMargins(40, 20, 40, 40);
    
    QLabel *quizHeader = new QLabel("Knowledge Check");
    quizHeader->setStyleSheet("font-family: 'Inter'; font-size: 26px; font-weight: 800; color: #6d8b85; letter-spacing: 0.5px; text-transform: uppercase; margin-bottom: 20px;");
    
    QWidget *scrollContent = new QWidget;
    scrollContent->setStyleSheet("background-color: transparent;");
    m_quizLayout = new QVBoxLayout(scrollContent);
    m_quizLayout->setSpacing(20);
    
    QScrollArea *scroll = new QScrollArea;
    scroll->setWidget(scrollContent);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent;");
    
    m_quizSubmitBtn = new QPushButton("Submit Answers");
    m_quizSubmitBtn->setStyleSheet(StyleHelper::getButtonStyle("success"));
    connect(m_quizSubmitBtn, &QPushButton::clicked, this, &ChallengeSection::onQuizSubmit);
    
    quizRoot->addWidget(quizHeader);
    quizRoot->addWidget(scroll);
    quizRoot->addWidget(m_quizSubmitBtn, 0, Qt::AlignRight);
    
    // --- 3. Code Page (Original ChallengeSection Logic) ---
    m_codePage = new QWidget;
    QHBoxLayout *codeLayout = new QHBoxLayout(m_codePage);
    codeLayout->setContentsMargins(20, 20, 20, 20);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    
    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    QPushButton *backBtn = new QPushButton("← Back");
    backBtn->setStyleSheet("border: none; color: #6d8b85; font-weight: 600; text-align: left; font-size: 15px;");
    connect(backBtn, &QPushButton::clicked, this, &ChallengeSection::onBackClicked);
    
    m_title = new QLabel("Title");
    m_title->setStyleSheet(StyleHelper::getLabelStyle("header"));
    m_title->setWordWrap(true);
    
    m_desc = new QLabel("Desc");
    m_desc->setStyleSheet(StyleHelper::getLabelStyle("body"));
    m_desc->setWordWrap(true);
    
    m_hintBtn = new QPushButton("💡 Hint (-10 Bits)");
    m_hintBtn->setStyleSheet(StyleHelper::getButtonStyle("primary"));
    connect(m_hintBtn, &QPushButton::clicked, this, &ChallengeSection::onHintClicked);
    
    leftLayout->addWidget(backBtn);
    leftLayout->addWidget(m_title);
    leftLayout->addWidget(m_desc);
    leftLayout->addStretch();
    leftLayout->addWidget(m_hintBtn);

    m_editor = new QPlainTextEdit;
    m_editor->setStyleSheet(StyleHelper::getCodeEditorStyle());
    
    QWidget *rightPanel = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    QLabel *outLbl = new QLabel("Output:");
    outLbl->setStyleSheet("font-weight: bold;");
    
    m_output = new QTextBrowser;
    m_output->setStyleSheet(
        "background-color: #cbe0dc; "
        "color: #3a4a47; "
        "border: 2px solid #a3cac3; "
        "border-radius: 20px; "
        "padding: 12px; "
        "font-family: 'JetBrains  Mono';"
    );
    
    m_runBtn = new QPushButton("🚀 Run & Validate");
    m_runBtn->setStyleSheet(StyleHelper::getButtonStyle("success"));
    connect(m_runBtn, &QPushButton::clicked, this, &ChallengeSection::onRunClicked);
    
    rightLayout->addWidget(outLbl);
    rightLayout->addWidget(m_output);
    rightLayout->addWidget(m_runBtn);

    splitter->addWidget(leftPanel);
    splitter->addWidget(m_editor);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(1, 2); 
    codeLayout->addWidget(splitter);

    // Add pages to stack
    m_stack->addWidget(m_storyPage);
    m_stack->addWidget(m_quizPage);
    m_stack->addWidget(m_codePage);
}

void ChallengeSection::loadChallenge(int id) {
    m_currentChallenge = DatabaseManager::instance().getChallenge(id);
    
    // Setup Story
    m_storyTitle->setText(m_currentChallenge.title);
    
    // Apply Document CSS for Headers (Purple) and Body (Green)
    m_storyContent->document()->setDefaultStyleSheet(StyleHelper::getStoryDocumentCSS());
    
    // Process story text to HTML and display
    QString processed = processStoryText(m_currentChallenge.story_text);
    m_storyContent->setHtml(processed);
    
    // Setup Quiz (Parse JSON)
    QJsonDocument doc = QJsonDocument::fromJson(m_currentChallenge.quiz_data.toUtf8());
    QJsonArray questions = doc.array();
    
    // Clear old quiz
    QLayoutItem *child;
    while ((child = m_quizLayout->takeAt(0)) != 0) {
        if(child->widget()) delete child->widget();
        delete child;
    }
    m_quizAnswers.clear();
    m_quizOptionGroups.clear();
    
    int qIdx = 1;
    for(const auto &val : questions) {
        QJsonObject q = val.toObject();
        QLabel *lbl = new QLabel(q["question"].toString());
        lbl->setStyleSheet("font-family: 'Inter'; font-size: 20px; font-weight: 800; color: #6d8b85; margin-bottom: 15px; margin-top: 20px;");
        lbl->setWordWrap(true);
        m_quizLayout->addWidget(lbl);
        
        QJsonArray opts = q["options"].toArray();
        QList<QRadioButton*> group;
        QButtonGroup *btnGroup = new QButtonGroup(this); 
        
        for(int i=0; i<opts.size(); ++i) {
            QRadioButton *rb = new QRadioButton(opts[i].toString());
            rb->setCursor(Qt::PointingHandCursor);
            rb->setStyleSheet(StyleHelper::getQuizOptionStyle());
            m_quizLayout->addWidget(rb);
            group.append(rb);
            btnGroup->addButton(rb, i);
        }
        m_quizOptionGroups.append(group);
        m_quizAnswers.append(q["answer"].toInt());
    }
    m_quizLayout->addStretch();
    
    // Setup Code
    m_title->setText(m_currentChallenge.title);
    m_desc->setText(m_currentChallenge.description);
    m_editor->setPlainText(m_currentChallenge.buggyCode);
    m_output->clear();
    m_hintIndex = 0; // Reset hint index for new challenge
    setupHighlighter(m_currentChallenge.language);
    
    // Start at Story
    showStory();
}

void ChallengeSection::showStory() {
    m_stage = LessonStage::Story;
    m_stack->setCurrentWidget(m_storyPage);
    AnimationHelper::fadeIn(m_storyPage);
    AnimationHelper::slideInUp(m_storyContent, 20);
}

void ChallengeSection::onStoryNext() {
    showQuiz();
}

void ChallengeSection::showQuiz() {
    m_stage = LessonStage::Quiz;
    m_stack->setCurrentWidget(m_quizPage);
    AnimationHelper::fadeIn(m_quizPage);
    // Animate quiz items sequentially? For now just main fade
}

void ChallengeSection::onQuizSubmit() {
    // Validate Answers
    int correctCount = 0;
    for(int i=0; i<m_quizAnswers.size(); ++i) {
        int correctIdx = m_quizAnswers[i];
        QList<QRadioButton*> group = m_quizOptionGroups[i];
        if (correctIdx >= 0 && correctIdx < group.size()) {
            if (group[correctIdx]->isChecked()) correctCount++;
        }
    }
    
    if (correctCount == m_quizAnswers.size()) {
        NotificationManager::instance().showNotification("Quiz Perfect!", "Proceeding to Code Challenge...", "success");
        showCode();
    } else {
        NotificationManager::instance().showNotification("Try Again", QString("You got %1/%2 correct.").arg(correctCount).arg(m_quizAnswers.size()), "error");
        AnimationHelper::shake(m_quizSubmitBtn); // Shake button on error
    }
}

void ChallengeSection::showCode() {
    m_stage = LessonStage::Code;
    m_stack->setCurrentWidget(m_codePage);
    AnimationHelper::fadeIn(m_codePage);
}

void ChallengeSection::onRunClicked() {
    m_output->setText("Running...");
    m_runBtn->setEnabled(false);
    
    QString input = ""; 
    QString expected = "";
    
    QJsonDocument doc = QJsonDocument::fromJson(m_currentChallenge.tests.toUtf8());
    QJsonArray arr = doc.array();
    if (!arr.isEmpty()) {
        QJsonObject t = arr.at(0).toObject();
        input = t["input"].toString();
        expected = t["expected"].toString();
    }
    
    QString lang = m_currentChallenge.language;
    QString code = m_editor->toPlainText();
    QString sol = m_currentChallenge.solutionCode;
    int xp = m_currentChallenge.xp;
    int id = m_currentChallenge.id;
    bool solved = m_currentChallenge.solved;

    // Check for missing compiler
    if (!CompilerManager::instance().isCompilerAvailable(lang)) {
        QMessageBox::warning(this, "Compiler Missing", 
            CompilerManager::instance().getUnavailableMessage(lang) + 
            "\n\nNote: Because the compiler is missing, we will use Static Analysis. Your code must exactly match the expected solution format to pass.");
    }

    // Use QThread::create for background execution to prevent UI freezing
    QThread* thread = QThread::create([=]() {
        Validator v;
        ValidationResult res = v.validate(lang, code, input, sol, expected);
        
        // Return to main thread to update UI
        QMetaObject::invokeMethod(const_cast<ChallengeSection*>(this), [=]() {
            m_runBtn->setEnabled(true);
            
            QString html = "<pre>" + res.output + "</pre>";
            if (!res.error.isEmpty()) {
                 html += "<p style='color: red'>" + res.error + "</p>";
            }
            
            bool correct = res.passed;
            if (!correct && !expected.isEmpty() && res.output.trimmed() == expected.trimmed()) {
                correct = true;
            }
            
            if (correct) {
                html += "<h3 style='color: #22c55e'>✅ PASSED</h3>";
                m_output->setHtml(html);
                
                if (!solved) {
                    DatabaseManager::instance().markSolved(id);
                    GamificationEngine::instance().addXP(xp);
                    DatabaseManager::instance().addBits(100); 
                    GamificationEngine::instance().completeDailyGoal();
                    
                    if (xp >= 400) {
                         DatabaseManager::instance().addBits(500);
                         NotificationManager::instance().showNotification("Lesson Complete!", QString("+%1 XP & +600 Bits!").arg(xp), "success");
                    } else {
                        NotificationManager::instance().showNotification("Lesson Complete!", QString("+%1 XP & +100 Bits").arg(xp), "success");
                    }
                    
                    AnimationHelper::confetti(this);
                    QTimer::singleShot(1200, this, [this]() {
                        emit backToLearn();
                    });
                } else {
                     NotificationManager::instance().showNotification("Great Job!", "Practice makes perfect!", "success");
                     AnimationHelper::confetti(this);
                }
            } else {
                html += "<h3 style='color: #ef4444'>❌ FAILED</h3>";
                html += "<p>Expected: " + expected + "</p>";
                m_output->setHtml(html);
                AnimationHelper::shake(m_runBtn);
                EconomyManager::instance().loseHeart();
                NotificationManager::instance().showNotification("Bug Found!", "You lost a heart. Debug it!", "error");
            }
        }, Qt::QueuedConnection);
    });
    
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void ChallengeSection::onHintClicked() {
    QJsonDocument doc = QJsonDocument::fromJson(m_currentChallenge.hints.toUtf8());
    QJsonArray arr = doc.array();
    if (arr.isEmpty()) {
        NotificationManager::instance().showNotification("No Hints", "No hints available for this challenge.", "error");
        return;
    }
    if (!EconomyManager::instance().spendBits(10)) {
        NotificationManager::instance().showNotification("Shop", "Not enough bits!", "error");
        return;
    }
    // Show next hint, cycling through all available ones
    QString hint = arr.at(m_hintIndex % arr.size()).toString();
    m_hintIndex++;
    QString hintNum = QString("Hint %1/%2").arg(qMin(m_hintIndex, (int)arr.size())).arg(arr.size());
    QMessageBox::information(this, hintNum, hint);
}

void ChallengeSection::onBackClicked() {
    emit backToLearn();
}

void ChallengeSection::setupHighlighter(const QString& lang) {
    // Delete old highlighter if exists (proper cleanup)
    if (m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;
    }
    
    // Create new highlighter (parented to document for automatic cleanup)
    if (lang == "cpp" || lang == "c") {
        m_highlighter = new CppHighlighter(m_editor->document());
    } else if (lang == "python") {
        m_highlighter = new PythonHighlighter(m_editor->document());
    } else if (lang == "java") {
        m_highlighter = new JavaHighlighter(m_editor->document());
    }
}

QString ChallengeSection::processStoryText(const QString &raw) {
    QString md = raw;
    
    // 1. Convert simple Markdown to HTML (Simplified)
    // Actually, let's use Qt's text engine to do the heavy lifting for layout,
    // but syntax highlighting in markdown blocks is hard.
    // Strategy: We will manually replace ```cpp ... ``` blocks with HTML tables or pre blocks
    // containing COLORIZED spans.
    
    QRegularExpression codeBlock("```([a-zA-Z]+)?(.*?)```", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator i = codeBlock.globalMatch(md);
    
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString lang = match.captured(1).trimmed();
        QString content = match.captured(2).trimmed();
        
        // Colorize Keywords
        content.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;"); // Escape HTML
        
        QStringList keywords;
        if (lang == "python") {
            keywords = { "def ", "class ", "return ", "if ", "elif ", "else:", "import ", "print", "from ", "for ", "while ", "True", "False" };
        } else if (lang == "java") {
            keywords = { "int ", "void ", "return ", "if ", "else ", "class ", "public ", "private ", "import ", "String", "System.out.println" };
        } else {
            keywords = { "int ", "void ", "return ", "if ", "else ", "class ", "public:", "private:", "include ", "std::", "cout", "endl" };
        }
        
        for (const QString &k : keywords) {
            content.replace(k, QString("<span style='color: #7b1fa2; font-weight: bold;'>%1</span>").arg(k)); // Purple 800
        }
    
    // Strings -> Dark Green (proper replacement using match iterator)
    {
        QRegularExpression strReg("\"[^\"]*\"");
        QString result;
        int lastEnd = 0;
        QRegularExpressionMatchIterator it = strReg.globalMatch(content);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            result += content.mid(lastEnd, m.capturedStart() - lastEnd);
            result += "<span style='color: #388e3c;'>" + m.captured(0) + "</span>";
            lastEnd = m.capturedEnd();
        }
        result += content.mid(lastEnd);
        content = result;
    }
    
    // Comments -> Gray (proper replacement using match iterator)
    {
        QRegularExpression comReg(lang == "python" ? "#[^\n]*" : "//[^\n]*");
        QString result;
        int lastEnd = 0;
        QRegularExpressionMatchIterator it = comReg.globalMatch(content);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            result += content.mid(lastEnd, m.capturedStart() - lastEnd);
            result += "<span style='color: #757575;'>" + m.captured(0) + "</span>";
            lastEnd = m.capturedEnd();
        }
        result += content.mid(lastEnd);
        content = result;
    }

        // Wrap in styled div
        QString htmlBlock = QString("<pre style='background-color: #fafafa; color: #37474f; padding: 15px; border: 1px solid #e0e0e0; border-radius: 8px;'>%1</pre>").arg(content);
        
        // Replace in original string
        md.replace(match.captured(0), htmlBlock);
    }
    
    // Convert remaining basic markdown Headers to HTML for our CSS to catch
    // # Header -> <h1>Header</h1>
    md.replace(QRegularExpression("^# (.*)$", QRegularExpression::MultilineOption), "<h1>\\1</h1>");
    md.replace(QRegularExpression("^## (.*)$", QRegularExpression::MultilineOption), "<h2>\\1</h2>");
    md.replace(QRegularExpression("^### (.*)$", QRegularExpression::MultilineOption), "<h3>\\1</h3>");
    
    // Newlines to <br> or <p>
    md.replace("\n", "<br>");
    
    return md;
}
