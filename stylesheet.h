#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QFile>

/**
 * Contains static methods that return details on how the gui should be styled.
 */
class StyleSheet {

public:
    /**
     * Gets the style info for the CommAudio project.
     *
     * @author Dean Morin
     * @return The style info formatted as a QString.
     */
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
