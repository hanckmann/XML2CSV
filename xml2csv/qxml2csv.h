#ifndef QXML2CSV_H
#define QXML2CSV_H

#include <QFile>
#include <QStack>
#include <QString>
#include <QVector>
#include <QXmlDefaultHandler>

#include "QIndexMap.h"
#include "QTuple.h"

class QXML2CSV : public QXmlDefaultHandler
{
public:
    explicit QXML2CSV();

    bool parse(const QFile &sourceXML, const QFile &csvFile, const int &splitLevel, const QString &csvSeparator);

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
    void printToScreen();
    void printToScreenTableHeader();

    int splitLevel;
    int currentLevel;
    int currentRow;
    uint currentColumnCount;
    QString csvSeparator;
    QFile csvFile;

    QStack<QString> nameStack;
    QIndexMap<QString> columnNames;
    QVector< QTuple<int> > columnCountChange; // Tuple(row, columnCount)
    QMap<QString, QString> columnData;

};

#endif // QXML2CSV_H
