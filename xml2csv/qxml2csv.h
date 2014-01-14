#ifndef QXML2CSV_H
#define QXML2CSV_H

#include <QFile>
#include <QStack>
#include <QString>
#include <QXmlDefaultHandler>

#include "qindexmap.h"

class QXML2CSV : public QXmlDefaultHandler
{
public:
    explicit QXML2CSV();

    bool parse(const QFile &sourceXML, const QFile &destinationCSV, const int &splitLevel, const QString &csvSeparator);

protected:
    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);


private:
    void printRowToScreen();

    int splitLevel;
    int currentLevel;
    QString csvSeparator;
    QFile destinationCSV;

    QStack<QString> nameStack;
    QIndexMap<QString> columnNames;
    QMap<QString, QString> columnData;

};

#endif // QXML2CSV_H
