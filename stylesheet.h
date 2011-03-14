#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QFile>

class StyleSheet {

public:
    static QString commAudio();
};

QString StyleSheet::commAudio() { 
    QString style;
    QFile file("commAudio.qss");

    file.open(QIODevice::ReadOnly);
    style = file.readAll();
    file.close();

    return style;
}

#endif // STYLESHEET_H
