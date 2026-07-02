#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>

class StyleHelper {
public:
    static QString getMainWindowStyle();
    static QString getSidebarStyle();
    static QString getCardStyle();
    static QString getButtonStyle(const QString &type);
    static QString getCodeEditorStyle();
    static QString getQuizOptionStyle();
    static QString getLabelStyle(const QString &type);
    static QString getStoryDocumentCSS();
};

#endif // STYLEHELPER_H
