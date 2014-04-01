#include "qcsv2matrix.h"

#include <iostream>
#include <QDateTime>

/**
 * @brief QCSV2MATRIX::QCSV2MATRIX
 */
QCSV2MATRIX::QCSV2MATRIX() :
    noHeader(false),
    lastLine(false),
    unique(false),
    csvSeperator(";"),
    mtxSeperator(";"),
    minColumns(-1),
    mtxFileName(""),
    csvFileName("")
{
}

bool QCSV2MATRIX::convert(const QFile& sourceCSVFile, const QFile &sourceMTXFile, const QString& csvSeperator, const QString& mtxSeperator, const QList<int>& rawColumns, const int& dtColumn = -1, const QString& dtFormat = "", const int& minColumns = -1, const bool& noHeader = false, const bool& lastLine = false, const bool& unique = false)
{
    // Initialise CSV reader
    QFile csvFile(sourceCSVFile.fileName());
    if (!csvFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream csvStream(&csvFile);

    // Initialise MTX writer
    QFile mtxFile(sourceMTXFile.fileName());
    if(!mtxFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream mtxStream(&mtxFile);

    // Init variables
    int lineCount = 0;
    int uniqueValue = 0;
    QString prevLine;
    QString header;
    QList<QMap<QString, int> > columnLegends;
    QList<int> copyColumns(rawColumns);
    copyColumns.append(dtColumn);

    // Loop through file
    while (!csvStream.atEnd())
    {
        QString line = csvStream.readLine();
        // Skip empty lines
        if(line.isEmpty())
            continue;

        // Process header
        if((lineCount == 0) && !lastLine && !noHeader)
        {
            header = line;
            ++lineCount;
            continue;
        }

        // Process previous line
        if(!prevLine.isEmpty())
        {
            QStringList columnList = prevLine.split(csvSeperator);
            QString mtxLine = "";
            int index = 0;
            for(index = 0; index < columnList.size(); ++index)
            {
                QString columnValue = columnList.at(index);
                // Check if we need to extend the columnLegends
                if(index >= columnLegends.size())
                {
                    QMap<QString, int> msi;
                    columnLegends.append(msi);
                    // Check if this should be a raw column
                    if(stringIsNumber(columnValue))
                    {
                        // Add this number in the rawColumns list
                        copyColumns.append(index);
                    }
                }
                // Check if this is a raw column
                if(!copyColumns.contains(index))
                {
                    if(!columnLegends.at(index).contains(columnValue))
                    {
                        QMap<QString, int> tmpLegend;
                        tmpLegend = columnLegends.at(index);
                        int value;
                        if(unique)
                        {
                            value = uniqueValue;
                            ++uniqueValue;
                        }
                        else
                        {
                            value = columnLegends.at(index).size();
                        }
                        tmpLegend.insert(columnValue, value);
                        columnLegends.replace(index, tmpLegend);
                    }
                    columnValue.setNum(columnLegends.at(index).value(columnValue));
                }
                // Check if this is a date-time column that should be converted to unix time
                if(dtColumn == index)
                {
                    QDateTime dt = QDateTime::fromString(columnValue, dtFormat);
                    columnValue = "";
                    columnValue = QString::number(dt.toMSecsSinceEpoch());
                }
                // The columnValue is prepared for the mtx
                if(index > 0)
                    mtxLine += mtxSeperator;
                mtxLine += columnValue;
            }
            // Extend the number of columns to satisfy minColumns
            for(;index < minColumns; ++index)
            {
                mtxLine += mtxSeperator;
            }
            // Write the line to file
            mtxStream << mtxLine << "\n";
        }

        // Go to next line
        prevLine = line;
        ++lineCount;
    }

    if(lastLine)
    {
        header = prevLine;
    }
    else
    {
        //process last line
    }

    // Close the files neatly
    csvFile.close();
    mtxFile.close();

    // Serialise the legends and headers

    return true;
}

bool QCSV2MATRIX::stringIsNumber(const QString &str)
{
    bool ok;
    str.toDouble(&ok);
    return ok;
}

