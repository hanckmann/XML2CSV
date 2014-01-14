#ifndef QXML2CSV_H
#define QXML2CSV_H

#include <QFile>
#include <QStack>
#include <QString>
#include <QVector>
#include <QTextStream>
#include <QXmlDefaultHandler>

#include "QIndexMap.h"
#include "QTuple.h"

class QXML2CSV : public QXmlDefaultHandler
{
public:
    explicit QXML2CSV();

    bool parse(const QFile &sourceXML, const QFile &csvFile, const int &splitLevel, const int &maxRows);
    void setAttributeExpansion(const QStringList &attributeExpansion);
    void setCsvSeparator(const QString &csvSeparator);
    void setWriteToScreen(bool writeToScreen);
    void enableRewrite(bool rewrite);

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
    void write();
    void writeTableHeader();
    void finalizeTable();

    bool writeToScreen;
    bool rewrite;

    int splitLevel;
    int maxRows;
    int currentLevel;
    int currentRow;
    uint currentColumnCount;
    QString csvSeparator;
    QString csvFileName;
    QTextStream csvStream;

    QStack<QString> nameStack;
    QIndexMap<QString> columnNames;
    QVector< QTuple<int> > columnCountChange; // Tuple(row, columnCount)
    QMap<QString, QString> columnData;

    QMap<QString, QString> attributeExpansion;
    QMap<QString, QString> attributeExpansionInverted;
};

#endif // QXML2CSV_H
