#include "StyleHelper.h"

// ================================================================
// HIGH CONTRAST MODERN THEME
// ================================================================
// Background: #1a202c (Dark Blue-Gray) to #2d3748
// Cards/Panels: #ffffff (White)
// Text: #1a202c (Very Dark) or #ffffff (White on dark)
// Primary Accent: #667eea to #764ba2 (Purple Gradient)
// Secondary Accent: #4299e1 (Blue)
// Success: #48bb78 (Green)
// Error: #e53e3e (Red)
// ================================================================

QString StyleHelper::getLabelStyle(const QString &type) {
    if (type == "header") {
        return "font-family: 'Inter', sans-serif; "
               "font-size: 32px; "
               "font-weight: 900; "
               "color: #1a202c; "  // Dark text for contrast
               "margin-bottom: 20px; "
               "letter-spacing: -0.5px;";
    } else if (type == "body") {
        return "font-family: 'Inter', sans-serif; "
               "font-size: 16px; "
               "color: #2d3748; "  // Dark gray text
               "line-height: 1.6;";
    } else if (type == "muted") {
        return "color: #718096; "  // Medium gray
               "font-size: 14px; "
               "font-weight: 500;";
    }
    return "";
}

QString StyleHelper::getButtonStyle(const QString &type) {
    if (type == "primary") {
        // Purple Gradient
        return "QPushButton { "
               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
               "    stop:0 #667eea, stop:1 #764ba2); "
               "  color: white; "
               "  border: none; "
               "  border-radius: 12px; "
               "  padding: 14px 28px; "
               "  font-weight: 700; "
               "  font-size: 16px; "
               "} "
               "QPushButton:hover { "
               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
               "    stop:0 #5a67d8, stop:1 #6b46c1); "
               "  transform: scale(1.02); "
               "} "
               "QPushButton:pressed { "
               "  background: #4c51bf; "
               "}";
    } else if (type == "success") {
        // Green Gradient
        return "QPushButton { "
               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
               "    stop:0 #48bb78, stop:1 #38a169); "
               "  color: white; "
               "  border: none; "
               "  border-radius: 12px; "
               "  padding: 10px 20px; "
               "  font-weight: 700; "
               "  font-size: 14px; "
               "} "
               "QPushButton:hover { "
               "  background: #38a169; "
               "}";
    } else if (type == "danger") {
        // Tan to Brown gradient
        return "QPushButton { "
               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
               "    stop:0 #e8caba, stop:1 #ccab99); "
               "  color: white; "
               "  border: none; "
               "  border-radius: 24px; "
               "  padding: 14px 28px; "
               "  font-weight: 700; "
               "  font-size: 16px; "
               "} "
               "QPushButton:hover { "
               "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
               "    stop:0 #e8d5cb, stop:1 #e8caba); "
               "}";
    }
    return "";
}

QString StyleHelper::getMainWindowStyle() {
    return "QMainWindow { background-color: #f7fafc; }"; // Light Gray Background
}

QString StyleHelper::getSidebarStyle() {
    return "QListWidget { "
           "  background-color: transparent; "
           "  border: none; "
           "  outline: none; "
           "} "
           "QListWidget::item { "
           "  color: #4a5568; " // Dark Gray
           "  padding: 12px 18px; "
           "  margin-bottom: 8px; "
           "  border-radius: 10px; "
           "  font-weight: 600; "
           "  font-size: 15px; "
           "} "
           "QListWidget::item:selected { "
           "  background-color: #ebf8ff; " // Light Blue
           "  color: #3182ce; " // Blue
           "} "
           "QListWidget::item:hover { "
           "  background-color: #f7fafc; "
           "}";
}

QString StyleHelper::getCodeEditorStyle() {
    return "QPlainTextEdit { "
           "  background-color: #ffffff; "
           "  color: #2d3748; "
           "  font-family: 'JetBrains Mono', 'Consolas', monospace; "
           "  font-size: 14px; "
           "  border: 1px solid #cbd5e0; "
           "  border-radius: 12px; "
           "  padding: 16px; "
           "  selection-background-color: #bee3f8; "
           "  selection-color: #2c5282; "
           "}";
}

QString StyleHelper::getCardStyle() {
    return "QFrame { "
           "  background-color: #ffffff; "
           "  border: 1px solid #e2e8f0; "
           "  border-radius: 16px; "
           "}";
}

QString StyleHelper::getQuizOptionStyle() {
    // Sage green quiz cards
    return "QRadioButton { "
           "  background-color: #e8e7cf; "  // Light sage
           "  border: 2px solid #d8d6ab; "
           "  border-radius: 20px; "  // SMOOTHER!
           "  padding: 18px 20px; "
           "  color: #5a4a42; "
           "  font-size: 16px; "
           "  font-weight: 600; "
           "} "
           "QRadioButton::indicator { width: 0px; height: 0px; } "
           "QRadioButton:checked { "
           "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
           "    stop:0 #bebd92, stop:1 #a3a27a); "
           "  border: 2px solid #a3a27a; "
           "  color: white; "
           "  font-weight: 700; "
           "} "
           "QRadioButton:hover { "
           "  border: 2px solid #bebd92; "
           "  background-color: #d8d6ab; "
           "}";
}

QString StyleHelper::getStoryDocumentCSS() {
    return "h1, h2, h3 { "
           "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
           "    stop:0 #c88ca0, stop:1 #87aca4); "  // Pink to teal
           "  -webkit-background-clip: text; "
           "  color: #6d8b85; "  //  Fallback
           "  font-family: 'Inter', sans-serif; "
           "  font-weight: 800; "
           "  margin-bottom: 12px; "
           "  letter-spacing: -0.5px; "
           "} "
           "p, li { "
           "  color: #5a4a42; "
           "  font-family: 'Inter', sans-serif; "
           "  font-size: 17px; "
           "  line-height: 1.7; "
           "} "
           "code { "
           "  font-family: 'JetBrains Mono', monospace; "
           "  background-color: #e6c6d1; "  // Soft pink
           "  color: #b16c82; "
           "  padding: 3px 8px; "
           "  border-radius: 8px; "
           "  border: 1px solid #e3b1c1; "
           "} "
           "pre { "
           "  background-color: #cbe0dc; "  // Light mint
           "  color: #3a4a47; "
           "  padding: 20px; "
           "  border: 2px solid #a3cac3; "
           "  border-radius: 16px; "
           "  margin: 16px 0; "
           "  font-family: 'JetBrains Mono', monospace; "
           "}";
}
