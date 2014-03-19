#ifndef QCSV2MATRIX_H
#define QCSV2MATRIX_H

#include <QFile>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTextStream>

class QCSV2MATRIX
{
public:
    explicit QCSV2MATRIX();

    bool convert(const QFile& sourceCSVFile, const QFile& sourceMTXFile, const QString& csvSeperator, const QString& mtxSeperator, const QList<int>& rawColumns, const int& minColumns, const bool& noHeader, const bool& lastLine, const bool& unique);

private:
    bool noHeader;
    bool lastLine;
    bool unique;
    QString csvSeperator;
    QString mtxSeperator;
    int minColumns;
    QList<int> rawColumns;
    QString mtxFileName;
    QString csvFileName;

    bool stringIsNumber(const QString& str);
};

#endif // QCSV2MATRIX_H
